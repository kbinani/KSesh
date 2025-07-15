#pragma once

namespace ksesh {

class Line {
public:
  Line(int rawOffset, std::u32string const &raw, std::shared_ptr<FontAdapter> const &font) : fRawOffset(rawOffset), fRaw(raw) {
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
      if (chars[i].fCh.starts_with(U"𓊈"s)) {
        for (size_t j = i + 1; j < chars.size(); j++) {
          auto &ch = chars[j];
          auto found = find_if(SignList::enclosureBeginning.begin(), SignList::enclosureBeginning.end(), [&](u32string const &c) {
            return ch.fCh.starts_with(c);
          });
          if (found != SignList::enclosureBeginning.end()) {
            break;
          }
          found = find_if(SignList::enclosureTerminal.begin(), SignList::enclosureTerminal.end(), [&](u32string const &c) {
            return ch.fCh.ends_with(c);
          });
          if (found != SignList::enclosureTerminal.end()) {
            ch.fCh = U"\U0001343d"s;
            break;
          }
        }
      }
    }
    for (size_t i = 1; i + 1 < chars.size(); i++) {
      auto center = chars[i];
      if (center.fCh != U"&") {
        continue;
      }
      auto left = chars[i - 1];
      auto right = chars[i + 1];
      CharBase leftCh = left;
      optional<u32string> glue;
      if (left.fRaw == U")") {
        int count = 1;
        for (int j = (int)i - 2; j >= 2; j--) {
          auto raw = chars[j].fRaw;
          if (raw == U")") {
            count++;
          } else if (raw == U"(") {
            count--;
            if (count == 0) {
              if (chars[j - 1].fRaw == U"&" && chars[j - 2].fSign) {
                glue = chars[j - 1].fCh;
                leftCh = chars[j - 2];
              }
              break;
            }
          }
        }
      }
      if (left.fSign && right.fSign) {
        auto type = SignList::InsertionType(left.fCh, right.fCh);
        if (type == Insertions::Type::TopStart) {
          chars[i - 1] = CharBase(right.fCh, left.fRaw, left.fRawOffset, left.fResultOffset, left.fCtrl, left.fSign, left.fCh);
          chars[i] = CharBase(SignList::topStartInsertion, center.fRaw, center.fRawOffset, center.fResultOffset, center.fCtrl, center.fSign, U"");
          chars[i + 1] = CharBase(left.fCh, right.fRaw, right.fRawOffset, right.fResultOffset, right.fCtrl, right.fSign, right.fCh);
        } else if (type == Insertions::Type::BottomStart) {
          chars[i - 1] = CharBase(right.fCh, left.fRaw, left.fRawOffset, left.fResultOffset, left.fCtrl, left.fSign, left.fCh);
          chars[i] = CharBase(SignList::bottomStartInsertion, center.fRaw, center.fRawOffset, center.fResultOffset, center.fCtrl, center.fSign, U"");
          chars[i + 1] = CharBase(left.fCh, right.fRaw, right.fRawOffset, right.fResultOffset, right.fCtrl, right.fSign, right.fCh);
        } else if (type == Insertions::Type::TopEnd) {
          chars[i] = CharBase(SignList::topEndInsertion, center.fRaw, center.fRawOffset, center.fResultOffset, center.fCtrl, center.fSign, U"");
        } else if (type == Insertions::Type::BottomEnd) {
          chars[i] = CharBase(SignList::bottomEndInsertion, center.fRaw, center.fRawOffset, center.fResultOffset, center.fCtrl, center.fSign, U"");
        }
        if (type == Insertions::Type::TopStart || type == Insertions::Type::BottomStart) {
          if (i + 3 < chars.size() && chars[i + 2].fCh == U"&" && chars[i + 3].fSign) {
            auto o = chars[i + 2];
            auto t = SignList::InsertionType(right.fCh, chars[i + 3].fCh);
            if (t == Insertions::Type::TopEnd) {
              chars[i + 2] = CharBase(SignList::topEndInsertion, o.fRaw, o.fRawOffset, o.fResultOffset, o.fCtrl, o.fSign, U"");
            } else if (t == Insertions::Type::BottomEnd) {
              chars[i + 2] = CharBase(SignList::bottomEndInsertion, o.fRaw, o.fRawOffset, o.fResultOffset, o.fCtrl, o.fSign, U"");
            }
            i += 3;
          }
        }
      } else if (leftCh.fSign && glue) {
        // (d:d)&D&(t)
        if (glue == SignList::topStartInsertion || glue == SignList::bottomStartInsertion) {
          if (SignList::HasInsertion(leftCh.fCh, Insertions::Type::TopEnd)) {
            chars[i] = CharBase(SignList::topEndInsertion, center.fRaw, center.fRawOffset, center.fResultOffset, true, false, U"");
          } else if (SignList::HasInsertion(leftCh.fCh, Insertions::Type::BottomEnd)) {
            chars[i] = CharBase(SignList::bottomEndInsertion, center.fRaw, center.fRawOffset, center.fResultOffset, true, false, U"");
          }
        }
      } else if (left.fSign && right.fRaw == U"(") {
        int count = 1;
        optional<size_t> term;
        for (size_t j = i + 2; j < chars.size(); j++) {
          auto raw = chars[j].fRaw;
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
          if (SignList::HasInsertion(left.fCh, Insertions::Type::BottomEnd)) {
            chars[i] = CharBase(SignList::bottomEndInsertion, center.fRaw, center.fRawOffset, center.fResultOffset, true, false, U"");
          } else if (SignList::HasInsertion(left.fCh, Insertions::Type::TopEnd)) {
            chars[i] = CharBase(SignList::topEndInsertion, center.fRaw, center.fRawOffset, center.fResultOffset, true, false, U"");
          }
        }
      } else if (left.fRaw == U")" && right.fSign) {
        int count = 1;
        optional<int> term;
        for (int j = (int)i - 2; j >= 0; j--) {
          auto raw = chars[j].fRaw;
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
          if (SignList::HasInsertion(right.fCh, Insertions::Type::BottomStart)) {
            insertion = SignList::bottomStartInsertion;
          } else if (SignList::HasInsertion(right.fCh, Insertions::Type::TopStart)) {
            insertion = SignList::topStartInsertion;
          }
          if (insertion) {
            for (int j = i + 1; j >= *term + 2; j--) {
              chars[j] = chars[j - 2];
            }
            chars[*term] = right;
            chars[*term + 1] = CharBase(*insertion, center.fRaw, center.fRawOffset, center.fResultOffset, true, false, U"");
          }
        }
      }
    }
    for (auto const &it : chars) {
      fResult += it.fCh;
    }

