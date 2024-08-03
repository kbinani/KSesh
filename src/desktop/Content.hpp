#pragma once

namespace ksesh {

struct CharBase {
  std::u32string ch;
  std::optional<std::u32string> charPlain;
  std::u32string raw;
  int rawOffset;
  int resultOffset;
  bool ctrl;
  bool sign;

  CharBase(std::u32string ch,
           std::u32string raw,
           int rawOffset,
           int resultOffset,
           bool ctrl,
           bool sign,
           std::optional<std::u32string> charPlain = std::nullopt) : ch(ch), charPlain(charPlain), raw(raw), rawOffset(rawOffset), resultOffset(resultOffset), ctrl(ctrl), sign(sign) {
  }
};

struct Char : public CharBase {
  int clusterIndex;
  int cluster;
  Char(CharBase const &base, int clusterIndex, int cluster) : CharBase(base.ch, base.raw, base.rawOffset, base.resultOffset, base.ctrl, base.sign), clusterIndex(clusterIndex), cluster(cluster) {}
};

struct Cluster {
  int resultOffset;
  int cluster;
  std::optional<juce::Rectangle<float>> bounds;
  Cluster(int resultOffset, std::optional<juce::Rectangle<float>> bounds, int cluster) : resultOffset(resultOffset), cluster(cluster), bounds(bounds) {}
};

class Line {
public:
  Line(int rawOffset, std::u32string const &raw, HbFontUniquePtr const &font) : rawOffset(rawOffset), raw(raw) {
    using namespace std;
    using namespace std::literals::string_literals;
    vector<CharBase> chars;
    int offset = 0;
    for (size_t index = 0; index < raw.size();) {
      auto map = SignList::Map(raw, index);
      if (map) {
        bool ctrl = map->second.size() == 0 || SignList::IsFormatControl(map->second);
        bool sign = SignList::IsSign(map->second);
        chars.push_back(CharBase(map->second,
                                 map->first,
                                 index,
                                 offset,
                                 ctrl,
                                 sign));
        index = index + map->first.size();
        offset += map->second.size();
      } else {
        chars.push_back(CharBase(raw.substr(index, 1),
                                 raw.substr(index, 1),
                                 index,
                                 offset,
                                 false,
                                 false));
        index += 1;
      }
    }
    for (size_t i = 0; i < chars.size(); i++) {
      if (chars[i].ch.starts_with(U"𓊈"s)) {
        for (size_t j = i + 1; j < chars.size(); j++) {
          auto &ch = chars[j];
          auto found = find_if(SignList::enclosureBeginning.begin(), SignList::enclosureBeginning.end(), [&](u32string const &c) {
            return ch.ch.starts_with(c);
          });
          if (found != SignList::enclosureBeginning.end()) {
            break;
          }
          found = find_if(SignList::enclosureTerminal.begin(), SignList::enclosureTerminal.end(), [&](u32string const &c) {
            return ch.ch.ends_with(c);
          });
          if (found != SignList::enclosureTerminal.end()) {
            ch.ch = U"\U0001343d"s;
            break;
          }
        }
      }
    }
    for (size_t i = 1; i + 1 < chars.size(); i++) {
      if (!chars[i - 1].sign || chars[i].ch != U"&" || !chars[i + 1].sign) {
        continue;
      }
      auto left = chars[i - 1];
      auto center = chars[i];
      auto right = chars[i + 1];
      auto type = SignList::InsertionType(left.ch, right.ch);
      if (type == Insertions::Type::TopStart) {
        chars[i - 1] = CharBase(right.ch, left.raw, left.rawOffset, left.resultOffset, left.ctrl, left.sign, left.ch);
        chars[i] = CharBase(SignList::topStartInsertion, center.raw, center.rawOffset, center.resultOffset, center.ctrl, center.sign, U"");
        chars[i + 1] = CharBase(left.ch, right.raw, right.rawOffset, right.resultOffset, right.ctrl, right.sign, right.ch);
      } else if (type == Insertions::Type::BottomStart) {
        chars[i - 1] = CharBase(right.ch, left.raw, left.rawOffset, left.resultOffset, left.ctrl, left.sign, left.ch);
        chars[i] = CharBase(SignList::bottomStartInsertion, center.raw, center.rawOffset, center.resultOffset, center.ctrl, center.sign, U"");
        chars[i + 1] = CharBase(left.ch, right.raw, right.rawOffset, right.resultOffset, right.ctrl, right.sign, right.ch);
      } else if (type == Insertions::Type::TopEnd) {
        chars[i] = CharBase(SignList::topEndInsertion, center.raw, center.rawOffset, center.resultOffset, center.ctrl, center.sign, U"");
      } else if (type == Insertions::Type::BottomEnd) {
        chars[i] = CharBase(SignList::bottomEndInsertion, center.raw, center.rawOffset, center.resultOffset, center.ctrl, center.sign, U"");
      }
      if (type == Insertions::Type::TopStart || type == Insertions::Type::BottomStart) {
        if (i + 3 < chars.size() && chars[i + 2].ch == U"&" && chars[i + 3].sign) {
          auto o = chars[i + 2];
          auto t = SignList::InsertionType(right.ch, chars[i + 3].ch);
          if (t == Insertions::Type::TopEnd) {
            chars[i + 2] = CharBase(SignList::topEndInsertion, o.raw, o.rawOffset, o.resultOffset, o.ctrl, o.sign, U"");
          } else if (t == Insertions::Type::BottomEnd) {
            chars[i + 2] = CharBase(SignList::bottomEndInsertion, o.raw, o.rawOffset, o.resultOffset, o.ctrl, o.sign, U"");
          }
          i += 3;
        }
      }
    }
    for (auto const &it : chars) {
      result += it.ch;
    }
    u8string utf8 = U8StringFromU32String(result);

    HbBufferUniquePtr buffer(Harfbuzz::CreateBuffer(result, font));

    hb_font_extents_t extents{};
    hb_font_get_h_extents(font.get(), &extents);
    auto ascender = extents.ascender;
    auto descender = extents.descender;
    unitsPerEm = hb_face_get_upem(hb_font_get_face(font.get()));

    Harfbuzz::CreateGlyphInformations(buffer, font, glyphs);

    uint32_t lastCluster = 0;
    int index = 0;
    optional<juce::Rectangle<float>> bb;
    float maxX = 0;
    for (auto const &info : glyphs) {
      juce::Path path = Harfbuzz::CreatePath(info.glyphId, font, info.x, info.y);
      juce::Rectangle<float> bounds = path.getBounds();
      if (info.cluster != lastCluster) {
        auto u32 = U32StringFromU8string(utf8.substr(lastCluster, info.cluster - lastCluster));
        clusters.push_back(Cluster(index, bb, lastCluster));
        index += u32.size();
        lastCluster = info.cluster;
        bb = nullopt;
      }
      if (bounds.getWidth() > 0 && bounds.getHeight() > 0) {
        maxX = std::max(maxX, bounds.getRight());
        if (bb) {
          bb = bb->getUnion(bounds);
        } else {
          bb = bounds;
        }
      }
    }
    this->buffer.swap(buffer);
    if (lastCluster < utf8.size()) {
      clusters.push_back(Cluster(index, bb, lastCluster));
    }
    for (size_t i = 0; i < clusters.size(); i++) {
      int from = clusters[i].resultOffset;
      int to =
          i + 1 < clusters.size()
              ? clusters[i + 1].resultOffset
              : this->result.size();
      int cluster = clusters[i].cluster;
      for (CharBase const &ch : chars) {
        if (from <= ch.resultOffset && ch.resultOffset < to) {
          this->chars.push_back(Char(ch, i, cluster));
        }
      }
    }

    boundingBox = juce::Rectangle<float>(0, 0, maxX, ascender - descender);
  }

public:
  std::u32string result;
  std::vector<Cluster> clusters;
  juce::Rectangle<float> boundingBox;
  std::vector<Char> chars;
  HbBufferUniquePtr buffer;
  int unitsPerEm;
  std::vector<GlyphInformation> glyphs;
  int rawOffset;
  std::u32string const raw;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Line)
};

