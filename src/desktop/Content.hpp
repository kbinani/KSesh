#pragma once

namespace ksesh {

class Content {
#if defined(JUCE_WINDOWS)
  template <class Handle, auto *deleter>
  struct ScopedHandle {
    explicit ScopedHandle(Handle handle) : fHandle(handle) {
    }
    ~ScopedHandle() {
      deleter(fHandle);
    }
    operator Handle() const {
      return fHandle;
    }
    operator bool() const {
      return fHandle != INVALID_HANDLE_VALUE;
    }
    Handle fHandle;
  };
#endif

public:
  Content(std::u32string const &raw, std::shared_ptr<FontAdapter> const &font) : fFont(font), fRaw(raw) {
    using namespace std;
    u32string::size_type offset = 0;
    while (offset < raw.size()) {
      auto found = raw.find(U'\n', offset);
      if (found == u32string::npos) {
        auto l = make_shared<Line>(offset, raw.substr(offset), font);
        fLines.push_back(l);
        break;
      }
      auto l = make_shared<Line>(offset, raw.substr(offset, found - offset), font);
      fLines.push_back(l);
      offset = found + 1;
    }
  }

  CaretLocation closestPosition(
      std::optional<int> current,
      juce::Point<float> point,
      PresentationSetting const &setting) {
    float const padding = setting.fPadding;
    float const fontSize = setting.fFontSize;
    float const lineSpacing = setting.lineSpacing();

    if (fLines.empty()) {
      return CaretLocation(0, Direction::Forward);
    }
    int lineIndex;
    if (point.y <= padding + fontSize + lineSpacing / 2) {
      lineIndex = 0;
    } else {
      auto dy = point.y - (padding + fontSize + lineSpacing / 2);
      lineIndex = std::min<int>(
          1 + (int)floor(dy / (fontSize + lineSpacing)),
          (int)fLines.size() - 1);
    }
    auto line = fLines[lineIndex];
    float minDistance = std::numeric_limits<float>::max();
    CaretLocation nearest(line->fRawOffset, Direction::Forward);
    std::vector<CaretLocation> test;
    if (!line->fChars.empty()) {
      auto last = line->fChars[line->fChars.size() - 1];
      auto location = line->fRawOffset + last.fRawOffset + (int)last.fRaw.size();
      auto cursor = this->cursor(location, location, Direction::Forward, setting);
      if (cursor.fRect) {
        auto center = cursor.fRect->fRect.getCentre();
        float distance = hypotf(point.x - center.x, point.y - center.y);
        minDistance = distance;
        nearest = CaretLocation(location, Direction::Forward);
      }
    }
    for (int i = 0; i < (int)line->fChars.size(); i++) {
      auto ch = line->fChars[i];
      if (!ch.fSign) {
        continue;
      }
      auto start = line->fRawOffset + ch.fRawOffset;
      auto end = line->fRawOffset + ch.fRawOffset + (int)ch.fRaw.size();
      test.clear();
      if (!current || i == *current) {
        test.push_back(CaretLocation(start, Direction::Forward));
        test.push_back(CaretLocation(end, Direction::Backward));
      } else if (i < current) {
        test.push_back(CaretLocation(start, Direction::Forward));
      } else {
        test.push_back(CaretLocation(end, Direction::Backward));
      }
      for (auto it : test) {
        auto cursor = this->cursor(
            it.fLocation,
            it.fLocation,
            it.fDirection,
            setting);
        if (!cursor.fRect) {
          continue;
        }
        auto center = cursor.fRect->fRect.getCentre();
        float distance = hypotf(point.x - center.x, point.y - center.y);
        if (distance <= minDistance) {
          minDistance = distance;
          nearest = it;
        }
      }
    }
    return nearest;
  }

