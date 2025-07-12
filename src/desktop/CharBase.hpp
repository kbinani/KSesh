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

} // namespace ksesh