enum class Direction {
  Forward,
  Backward,
};

struct LineAndCluster {
  int lineIndex;
  int clusterIndex;
  LineAndCluster() : lineIndex(0), clusterIndex(0) {}
  LineAndCluster(int lineIndex, int clusterIndex) : lineIndex(lineIndex), clusterIndex(clusterIndex) {}
  LineAndCluster(LineAndCluster const &) = default;
  LineAndCluster &operator=(LineAndCluster const &) = default;
};
inline int compareLineAndCluster(LineAndCluster left, LineAndCluster right) {
  if (left.lineIndex == right.lineIndex) {
    return left.clusterIndex - right.clusterIndex;
  } else {
    return left.lineIndex - right.lineIndex;
  }
}

struct CursorLocationLeft : public LineAndCluster {
  CursorLocationLeft(int lineIndex, int clusterIndex) : LineAndCluster(lineIndex, clusterIndex) {}
};
struct CursorLocationRight : public LineAndCluster {
  bool block;
  CursorLocationRight(int lineIndex, int clusterIndex, bool block) : LineAndCluster(lineIndex, clusterIndex), block(block) {}
};
struct CursorLocationEnd {
  int lineIndex;
  explicit CursorLocationEnd(int lineIndex) : lineIndex(lineIndex) {}
};
using CursorLocation = std::variant<CursorLocationLeft, CursorLocationRight, CursorLocationEnd>;

