#pragma once

namespace ksesh {

class MainWindow : public juce::DocumentWindow {
public:
  MainWindow(juce::String name, HbFontUniquePtr const &font) : juce::DocumentWindow(name,
                                                                                    juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId),
                                                                                    juce::DocumentWindow::allButtons) {
    setUsingNativeTitleBar(true);
    fMain = std::make_unique<MainComponent>(font);
    setContentNonOwned(fMain.get(), true);

#if JUCE_IOS || JUCE_ANDROID
    setFullScreen(true);
#else
    setResizable(true, true);
    centreWithSize(getWidth(), getHeight());
#endif
    setResizeLimits(400, 300, std::numeric_limits<int>::max(), std::numeric_limits<int>::max());

    setVisible(true);
  }

  void closeButtonPressed() override {
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
  }

  void lookAndFeelChanged() override {
    setBackgroundColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
  }

private:
  std::unique_ptr<MainComponent> fMain;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};

} // namespace ksesh
