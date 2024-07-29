#pragma once

namespace ksesh {

class LookAndFeel : public juce::LookAndFeel_V4 {
public:
  void drawTextEditorOutline(juce::Graphics &, int width, int height, juce::TextEditor &) override {
  }
};

} // namespace ksesh