struct Cursor {
  std::optional<juce::Rectangle<float>> rect;
  std::vector<juce::Rectangle<float>> selectionRects;
};

struct CaretLocation {
  int location;
  Direction direction;
  CaretLocation(int location, Direction direction) : location(location), direction(direction) {}
};

class Content {
public:
  Content(std::u32string const &raw, HbFontUniquePtr const &font) {
    using namespace std;
    u32string::size_type offset = 0;
    while (offset < raw.size()) {
      auto found = raw.find(U'\n', offset);
      if (found == u32string::npos) {
        auto l = make_shared<Line>(offset, raw.substr(offset), font);
        lines.push_back(l);
        break;
      }
      auto l = make_shared<Line>(offset, raw.substr(offset, found - offset), font);
      lines.push_back(l);
      offset = found + 1;
    }
  }

  CaretLocation closestPosition(
      std::optional<int> current,
      juce::Point<float> point,
      HbFontUniquePtr const &font,
      PresentationSetting const &setting) {
    float const padding = setting.padding;
    float const fontSize = setting.fontSize;
    float const lineSpacing = setting.lineSpacing;

    if (lines.empty()) {
      return CaretLocation(0, Direction::Forward);
    }
    int lineIndex;
    if (point.y <= padding + fontSize + lineSpacing / 2) {
      lineIndex = 0;
    } else {
      auto dy = point.y - (padding + fontSize + lineSpacing / 2);
      lineIndex = std::min<int>(
          1 + (int)floor(dy / (fontSize + lineSpacing)),
          (int)lines.size() - 1);
    }
    auto line = lines[lineIndex];
    float minDistance = std::numeric_limits<float>::max();
    CaretLocation nearest(line->rawOffset, Direction::Forward);
    std::vector<CaretLocation> test;
    for (int i = 0; i < (int)line->chars.size(); i++) {
      auto ch = line->chars[i];
      if (!ch.sign) {
        continue;
      }
      auto start = line->rawOffset + ch.rawOffset;
      auto end = line->rawOffset + ch.rawOffset + (int)ch.raw.size();
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
            it.location,
            it.location,
            it.direction,
            font,
            setting);
        if (!cursor.rect) {
          continue;
        }
        auto center = cursor.rect->getCentre();
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
    auto lineItr = find_if(lines.begin(), lines.end(), [location](shared_ptr<Line> const &line) {
      return line->rawOffset <= location && location <= line->rawOffset + (int)line->raw.size();
    });
    if (lineItr == lines.end()) {
      return nullopt;
    }
    int lineIndex = distance(lines.begin(), lineItr);
    shared_ptr<Line> line = *lineItr;
    if (location == line->rawOffset) {
      return CursorLocationLeft(lineIndex, 0);
    }
    int charIndex = -1;
    for (int i = 0; i < (int)line->chars.size(); i++) {
      auto char_ = line->chars[i];
      auto from = line->rawOffset + char_.rawOffset;
      int to;
      if (i + 1 < (int)line->chars.size()) {
        to = line->chars[i + 1].rawOffset + (int)line->chars[i + 1].raw.size();
      } else {
        to = (int)line->raw.size();
      }
      to += line->rawOffset;
      if (from <= location && location < to) {
        charIndex = i;
        break;
      }
    }
    if (charIndex < 0 || (int)line->chars.size() <= charIndex) {
      return CursorLocationEnd(lineIndex);
    }
    auto char_ = line->chars[charIndex];
    int firstCharIndex = charIndex;
    for (int i = charIndex; i >= 0; i--) {
      auto ch = line->chars[i];
      if (ch.clusterIndex == char_.clusterIndex) {
        firstCharIndex = i;
      } else if (ch.clusterIndex < char_.clusterIndex) {
        break;
      }
    }
    int lastCharIndex = charIndex;
    for (int i = charIndex; i < (int)line->chars.size(); i++) {
      auto ch = line->chars[i];
      if (ch.clusterIndex == char_.clusterIndex) {
        lastCharIndex = i;
      } else if (ch.clusterIndex > char_.clusterIndex) {
        break;
      }
    }
    int startCharIndex = -1;
    for (int i = firstCharIndex; i <= lastCharIndex; i++) {
      auto ch = line->chars[i];
      if (ch.sign) {
        startCharIndex = i;
        break;
      }
    }
    if (startCharIndex < 0) {
      return nullopt;
    }
    if (char_.clusterIndex > 0) {
      for (int i = firstCharIndex - 1; i >= 0; i--) {
        auto ch = line->chars[i];
        if (ch.sign && !ch.ctrl) {
          if (location == line->rawOffset + ch.rawOffset + (int)ch.raw.size()) {
            return CursorLocationRight(lineIndex, ch.clusterIndex, true);
          }
        }
      }
    }
    auto start = line->chars[startCharIndex];
    if (
        line->rawOffset + start.rawOffset < location &&
        location <= line->rawOffset + start.rawOffset + (int)start.raw.size()) {
      return CursorLocationRight(lineIndex, start.clusterIndex, true);
    }
    if (line->rawOffset + start.rawOffset + (int)start.raw.size() < location) {
      if (direction == Direction::Backward) {
        return CursorLocationRight(lineIndex, char_.clusterIndex, false);
      } else if ((int)line->clusters.size() - 1 == char_.clusterIndex) {
        return CursorLocationEnd(lineIndex);
      } else {
        for (int i = lastCharIndex + 1; i < (int)line->chars.size(); i++) {
          auto ch = line->chars[i];
          if (ch.sign) {
            return CursorLocationLeft(lineIndex, ch.clusterIndex);
          }
        }
      }
    }
    if (charIndex <= startCharIndex) {
      if (direction == Direction::Forward || char_.clusterIndex == 0) {
        return CursorLocationLeft(lineIndex, char_.clusterIndex);
      }
    }
    if (direction == Direction::Forward) {
      for (int i = charIndex + 1; i < (int)line->chars.size(); i++) {
        auto ch = line->chars[i];
        if (ch.sign) {
          return CursorLocationLeft(lineIndex, ch.clusterIndex);
        }
      }
    } else {
      for (int i = charIndex - 1; i >= 0; i--) {
        auto ch = line->chars[i];
        if (ch.sign) {
          return CursorLocationRight(lineIndex, ch.clusterIndex, false);
        }
      }
    }
    return nullopt;
  }

