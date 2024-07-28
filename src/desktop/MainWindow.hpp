#pragma once

namespace ksesh {

class MainWindow : public juce::DocumentWindow {
public:
  explicit MainWindow(juce::String name) : juce::DocumentWindow(name,
                                                                juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId),
                                                                juce::DocumentWindow::allButtons) {
    setUsingNativeTitleBar(true);

#if JUCE_IOS || JUCE_ANDROID
    setFullScreen(true);
#else
    setResizable(true, true);
    centreWithSize(getWidth(), getHeight());
#endif

    setVisible(true);
  }

  void closeButtonPressed() override {
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
  }

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};

} // namespace ksesh
