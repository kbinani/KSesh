#pragma once

namespace ksesh {

class HieroglyphComponent : public juce::Component {
public:
  explicit HieroglyphComponent(juce::Typeface::Ptr typeface) : fTypeface(typeface) {
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(juce::Colours::green);
  }

private:
  juce::Typeface::Ptr fTypeface;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HieroglyphComponent)
};

} // namespace ksesh
