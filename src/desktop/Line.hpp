#pragma once

namespace ksesh {

class Line {
public:
  Line(int rawOffset, std::u32string const &raw, std::shared_ptr<FontAdapter> const &font) : rawOffset(rawOffset), raw(raw) {
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
      auto center = chars[i];
      if (center.ch != U"&") {
        continue;
      }
      auto left = chars[i - 1];
      auto right = chars[i + 1];
      CharBase leftCh = left;
      optional<u32string> glue;
      if (left.raw == U")") {
        int count = 1;
        for (int j = (int)i - 2; j >= 2; j--) {
          auto raw = chars[j].raw;
          if (raw == U")") {
            count++;
          } else if (raw == U"(") {
            count--;
            if (count == 0) {
              if (chars[j - 1].raw == U"&" && chars[j - 2].sign) {
                glue = chars[j - 1].ch;
                leftCh = chars[j - 2];
              }
              break;
            }
          }
        }
      }
      if (left.sign && right.sign) {
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
      } else if (leftCh.sign && glue) {
        // (d:d)&D&(t)
        if (glue == SignList::topStartInsertion || glue == SignList::bottomStartInsertion) {
          if (SignList::HasInsertion(leftCh.ch, Insertions::Type::TopEnd)) {
            chars[i] = CharBase(SignList::topEndInsertion, center.raw, center.rawOffset, center.resultOffset, true, false, U"");
          } else if (SignList::HasInsertion(leftCh.ch, Insertions::Type::BottomEnd)) {
            chars[i] = CharBase(SignList::bottomEndInsertion, center.raw, center.rawOffset, center.resultOffset, true, false, U"");
          }
        }
      } else if (left.sign && right.raw == U"(") {
        int count = 1;
        optional<size_t> term;
        for (size_t j = i + 2; j < chars.size(); j++) {
          auto raw = chars[j].raw;
          if (raw == U"(") {
            count++;
          } else if (raw == U")") {
            count--;
            if (count == 0) {
              term = j;
              break;
            }
          }
        }
        if (term) {
          if (SignList::HasInsertion(left.ch, Insertions::Type::BottomEnd)) {
            chars[i] = CharBase(SignList::bottomEndInsertion, center.raw, center.rawOffset, center.resultOffset, true, false, U"");
          } else if (SignList::HasInsertion(left.ch, Insertions::Type::TopEnd)) {
            chars[i] = CharBase(SignList::topEndInsertion, center.raw, center.rawOffset, center.resultOffset, true, false, U"");
          }
        }
      } else if (left.raw == U")" && right.sign) {
        int count = 1;
        optional<int> term;
        for (int j = (int)i - 2; j >= 0; j--) {
          auto raw = chars[j].raw;
          if (raw == U")") {
            count++;
          } else if (raw == U"(") {
            count--;
            if (count == 0) {
              term = j;
              break;
            }
          }
        }
        if (term) {
          optional<u32string> insertion;
          if (SignList::HasInsertion(right.ch, Insertions::Type::BottomStart)) {
            insertion = SignList::bottomStartInsertion;
          } else if (SignList::HasInsertion(right.ch, Insertions::Type::TopStart)) {
            insertion = SignList::topStartInsertion;
          }
          if (insertion) {
            for (int j = i + 1; j >= *term + 2; j--) {
              chars[j] = chars[j - 2];
            }
            chars[*term] = right;
            chars[*term + 1] = CharBase(*insertion, center.raw, center.rawOffset, center.resultOffset, true, false, U"");
          }
        }
      }
    }
    for (auto const &it : chars) {
      result += it.ch;
    }

    HbBufferUniquePtr buffer(Harfbuzz::CreateBuffer(result, font->fFont.get()));
    Harfbuzz::CreateGlyphInformations(buffer, font->fFont.get(), glyphs);

    uint32_t lastCluster = 0;
    int index = 0;
    optional<juce::Rectangle<float>> bb;
    float maxX = 0;
    for (auto const &info : glyphs) {
      juce::Path path = Harfbuzz::CreatePath(info.glyphId, font->fFont.get(), info.x, info.y);
      juce::Rectangle<float> bounds = path.getBounds();
      if (info.cluster != lastCluster) {
        auto sub = result.substr(lastCluster, info.cluster - lastCluster);
        clusters.push_back(Cluster(index, bb, lastCluster));
        index += sub.size();
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
    if (lastCluster < result.size()) {
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

    width = maxX;
  }

public:
  std::u32string result;
  std::vector<Cluster> clusters;
  float width;
  std::vector<Char> chars;
  HbBufferUniquePtr buffer;
  std::vector<GlyphInformation> glyphs;
  int rawOffset;
  std::u32string const raw;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Line)
};

} // namespace ksesh