  std::optional<CursorLocation> cursorLocation(int location, Direction direction) {
    using namespace std;
    auto lineItr = find_if(fLines.begin(), fLines.end(), [location](shared_ptr<Line> const &line) {
      return line->fRawOffset <= location && location <= line->fRawOffset + (int)line->fRaw.size();
    });
    if (lineItr == fLines.end()) {
      return nullopt;
    }
    int lineIndex = distance(fLines.begin(), lineItr);
    shared_ptr<Line> line = *lineItr;
    if (location == line->fRawOffset) {
      return CursorLocationLeft(lineIndex, 0);
    }
    int charIndex = -1;
    for (int i = 0; i < (int)line->fChars.size(); i++) {
      auto char_ = line->fChars[i];
      auto from = line->fRawOffset + char_.fRawOffset;
      int to;
      if (i + 1 < (int)line->fChars.size()) {
        to = line->fChars[i + 1].fRawOffset + (int)line->fChars[i + 1].fRaw.size();
      } else {
        to = (int)line->fRaw.size();
      }
      to += line->fRawOffset;
      if (from <= location && location < to) {
        charIndex = i;
        break;
      }
    }
    if (charIndex < 0 || (int)line->fChars.size() <= charIndex) {
      return CursorLocationEnd(lineIndex);
    }
    auto char_ = line->fChars[charIndex];
    int firstCharIndex = charIndex;
    for (int i = charIndex; i >= 0; i--) {
      auto ch = line->fChars[i];
      if (ch.fClusterIndex == char_.fClusterIndex) {
        firstCharIndex = i;
      } else if (ch.fClusterIndex < char_.fClusterIndex) {
        break;
      }
    }
    int lastCharIndex = charIndex;
    for (int i = charIndex; i < (int)line->fChars.size(); i++) {
      auto ch = line->fChars[i];
      if (ch.fClusterIndex == char_.fClusterIndex) {
        lastCharIndex = i;
      } else if (ch.fClusterIndex > char_.fClusterIndex) {
        break;
      }
    }
    int startCharIndex = -1;
    for (int i = firstCharIndex; i <= lastCharIndex; i++) {
      auto ch = line->fChars[i];
      if (ch.fSign) {
        startCharIndex = i;
        break;
      }
    }
    if (startCharIndex < 0) {
      return nullopt;
    }
    if (char_.fClusterIndex > 0) {
      for (int i = firstCharIndex - 1; i >= 0; i--) {
        auto ch = line->fChars[i];
        if (ch.fSign && !ch.fCtrl) {
          if (location == line->fRawOffset + ch.fRawOffset + (int)ch.fRaw.size()) {
            return CursorLocationRight(lineIndex, ch.fClusterIndex, true);
          }
        }
      }
    }
    auto start = line->fChars[startCharIndex];
    if (
        line->fRawOffset + start.fRawOffset < location &&
        location <= line->fRawOffset + start.fRawOffset + (int)start.fRaw.size()) {
      return CursorLocationRight(lineIndex, start.fClusterIndex, true);
    }
    if (line->fRawOffset + start.fRawOffset + (int)start.fRaw.size() < location) {
      if (direction == Direction::Backward) {
        return CursorLocationRight(lineIndex, char_.fClusterIndex, false);
      } else if ((int)line->fClusters.size() - 1 == char_.fClusterIndex) {
        return CursorLocationEnd(lineIndex);
      } else {
        for (int i = lastCharIndex + 1; i < (int)line->fChars.size(); i++) {
          auto ch = line->fChars[i];
          if (ch.fSign) {
            return CursorLocationLeft(lineIndex, ch.fClusterIndex);
          }
        }
      }
    }
    if (charIndex <= startCharIndex) {
      if (direction == Direction::Forward || char_.fClusterIndex == 0) {
        return CursorLocationLeft(lineIndex, char_.fClusterIndex);
      }
    }
    if (direction == Direction::Forward) {
      for (int i = charIndex + 1; i < (int)line->fChars.size(); i++) {
        auto ch = line->fChars[i];
        if (ch.fSign) {
          return CursorLocationLeft(lineIndex, ch.fClusterIndex);
        }
      }
    } else {
      for (int i = charIndex - 1; i >= 0; i--) {
        auto ch = line->fChars[i];
        if (ch.fSign) {
          return CursorLocationRight(lineIndex, ch.fClusterIndex, false);
        }
      }
    }
    return nullopt;
  }