  Cursor cursor(
      int selectionStart,
      int selectionEnd,
      Direction direction,
      HbFontUniquePtr const &font,
      PresentationSetting const &setting) {
    float fontSize = setting.fontSize;
    float lineSpacing = setting.lineSpacing;
    float padding = setting.padding;

    float upem = Harfbuzz::UnitsPerEm(font);
    auto scale = fontSize / upem;
    if (selectionStart == selectionEnd) {
      auto location = cursorLocation(selectionStart, direction);
      if (!location) {
        Cursor ret;
        return ret;
      }
      if (std::holds_alternative<CursorLocationEnd>(*location)) {
        auto loc = std::get<CursorLocationEnd>(*location);
        auto lineIndex = loc.lineIndex;
        float dx = padding;
        float dy = padding + (fontSize + lineSpacing) * lineIndex;
        auto line = this->lines[lineIndex];
        auto bounds = (line->boundingBox * scale).expanded(setting.caretExpand);
        Cursor ret;
        ret.rect = juce::Rectangle<float>(
            dx + bounds.getX() + bounds.getWidth(),
            dy + bounds.getY(),
            0,
            bounds.getHeight());
        return ret;
      } else {
        int lineIndex;
        int clusterIndex;
        if (std::holds_alternative<CursorLocationLeft>(*location)) {
          auto loc = std::get<CursorLocationLeft>(*location);
          lineIndex = loc.lineIndex;
          clusterIndex = loc.clusterIndex;
        } else if (std::holds_alternative<CursorLocationRight>(*location)) {
          auto loc = std::get<CursorLocationRight>(*location);
          lineIndex = loc.lineIndex;
          clusterIndex = loc.clusterIndex;
        } else {
          Cursor ret;
          return ret;
        }
        float dx = padding;
        float dy = padding + (fontSize + lineSpacing) * lineIndex;
        auto line = this->lines[lineIndex];
        if (clusterIndex < 0 || (int)line->clusters.size() <= clusterIndex) {
          Cursor ret;
          ret.rect = juce::Rectangle<float>(dx, dy, 0, fontSize);
          return ret;
        }
        auto cluster = line->clusters[clusterIndex];
        if (!cluster.bounds) {
          Cursor ret;
          return ret;
        }
        auto bounds = ((*cluster.bounds) * scale).expanded(setting.caretExpand);
        if (std::holds_alternative<CursorLocationLeft>(*location)) {
          Cursor ret;
          ret.rect = juce::Rectangle<float>(dx + bounds.getX(), dy + bounds.getY(), 0, bounds.getHeight());
          return ret;
        } else {
          auto loc = std::get<CursorLocationRight>(*location);
          Cursor ret;
          ret.rect = juce::Rectangle<float>(dx + bounds.getRight(), dy + bounds.getY(), 0, bounds.getHeight());
          if (loc.block) {
            ret.selectionRects.push_back(juce::Rectangle<float>(dx + bounds.getX(),
                                                                dy + bounds.getY(),
                                                                bounds.getWidth(),
                                                                bounds.getHeight()));
          }
          return ret;
        }
      }
    } else {
      Cursor ret;
      juce::Range<int> selection(selectionStart, selectionEnd);
      float dx = padding;
      float dy = padding;
      for (auto const &line : lines) {
        std::optional<juce::Rectangle<float>> bb;

        for (auto const &ch : line->chars) {
          if (!ch.sign) {
            continue;
          }
          auto intersection = selection.getIntersectionWith({line->rawOffset + ch.rawOffset, line->rawOffset + ch.rawOffset + (int)ch.raw.size()});
          if (intersection.getLength() != (int)ch.raw.size()) {
            continue;
          }
          for (auto const &cluster : line->clusters) {
            if (!cluster.bounds || cluster.cluster != ch.cluster) {
              continue;
            }
            auto bounds = ((*cluster.bounds) * scale).expanded(setting.caretExpand).translated(dx, dy);
            if (bb) {
              bb = bb->getUnion(bounds);
            } else {
              bb = bounds;
            }
          }
        }
        if (bb) {
          ret.selectionRects.push_back(*bb);
        }
        dy += setting.lineSpacing + setting.fontSize;
      }
      return ret;
    }
  }

