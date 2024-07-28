#pragma once

namespace ksesh {

class SignListComponent : public juce::Component {
public:
  SignListComponent() {
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(juce::Colours::blue);
  }

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SignListComponent)
};

} // namespace ksesh
