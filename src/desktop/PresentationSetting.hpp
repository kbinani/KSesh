#pragma once

namespace ksesh {

struct PresentationSetting {
  float fontSize = 48;
  float lineSpacingRatio = 0.25f;
  float padding = 8;
  float caretExpand = 2;

  float lineSpacing() const {
    return fontSize * lineSpacingRatio;
  }
};

} // namespace ksesh