  Cursor cursor(
      int selectionStart,
      int selectionEnd,
      Direction direction,
      PresentationSetting const &setting) {
    float const fontSize = setting.fFontSize;
    float const lineSpacing = setting.lineSpacing();
    float const padding = setting.fPadding;
    float const caretExpand = setting.fCaretExpand;
    auto font = fFont.lock();
    if (!font) {
      Cursor ret;
      return ret;
    }

    auto mtx = juce::AffineTransform::translation(0, -font->fY).scaled(font->fScale * fontSize);
    if (selectionStart == selectionEnd) {
      auto location = cursorLocation(selectionStart, direction);
      if (!location) {
        Cursor ret;
        return ret;
      }
      if (std::holds_alternative<CursorLocationEnd>(*location)) {
        auto loc = std::get<CursorLocationEnd>(*location);
        auto lineIndex = loc.fLineIndex;
        float dx = padding;
        float dy = padding + (fontSize + lineSpacing) * lineIndex;
        auto line = fLines[lineIndex];
        Cursor ret;
        ret.fRect = {lineIndex, juce::Rectangle<float>(
                                    dx + line->fWidth * font->fScale * fontSize,
                                    dy,
                                    0,
                                    fontSize)};
        return ret;
      } else {
        int lineIndex;
        int clusterIndex;
        if (std::holds_alternative<CursorLocationLeft>(*location)) {
          auto loc = std::get<CursorLocationLeft>(*location);
          lineIndex = loc.fLineIndex;
          clusterIndex = loc.fClusterIndex;
        } else if (std::holds_alternative<CursorLocationRight>(*location)) {
          auto loc = std::get<CursorLocationRight>(*location);
          lineIndex = loc.fLineIndex;
          clusterIndex = loc.fClusterIndex;
        } else {
          Cursor ret;
          return ret;
        }
        float dx = padding;
        float dy = padding + (fontSize + lineSpacing) * lineIndex;
        auto line = fLines[lineIndex];
        if (clusterIndex < 0 || (int)line->fClusters.size() <= clusterIndex) {
          Cursor ret;
          ret.fRect = {lineIndex, juce::Rectangle<float>(dx, dy, 0, fontSize)};
          return ret;
        }
        auto cluster = line->fClusters[clusterIndex];
        if (!cluster.fBounds) {
          Cursor ret;
          return ret;
        }
        auto bounds = cluster.fBounds->transformedBy(mtx).expanded(caretExpand);
        if (std::holds_alternative<CursorLocationLeft>(*location)) {
          Cursor ret;
          ret.fRect = {lineIndex, juce::Rectangle<float>(dx + bounds.getX(), dy + bounds.getY(), 0, bounds.getHeight())};
          return ret;
        } else {
          auto loc = std::get<CursorLocationRight>(*location);
          Cursor ret;
          ret.fRect = {lineIndex, juce::Rectangle<float>(dx + bounds.getRight(), dy + bounds.getY(), 0, bounds.getHeight())};
          if (loc.fBlock) {
            ret.fSelectionRects.push_back({lineIndex, juce::Rectangle<float>(dx + bounds.getX(),
                                                                             dy + bounds.getY(),
                                                                             bounds.getWidth(),
                                                                             bounds.getHeight())});
          }
          return ret;
        }
      }
    } else {
      Cursor ret;
      juce::Range<int> selection(selectionStart, selectionEnd);
      float dx = padding;
      float dy = padding;
      for (int lineIndex = 0; lineIndex < (int)fLines.size(); lineIndex++) {
        auto const &line = fLines[lineIndex];
        std::optional<juce::Rectangle<float>> bb;

        for (auto const &ch : line->fChars) {
          if (!ch.fSign) {
            continue;
          }
          auto intersection = selection.getIntersectionWith({line->fRawOffset + ch.fRawOffset, line->fRawOffset + ch.fRawOffset + (int)ch.fRaw.size()});
          if (intersection.getLength() != (int)ch.fRaw.size()) {
            continue;
          }
          for (auto const &cluster : line->fClusters) {
            if (!cluster.fBounds || cluster.fCluster != ch.fCluster) {
              continue;
            }
            auto bounds = cluster.fBounds->transformedBy(mtx).expanded(setting.fCaretExpand).translated(dx, dy);
            if (bb) {
              bb = bb->getUnion(bounds);
            } else {
              bb = bounds;
            }
          }
        }
        if (bb) {
          ret.fSelectionRects.push_back({lineIndex, *bb});
        }
        dy += setting.lineSpacing() + setting.fFontSize;
      }
      return ret;
    }
  }

