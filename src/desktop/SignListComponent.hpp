#pragma once

namespace ksesh {

class SignListComponent : public juce::Component {
public:
  explicit SignListComponent(juce::Typeface::Ptr typeface) : fTypeface(typeface) {
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(juce::Colours::blue);
  }

private:
  juce::Typeface::Ptr fTypeface;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SignListComponent)
};

} // namespace ksesh
