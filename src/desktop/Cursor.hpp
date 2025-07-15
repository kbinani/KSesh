#pragma once

namespace ksesh {

struct Cursor {
  struct SelectionRect {
    int fLineIndex;
    juce::Rectangle<float> fRect;
  };
  std::optional<SelectionRect> fRect;
  std::vector<SelectionRect> fSelectionRects;
};

} // namespace ksesh
