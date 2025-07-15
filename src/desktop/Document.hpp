#pragma once

namespace ksesh {

class Document {
public:
  Document(std::u32string const &raw, std::shared_ptr<FontAdapter> const &font) : fFont(font), fRaw(raw) {
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

  std::vector<std::shared_ptr<Line>> fLines;
  std::weak_ptr<FontAdapter> fFont;
  std::u32string const fRaw;
};

} // namespace ksesh