  std::string toPDF(PresentationSetting const &setting) const {
    using namespace std;
    float const fontSize = setting.fFontSize;
    float const padding = setting.fPadding;
    auto font = fFont.lock();
    if (!font) {
      return {};
    }
    auto [width, height] = getSize(setting);
    struct Data {
      float fScale;
      float fDx;
      float fDy;
      float fTx;
      float fTy;
      vector<pdf_path_operation> fBuffer;
      float x(float v) const {
        return (v + fTx) * fScale + fDx;
      }
      float y(float v) const {
        return (v + fTy) * fScale + fDy;
      }
      float fCurrentX = 0;
      float fCurrentY = 0;
    };
    HbDrawFuncsUniquePtr funcs(hb_draw_funcs_create());
    hb_draw_funcs_set_move_to_func(
        funcs.get(),
        [](auto *, void *data, auto *, float x, float y, auto *) {
          auto &d = *static_cast<Data *>(data);
          pdf_path_operation op;
          op.op = 'm';
          op.x1 = d.x(x);
          op.y1 = d.y(y);
          d.fBuffer.push_back(op);
          d.fCurrentX = x;
          d.fCurrentY = y;
        },
        nullptr, nullptr);
    hb_draw_funcs_set_line_to_func(
        funcs.get(),
        [](auto *, void *data, auto *, float x, float y, auto *) {
          auto &d = *static_cast<Data *>(data);
          pdf_path_operation op;
          op.op = 'l';
          op.x1 = d.x(x);
          op.y1 = d.y(y);
          d.fBuffer.push_back(op);
          d.fCurrentX = x;
          d.fCurrentY = y;
        },
        nullptr, nullptr);
    hb_draw_funcs_set_quadratic_to_func(
        funcs.get(),
        [](auto *, void *data, auto *, float ctlX, float ctlY, float toX, float toY, auto *) {
          auto &d = *static_cast<Data *>(data);
          float xc1 = d.fCurrentX + (ctlX - d.fCurrentX) * (2.0f / 3.0f);
          float yc1 = d.fCurrentY + (ctlY - d.fCurrentY) * (2.0f / 3.0f);
          float xc2 = toX + (ctlX - toX) * (2.0f / 3.0f);
          float yc2 = toY + (ctlY - toY) * (2.0f / 3.0f);
          pdf_path_operation op;
          op.op = 'c';
          op.x3 = d.x(toX);
          op.y3 = d.y(toY);
          op.x1 = d.x(xc1);
          op.y1 = d.y(yc1);
          op.x2 = d.x(xc2);
          op.y2 = d.y(yc2);
          d.fBuffer.push_back(op);
          d.fCurrentX = toX;
          d.fCurrentY = toY;
        },
        nullptr, nullptr);
    hb_draw_funcs_set_cubic_to_func(
        funcs.get(),
        [](auto *, void *data, auto *, float ctlX1, float ctlY1, float ctlX2, float ctlY2, float toX, float toY, auto *) {
          auto &d = *static_cast<Data *>(data);
          pdf_path_operation op;
          op.op = 'c';
          op.x3 = d.x(toX);
          op.y3 = d.y(toY);
          op.x1 = d.x(ctlX1);
          op.y1 = d.y(ctlY1);
          op.x2 = d.x(ctlX2);
          op.y2 = d.y(ctlY2);
          d.fBuffer.push_back(op);
          d.fCurrentX = toX;
          d.fCurrentY = toY;
        },
        nullptr, nullptr);
    hb_draw_funcs_set_close_path_func(
        funcs.get(),
        [](auto *, void *data, auto *, auto *) {
          auto &d = *static_cast<Data *>(data);
          pdf_path_operation op;
          op.op = 'h';
          d.fBuffer.push_back(op);
        },
        nullptr, nullptr);

    unique_ptr<pdf_doc, juce::FunctionPointerDestructor<pdf_destroy>> doc(pdf_create(width, height, nullptr));
    pdf_object *page = pdf_append_page(doc.get());

    int lineIndex = 0;
    for (auto const &line : fLines) {
      unsigned int numGlyphs = hb_buffer_get_length(line->fBuffer.get());
      hb_glyph_info_t *glyphInfo = hb_buffer_get_glyph_infos(line->fBuffer.get(), nullptr);
      hb_glyph_position_t *glyphPos = hb_buffer_get_glyph_positions(line->fBuffer.get(), nullptr);
      float cursorX = 0;
      float cursorY = 0;
      for (unsigned int i = 0; i < numGlyphs; i++) {
        auto glyphId = glyphInfo[i].codepoint;
        auto xOffset = glyphPos[i].x_offset;
        auto yOffset = glyphPos[i].y_offset;
        auto xAdvance = glyphPos[i].x_advance;
        auto yAdvance = glyphPos[i].y_advance;
        float x = cursorX + xOffset;
        float y = cursorY + yOffset;

        Data data;
        data.fScale = fontSize * font->fScale;
        data.fTx = x;
        // https://gyazo.com/1aae4ad48ead791b3daeaf1e4c7146d1
        data.fTy = y + 1 / font->fScale + font->fY;
        data.fDx = padding;
        data.fDy = height - lineIndex * (fontSize + setting.lineSpacing()) - padding - fontSize;
        data.fBuffer.push_back({.op = 'm', .x1 = 0, .y1 = 0});
        hb_font_draw_glyph(font->fFont.get(), glyphId, funcs.get(), &data);
        if (data.fBuffer.size() > 1) {
          pdf_add_custom_path(doc.get(), page, data.fBuffer.data(), data.fBuffer.size(), 0, 0, PDF_BLACK);
        }
        cursorX += xAdvance;
        cursorY += yAdvance;
      }
      lineIndex++;
    }
    string out;
    pdf_save_stream(doc.get(), out);
    return out;
  }

