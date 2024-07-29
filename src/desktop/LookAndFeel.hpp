#pragma once

namespace ksesh {

class DarkLookAndFeel : public juce::LookAndFeel_V4 {
public:
  DarkLookAndFeel() {
  }

  void drawTextEditorOutline(juce::Graphics &, int width, int height, juce::TextEditor &) override {
  }
};

class LightLookAndFeel : public juce::LookAndFeel_V3 {
public:
  LightLookAndFeel() {
    setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::white);
    setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::black);
  }

  void drawTextEditorOutline(juce::Graphics &, int width, int height, juce::TextEditor &) override {
  }
};

} // namespace ksesh
