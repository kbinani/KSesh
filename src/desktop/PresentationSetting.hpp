#pragma once

namespace ksesh {

struct PresentationSetting {
  float fFontSize = 48;
  float fLineSpacingRatio = 0.25f;
  float fPadding = 8;
  float fCaretExpand = 2;

  float lineSpacing() const {
    return fFontSize * fLineSpacingRatio;
  }
};

} // namespace ksesh