  std::pair<float, float> getSize(PresentationSetting const &setting) const {
    float const fontSize = setting.fFontSize;
    float const padding = setting.fPadding;
    if (fLines.empty()) {
      return std::make_pair<float>(2 * padding, 2 * padding);
    }
    auto font = fFont.lock();
    if (!font) {
      return std::make_pair<float>(2 * padding, 2 * padding);
    }
    float const height = padding * 2 + fontSize * fLines.size() + setting.lineSpacing() * (fLines.size() - 1);
    float width = padding * 2;
    for (auto const &line : fLines) {
      float const lineWidth = line->fWidth * font->fScale * fontSize;
      width = std::max(width, lineWidth + 2 * padding);
    }
    return std::make_pair(width, height);
  }

  std::pair<int, int> getSizeCeiled(PresentationSetting const &setting, float scale) const {
    auto [width, height] = getSize(setting);
    int w = (int)ceil(width * scale);
    int h = (int)ceil(height * scale);
    return std::make_pair(w, h);
  }

  void draw(juce::Graphics &g, PresentationSetting const &setting) const {
    float const fontSize = setting.fFontSize;
    float const padding = setting.fPadding;
    float const lineSpacing = setting.lineSpacing();
    float dx = padding;
    float dy = padding;
    auto font = fFont.lock();
    if (!font) {
      return;
    }
    g.saveState();
    g.setColour(juce::Colours::black);
    for (auto const &line : fLines) {
      for (auto const &glyph : line->fGlyphs) {
        auto path = Harfbuzz::CreatePath(glyph.fGlyphId, font->fFont.get(), juce::AffineTransform::translation(glyph.fX, glyph.fY - font->fY).scaled(font->fScale * fontSize).translated(dx, dy));
        if (path.getBounds().isEmpty()) {
          continue;
        }
        g.fillPath(path);
      }
      dy += lineSpacing + fontSize;
    }
    g.restoreState();
  }

