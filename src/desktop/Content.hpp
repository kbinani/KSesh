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
  int cluster;
  Char(CharBase const &base, int cluster) : CharBase(base.ch, base.raw, base.rawOffset, base.resultOffset, base.ctrl, base.sign), cluster(cluster) {}
};

struct Cluster {
  int resultOffset;
  std::optional<juce::Rectangle<float>> bounds;
  Cluster(int resultOffset, std::optional<juce::Rectangle<float>> bounds) : resultOffset(resultOffset), bounds(bounds) {}
};

struct GlyphInformation {
  hb_codepoint_t glyphId;
  hb_position_t x;
  hb_position_t y;
  uint32_t cluster;
};

class Line {
public:
  Line(int rawOffset, std::u32string const &raw, HbFontUniquePtr const &font) {
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

    HbBufferUniquePtr buffer(hb_buffer_create());
    hb_buffer_add_utf8(buffer.get(), (char const *)utf8.c_str(), -1, 0, -1);
    hb_buffer_set_direction(buffer.get(), HB_DIRECTION_LTR);
    hb_buffer_set_script(buffer.get(), HB_SCRIPT_EGYPTIAN_HIEROGLYPHS);
    hb_buffer_set_cluster_level(buffer.get(), HB_BUFFER_CLUSTER_LEVEL_CHARACTERS);
    hb_shape(font.get(), buffer.get(), nullptr, 0);

    hb_font_extents_t extents{};
    hb_font_get_h_extents(font.get(), &extents);
    auto ascender = extents.ascender;
    auto descender = extents.descender;
    unitsPerEm = hb_face_get_upem(hb_font_get_face(font.get()));

    unsigned int numGlyphs = hb_buffer_get_length(buffer.get());
    hb_glyph_info_t *glyphInfo = hb_buffer_get_glyph_infos(buffer.get(), nullptr);
    hb_glyph_position_t *glyphPos = hb_buffer_get_glyph_positions(buffer.get(), nullptr);
    hb_position_t cursorX = 0;
    hb_position_t cursorY = -(unitsPerEm + descender);
    for (unsigned int i = 0; i < numGlyphs; i++) {
      GlyphInformation info;
      info.glyphId = glyphInfo[i].codepoint;
      info.cluster = glyphInfo[i].cluster;
      auto xOffset = glyphPos[i].x_offset;
      auto yOffset = glyphPos[i].y_offset;
      auto xAdvance = glyphPos[i].x_advance;
      auto yAdvance = glyphPos[i].y_advance;
      info.x = cursorX + xOffset;
      info.y = -(cursorY + yOffset);
      glyphs.push_back(info);
      cursorX += xAdvance;
      cursorY += yAdvance;
    }

    uint32_t lastCluster = 0;
    int index = 0;
    optional<juce::Rectangle<float>> bb;
    float maxX = 0;
    for (auto const &info : glyphs) {
      juce::Path path = Harfbuzz::CreatePath(info.glyphId, font);
      juce::Rectangle<float> bounds = path.getBounds();
      if (info.cluster != lastCluster) {
        auto u32 = U32StringFromU8string(utf8.substr(lastCluster, info.cluster - lastCluster));
        clusters.push_back(Cluster(index, bb));
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
      clusters.push_back(Cluster(index, bb));
    }
    for (size_t i = 0; i < clusters.size(); i++) {
      int from = clusters[i].resultOffset;
      int to =
          i + 1 < clusters.size()
              ? clusters[i + 1].resultOffset
              : this->result.size();
      for (CharBase const &ch : chars) {
        if (from <= ch.resultOffset && ch.resultOffset < to) {
          this->chars.push_back(Char(ch, i));
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

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Line)
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
      offset += found + 1;
    }
  }

  std::vector<std::shared_ptr<Line>> lines;
};

} // namespace ksesh
