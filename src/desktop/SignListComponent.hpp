#pragma once

namespace ksesh {

class SignListComponent : public juce::Component {
public:
  explicit SignListComponent(FontData const &font) : fFont(font) {
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(juce::Colours::blue);
  }

private:
  FontData const &fFont;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SignListComponent)
};

} // namespace ksesh
