#pragma once

namespace ksesh {

class TextEditorComponent : public juce::Component {
public:
  TextEditorComponent() {
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(juce::Colours::red);
  }

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextEditorComponent)
};

} // namespace ksesh