  std::string toPDF(HbFontUniquePtr const &font, PresentationSetting const &setting) const {
    using namespace std;
    float upem = Harfbuzz::UnitsPerEm(font);
    float scale = setting.fontSize / upem;
    optional<juce::Rectangle<float>> bb;
    float dx = setting.padding;
    float dy = setting.padding;
    for (auto const &line : lines) {
      auto bounds = (line->boundingBox * scale).translated(dx, dy);
      if (bb) {
        bb = bb->getUnion(bounds);
      } else {
        bb = bounds;
      }
      dy += setting.lineSpacing + setting.fontSize;
    }
    if (!bb) {
      return {};
    }
    int width = (int)ceil(bb->getRight()) + setting.padding;
    int height = (int)ceil(bb->getBottom()) + setting.padding;
    struct Data {
      float scale;
      float dx;
      float dy;
      float tx;
      float ty;
      vector<pdf_path_operation> buffer;
      float x(float v) const {
        return (v + tx) * scale + dx;
      }
      float y(float v) const {
        return (v + ty) * scale + dy;
      }
      float currentX = 0;
      float currentY = 0;
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
          d.buffer.push_back(op);
          d.currentX = x;
          d.currentY = y;
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
          d.buffer.push_back(op);
          d.currentX = x;
          d.currentY = y;
        },
        nullptr, nullptr);
    hb_draw_funcs_set_quadratic_to_func(
        funcs.get(),
        [](auto *, void *data, auto *, float ctlX, float ctlY, float toX, float toY, auto *) {
          auto &d = *static_cast<Data *>(data);
          float xc1 = d.currentX + (ctlX - d.currentX) * (2.0f / 3.0f);
          float yc1 = d.currentY + (ctlY - d.currentY) * (2.0f / 3.0f);
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
          d.buffer.push_back(op);
          d.currentX = toX;
          d.currentY = toY;
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
          d.buffer.push_back(op);
          d.currentX = toX;
          d.currentY = toY;
        },
        nullptr, nullptr);
    hb_draw_funcs_set_close_path_func(
        funcs.get(),
        [](auto *, void *data, auto *, auto *) {
          auto &d = *static_cast<Data *>(data);
          pdf_path_operation op;
          op.op = 'h';
          d.buffer.push_back(op);
        },
        nullptr, nullptr);