    HbBufferUniquePtr buffer(Harfbuzz::CreateBuffer(fResult, font->fFont.get()));
    Harfbuzz::CreateGlyphInformations(buffer, font->fFont.get(), fGlyphs);

    uint32_t lastCluster = 0;
    int index = 0;
    optional<juce::Rectangle<float>> bb;
    float maxX = 0;
    for (auto const &info : fGlyphs) {
      juce::Path path = Harfbuzz::CreatePath(info.fGlyphId, font->fFont.get(), info.fX, info.fY);
      if (info.fCluster != lastCluster) {
        auto sub = fResult.substr(lastCluster, info.fCluster - lastCluster);
        fClusters.push_back(Cluster(index, bb, lastCluster));
        index += sub.size();
        lastCluster = info.fCluster;
        bb = nullopt;
      }
      if (!path.isEmpty()) {
        juce::Rectangle<float> bounds = path.getBounds();
        maxX = std::max(maxX, bounds.getRight());
        if (bb) {
          bb = bb->getUnion(bounds);
        } else {
          bb = bounds;
        }
      }
    }
    fBuffer.swap(buffer);
    if (lastCluster < fResult.size()) {
      fClusters.push_back(Cluster(index, bb, lastCluster));
    }
    for (size_t i = 0; i < fClusters.size(); i++) {
      int from = fClusters[i].fResultOffset;
      int to =
          i + 1 < fClusters.size()
              ? fClusters[i + 1].fResultOffset
              : fResult.size();
      int cluster = fClusters[i].fCluster;
      for (CharBase const &ch : chars) {
        if (from <= ch.fResultOffset && ch.fResultOffset < to) {
          fChars.push_back(Char(ch, i, cluster));
        }
      }
    }

    fWidth = maxX;
  }

public:
  std::u32string fResult;
  std::vector<Cluster> fClusters;
  float fWidth;
  std::vector<Char> fChars;
  HbBufferUniquePtr fBuffer;
  std::vector<GlyphInformation> fGlyphs;
  int fRawOffset;
  std::u32string const fRaw;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Line)
};

} // namespace ksesh
