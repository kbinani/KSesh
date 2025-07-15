#pragma once

namespace ksesh {

class MainWindow : public juce::DocumentWindow, public juce::ApplicationCommandTarget {
public:
  MainWindow(
      juce::String name,
      std::unique_ptr<juce::ApplicationCommandManager> const &commandManager,
      std::shared_ptr<AppSetting> appSetting,
      FontSet const &fontSet)
      : juce::DocumentWindow(
            name,
            juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId),
            juce::DocumentWindow::allButtons) {
    setUsingNativeTitleBar(true);
    fMain = std::make_unique<MainComponent>(commandManager, appSetting, fontSet);
    commandManager->registerAllCommandsForTarget(fMain.get());
    fMain->onSaveFilePathChanged = [this](juce::File const &file, bool modified) {
      updateWindowTitle(file, modified);
    };
    setContentNonOwned(fMain.get(), true);

#if defined(JUCE_IOS) || defined(JUCE_ANDROID)
    setFullScreen(true);
#else
    setResizable(true, true);
    centreWithSize(getWidth(), getHeight());
#endif
    setResizeLimits(400, 300, std::numeric_limits<int>::max() / 2, std::numeric_limits<int>::max() / 2);
  }

  void closeButtonPressed() override {
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
  }

  void lookAndFeelChanged() override {
    setBackgroundColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
  }

  ApplicationCommandTarget *getNextCommandTarget() override {
    return fMain.get();
  }

  void getAllCommands(juce::Array<juce::CommandID> &commands) override {
  }

  void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) override {
  }

  bool perform(InvocationInfo const &info) override {
    return false;
  }

  void closeExample() {
    fMain->closeExample();
  }

  void closeAbout() {
    fMain->closeAbout();
  }

  void updateWindowTitle(juce::File const &file, bool modified) {
    auto name = file == juce::File() ? TRANS("Unnamed") : file.getFullPathName();
    setName(JUCE_APPLICATION_NAME_STRING + juce::String(" - ") + name + (modified ? " *" : ""));
  }

  void activeWindowStatusChanged() override {
    if (!isActiveWindow()) {
      return;
    }
    if (fMain) {
      fMain->assignInitialFocus();
    }
  }

private:
  std::unique_ptr<MainComponent> fMain;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};

} // namespace ksesh
