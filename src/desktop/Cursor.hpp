#pragma once

namespace ksesh {

struct Cursor {
  struct SelectionRect {
    int lineIndex;
    juce::Rectangle<float> rect;
  };
  std::optional<SelectionRect> rect;
  std::vector<SelectionRect> selectionRects;
};

} // namespace ksesh
