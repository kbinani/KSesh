#pragma once

namespace ksesh {

class SignListComponent : public juce::Component {
public:
  explicit SignListComponent(HbFontUniquePtr const &font) : fFont(font) {
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(juce::Colours::blue);
  }

private:
  HbFontUniquePtr const &fFont;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SignListComponent)
};

} // namespace ksesh
