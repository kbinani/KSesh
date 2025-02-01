#pragma once

namespace ksesh {

class MainWindow : public juce::DocumentWindow {
public:
  MainWindow(
      juce::String name,
      std::shared_ptr<hb_font_t> const &font,
      std::unique_ptr<juce::ApplicationCommandManager> const &commandManager,
      std::shared_ptr<AppSetting> appSetting)
      : juce::DocumentWindow(
            name,
            juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId),
            juce::DocumentWindow::allButtons) {
    setUsingNativeTitleBar(true);
    fMain = std::make_unique<MainComponent>(font, commandManager, appSetting);
    fMain->onSaveFilePathChanged = [this](juce::File const &file, bool modified) {
      auto name = file == juce::File() ? TRANS("Unnamed") : file.getFullPathName();
      setName(JUCE_APPLICATION_NAME_STRING + juce::String(" - ") + name + (modified ? " *" : ""));
    };
    setContentNonOwned(fMain.get(), true);

#if defined(JUCE_IOS) || defined(JUCE_ANDROID)
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

  void closeExample() {
    fMain->closeExample();
  }

  void closeAbout() {
    fMain->closeAbout();
  }

private:
  std::unique_ptr<MainComponent> fMain;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};

} // namespace ksesh
