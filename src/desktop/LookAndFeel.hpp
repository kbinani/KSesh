#pragma once

namespace ksesh {

class LookAndFeel : public juce::LookAndFeel_V4 {
public:
  LookAndFeel() {
  }

  void drawTextEditorOutline(juce::Graphics &, int width, int height, juce::TextEditor &) override {
  }

  int getMenuWindowFlags() override {
    return 0;
  }
};

} // namespace ksesh
