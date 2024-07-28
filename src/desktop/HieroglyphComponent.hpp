#pragma once

namespace ksesh {

class HieroglyphComponent : public juce::Component {
public:
  HieroglyphComponent() {
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(juce::Colours::green);
  }

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HieroglyphComponent)
};

} // namespace ksesh