    hb_font_extents_t extents{};
    hb_font_get_h_extents(font.get(), &extents);
    auto descender = extents.descender;
    auto ascender = extents.ascender;

    unique_ptr<pdf_doc, juce::FunctionPointerDestructor<pdf_destroy>> doc(pdf_create(width, height, nullptr));
    pdf_append_page(doc.get());

    int lineIndex = 0;
    for (auto const &line : lines) {
      unsigned int numGlyphs = hb_buffer_get_length(line->buffer.get());
      hb_glyph_info_t *glyphInfo = hb_buffer_get_glyph_infos(line->buffer.get(), nullptr);
      hb_glyph_position_t *glyphPos = hb_buffer_get_glyph_positions(line->buffer.get(), nullptr);
      hb_position_t cursorX = 0;
      hb_position_t cursorY = 0;
      for (unsigned int i = 0; i < numGlyphs; i++) {
        auto glyphId = glyphInfo[i].codepoint;
        auto xOffset = glyphPos[i].x_offset;
        auto yOffset = glyphPos[i].y_offset;
        auto xAdvance = glyphPos[i].x_advance;
        auto yAdvance = glyphPos[i].y_advance;
        float x = cursorX + xOffset;
        float y = (cursorY + yOffset);

        Data data;
        data.scale = scale;
        data.tx = x;
        data.ty = y - descender;
        data.dx = setting.padding;
        data.dy = height - lineIndex * (setting.fontSize + setting.lineSpacing) - setting.padding - setting.fontSize;
        data.buffer.push_back({.op = 'm', .x1 = 0, .y1 = 0});
        hb_font_draw_glyph(font.get(), glyphId, funcs.get(), &data);
        if (data.buffer.size() > 1) {
          pdf_add_custom_path(doc.get(), nullptr, data.buffer.data(), data.buffer.size(), 0, 0, PDF_BLACK);
        }
        cursorX += xAdvance;
        cursorY += yAdvance;
      }
      lineIndex++;
    }
    pdf_save(doc.get(), "result.pdf");
    return "";
  }

  std::vector<std::shared_ptr<Line>> lines;
};

} // namespace ksesh