  void draw(
      juce::Graphics &g,
      int start,
      int end,
      Direction direction,
      PresentationSetting const &setting,
      float caretWidth,
      juce::Colour const &textColor,
      juce::Colour const &highlightTextColor,
      juce::Colour const &caretColor,
      juce::Colour const &highlightColor,
      std::optional<float> maxWidth = std::nullopt) {
    auto cursor = this->cursor(start, end, direction, setting);

    float const fontSize = setting.fFontSize;
    float const padding = setting.fPadding;
    float const lineSpacing = setting.lineSpacing();
    auto font = fFont.lock();
    if (!font) {
      return;
    }
    std::optional<float> availableWidth;
    if (maxWidth) {
      float remaining = *maxWidth - 2 * padding;
      if (remaining <= 0) {
        return;
      }
      availableWidth = remaining;
    }
    g.saveState();
    g.setColour(highlightColor);
    for (auto const &rect : cursor.fSelectionRects) {
      auto line = fLines[rect.fLineIndex];
      float width = line->fWidth * font->fScale * fontSize;
      bool shrink = false;
      if (availableWidth && width > *availableWidth) {
        shrink = true;
        g.saveState();
        g.addTransform(juce::AffineTransform::scale(*availableWidth / width, 1));
      }
      g.fillRect(rect.fRect);
      if (shrink) {
        g.restoreState();
      }
    }
    g.restoreState();

    float const dx = padding;
    float dy = padding;
    g.saveState();
    juce::Range<int> selection(start, end);
    for (auto const &line : fLines) {
      if (start == end) {
        g.setColour(textColor);
      }

      float width = line->fWidth * font->fScale * fontSize;
      bool shrink = false;
      if (availableWidth && width > *availableWidth) {
        shrink = true;
        g.saveState();
        g.addTransform(juce::AffineTransform::scale(*availableWidth / width, 1));
      }
      for (auto const &glyph : line->fGlyphs) {
        auto path = Harfbuzz::CreatePath(glyph.fGlyphId,
                                         font->fFont.get(),
                                         juce::AffineTransform::translation(glyph.fX, glyph.fY - font->fY)
                                             .scaled(font->fScale * fontSize)
                                             .translated(dx, dy));
        if (path.getBounds().isEmpty()) {
          continue;
        }
        if (start != end) {
          bool selected = false;
          for (auto const &ch : line->fChars) {
            if (!ch.fSign || ch.fCluster != (int)glyph.fCluster) {
              continue;
            }
            if (selection.getIntersectionWith({line->fRawOffset + ch.fRawOffset, line->fRawOffset + ch.fRawOffset + (int)ch.fRaw.size()}).getLength() == (int)ch.fRaw.size()) {
              selected = true;
              break;
            }
          }
          g.setColour(selected ? highlightTextColor : textColor);
        }
        g.fillPath(path);
      }
      if (shrink) {
        g.restoreState();
      }
      dy += lineSpacing + fontSize;
    }
    g.restoreState();

    if (cursor.fRect) {
      g.setColour(caretColor);
      auto line = fLines[cursor.fRect->fLineIndex];
      float width = line->fWidth * font->fScale * fontSize;
      bool shrink = false;
      float scale = 1;
      if (availableWidth && width > *availableWidth) {
        shrink = true;
        g.saveState();
        scale = *availableWidth / width;
        g.addTransform(juce::AffineTransform::scale(scale, 1));
      }
      g.fillRect(cursor.fRect->fRect.expanded(caretWidth * 0.5f / scale, 0));
      if (shrink) {
        g.restoreState();
      }
    }
  }

#if defined(JUCE_WINDOWS)
  std::string toEMF(PresentationSetting const &setting) const {
    using namespace std;
    string out;
    auto font = fFont.lock();
    if (!font) {
      return out;
    }
    auto [widthf, heightf] = getSize(setting);
    LONG width = (LONG)ceil(widthf);
    LONG height = (LONG)ceil(heightf);
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = width;
    rc.bottom = height;
    HDC hdc = ::CreateEnhMetaFile(nullptr, nullptr, nullptr, nullptr);
    if (INVALID_HANDLE_VALUE == hdc) {
      return out;
    }
    ::SetGraphicsMode(hdc, GM_ADVANCED);
    ::SetWindowOrgEx(hdc, 0, 0, nullptr);
    ::SetWindowExtEx(hdc, width, height, nullptr);

    struct Data {
      HDC fHdc;
      int fDx = 0;
      int fDy = 0;
      float fTy = 0;
      float fTx = 0;
      float fCurrentX = 0;
      float fCurrentY = 0;
      int x(float v) const {
        return (int)round(v + fTx) + fDx;
      }
      int y(float v) const {
        return (int)round(-v + fTy) + fDy;
      }
      void current(float x, float y) {
        fCurrentX = x;
        fCurrentY = y;
      }
      void begin() {
        if (fBegan) {
          return;
        }
        ::BeginPath(fHdc);
        fBegan = true;
      }
      bool end() {
        if (!fBegan) {
          return false;
        }
        ::EndPath(fHdc);
        return true;
      }
      bool fBegan = false;
    };
    HbDrawFuncsUniquePtr funcs(hb_draw_funcs_create());
    hb_draw_funcs_set_move_to_func(
        funcs.get(),
        [](auto *, void *data, auto *, float x, float y, auto *) {
          auto &d = *static_cast<Data *>(data);
          d.begin();
          ::MoveToEx(d.fHdc, d.x(x), d.y(y), nullptr);
          d.current(x, y);
        },
        nullptr, nullptr);
    hb_draw_funcs_set_line_to_func(
        funcs.get(),
        [](auto *, void *data, auto *, float x, float y, auto *) {
          auto &d = *static_cast<Data *>(data);
          d.begin();
          ::LineTo(d.fHdc, d.x(x), d.y(y));
          d.current(x, y);
        },
        nullptr, nullptr);
    hb_draw_funcs_set_quadratic_to_func(
        funcs.get(),
        [](auto *, void *data, auto *, float ctlX, float ctlY, float toX, float toY, auto *) {
          auto &d = *static_cast<Data *>(data);
          float xc1 = d.fCurrentX + (ctlX - d.fCurrentX) * (2.0f / 3.0f);
          float yc1 = d.fCurrentY + (ctlY - d.fCurrentY) * (2.0f / 3.0f);
          float xc2 = toX + (ctlX - toX) * (2.0f / 3.0f);
          float yc2 = toY + (ctlY - toY) * (2.0f / 3.0f);
          POINT pt[3] = {
              {d.x(xc1), d.y(yc1)},
              {d.x(xc2), d.y(yc2)},
              {d.x(toX), d.y(toY)},
          };
          d.begin();
          ::PolyBezierTo(d.fHdc, pt, 3);
          d.current(toX, toY);
        },
        nullptr, nullptr);
    hb_draw_funcs_set_cubic_to_func(
        funcs.get(),
        [](auto *, void *data, auto *, float ctlX1, float ctlY1, float ctlX2, float ctlY2, float toX, float toY, auto *) {
          auto &d = *static_cast<Data *>(data);
          POINT pt[3] = {
              {d.x(ctlX1), d.y(ctlY1)},
              {d.x(ctlX2), d.y(ctlY2)},
              {d.x(toX), d.y(toY)},
          };
          d.begin();
          ::PolyBezierTo(d.fHdc, pt, 3);
          d.current(toX, toY);
        },
        nullptr, nullptr);
    hb_draw_funcs_set_close_path_func(
        funcs.get(),
        [](auto *, void *data, auto *, auto *) {
          auto &d = *static_cast<Data *>(data);
          d.begin();
          ::CloseFigure(d.fHdc);
        },
        nullptr, nullptr);
    {
      ScopedHandle<HBRUSH, ::DeleteObject> brush(::CreateSolidBrush(RGB(0, 0, 0)));
      ::SelectObject(hdc, brush);

      float const fontSize = setting.fFontSize;
      float const padding = setting.fPadding;
      float dy = 0;

      ::ModifyWorldTransform(hdc, nullptr, MWT_IDENTITY);
      XFORM mtx;
      mtx.eM11 = font->fScale * fontSize;
      mtx.eM12 = 0;
      mtx.eM21 = 0;
      mtx.eM22 = font->fScale * fontSize;
      mtx.eDx = padding;
      mtx.eDy = padding;
      ::SetWorldTransform(hdc, &mtx);

      for (int lineIndex = 0; lineIndex < (int)fLines.size(); lineIndex++) {
        auto const &line = fLines[lineIndex];
        unsigned int numGlyphs = hb_buffer_get_length(line->fBuffer.get());
        hb_glyph_info_t *glyphInfo = hb_buffer_get_glyph_infos(line->fBuffer.get(), nullptr);
        hb_glyph_position_t *glyphPos = hb_buffer_get_glyph_positions(line->fBuffer.get(), nullptr);
        float cursorX = 0;
        float cursorY = 0;
        for (unsigned int i = 0; i < numGlyphs; i++) {
          auto glyphId = glyphInfo[i].codepoint;
          auto xOffset = glyphPos[i].x_offset;
          auto yOffset = glyphPos[i].y_offset;
          auto xAdvance = glyphPos[i].x_advance;
          auto yAdvance = glyphPos[i].y_advance;
          float x = cursorX + xOffset;
          float y = -(cursorY + yOffset - font->fY);

          Data data;
          data.fHdc = hdc;
          data.fTx = x;
          data.fTy = y;
          data.fDx = 0;
          data.fDy = dy;
          hb_font_draw_glyph(font->fFont.get(), glyphId, funcs.get(), &data);
          if (data.end()) {
            ::FillPath(hdc);
          }
          cursorX += xAdvance;
          cursorY += yAdvance;
        }
        dy += (setting.lineSpacing() + fontSize) / (font->fScale * fontSize);
      }
    }

    ScopedHandle<HENHMETAFILE, ::DeleteEnhMetaFile> file(::CloseEnhMetaFile(hdc));
    if (!file) {
      return out;
    }
    auto size = ::GetEnhMetaFileBits(file, 0, nullptr);
    if (size == 0) {
      return out;
    }
    string s;
    s.resize(size);
    if (::GetEnhMetaFileBits(file, s.size(), (LPBYTE)s.data()) != size) {
      return out;
    }
    s.swap(out);
    return out;
  }
#endif

  std::vector<std::shared_ptr<Line>> fLines;
  std::weak_ptr<FontAdapter> fFont;
  std::u32string const fRaw;
};

} // namespace ksesh
