#pragma once

namespace ksesh {

struct CharBase {
  std::u32string fCh;
  std::optional<std::u32string> fCharPlain;
  std::u32string fRaw;
  int fRawOffset;
  int fResultOffset;
  bool fCtrl;
  bool fSign;

  CharBase(std::u32string ch,
           std::u32string raw,
           int rawOffset,
           int resultOffset,
           bool ctrl,
           bool sign,
           std::optional<std::u32string> charPlain = std::nullopt) : fCh(ch), fCharPlain(charPlain), fRaw(raw), fRawOffset(rawOffset), fResultOffset(resultOffset), fCtrl(ctrl), fSign(sign) {
  }
};

} // namespace ksesh
