#pragma once

namespace ksesh {

class MainComponent : public juce::Component, public juce::Timer, public juce::ApplicationCommandTarget {
  enum : int {
    resizerSize = 8,
  };

public:
  explicit MainComponent(HbFontUniquePtr const &font) : fFont(font) {
    int const width = 1280;
    int const height = 720;

    fTextEditor = std::make_unique<TextEditorComponent>();
    fTextEditor->setBounds(0, 0, width / 2 - resizerSize / 2, height / 2 - resizerSize / 2);
    fTextEditor->onTextChange = [this](juce::String const &text, int start, int end, Direction direction) {
      this->textEditorDidChangeText(text, start, end, direction);
    };
    fTextEditor->onCaretPositionChange = [this](juce::String const &text, int start, int end, Direction direction) {
      this->textEditorDidChangeCaretPosition(text, start, end, direction);
    };

    fHieroglyph = std::make_unique<HieroglyphComponent>(font);
    fHieroglyph->setBounds(width / 2 + resizerSize / 2, 0, width / 2 - resizerSize / 2, height / 2 - resizerSize / 2);
    fHieroglyph->onSelectedRangeChange = [this](int start, int end, Direction direction) {
      this->hieroglyphDidChangeSelectedRange(start, end, direction);
    };

    fVerticalSplitter = std::make_unique<SplitterComponent>(fTextEditor.get(), fHieroglyph.get(), true);
    fVerticalSplitter->setBounds(width / 2 - resizerSize / 2, 0, resizerSize, height / 2 - resizerSize / 2);

    fSignList = std::make_unique<SignListComponent>(font);
    fSignList->setBounds(0, height / 2 + resizerSize / 2, width, height / 2 - resizerSize / 2);
    fSignList->onClickSign = [this](Sign const &sign) {
      onClickSign(sign);
    };

    fHorizontalSplitter = std::make_unique<SplitterComponent>(fVerticalSplitter.get(), fSignList.get(), false);
    fHorizontalSplitter->setBounds(0, height / 2 + resizerSize / 2, width, resizerSize);
    addAndMakeVisible(*fHorizontalSplitter);

    fCommandManager = std::make_unique<juce::ApplicationCommandManager>();
    fCommandManager->registerAllCommandsForTarget(this);
    addKeyListener(fCommandManager->getKeyMappings());

    fMenuModel = std::make_unique<MenuBarModel>(fCommandManager.get());
#if JUCE_MAC
    juce::MenuBarModel::setMacMainMenu(fMenuModel.get());
#else
    fMenuComponent = std::make_unique<juce::MenuBarComponent>(fMenuModel.get());
    addAndMakeVisible(*fMenuComponent);
#endif

    setSize(width, height);
    startTimerHz(1);
  }

  void resized() override {
    juce::Rectangle<int> bounds(0, 0, getWidth(), getHeight());
#if !JUCE_MAC
    auto menuBarHeight = getLookAndFeel().getDefaultMenuBarHeight();
    fMenuComponent->setBounds(bounds.removeFromTop(menuBarHeight));
#endif
    fHorizontalSplitter->setBounds(bounds);
  }

  void timerCallback() override {
    if (auto root = getTopLevelComponent(); root && root->isOnDesktop()) {
      fTextEditor->focus();
      stopTimer();
    }
  }

  juce::ApplicationCommandTarget *getNextCommandTarget() override {
    return nullptr;
  }

  void getAllCommands(juce::Array<juce::CommandID> &commands) override {
  }

  void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) override {
  }

  bool perform(juce::ApplicationCommandTarget::InvocationInfo const &info) override {
    return false;
  }

private:
  void hieroglyphDidChangeSelectedRange(int start, int end, Direction direction) {
    fTextEditor->setSelectedRange(start, end, direction);
  }

  void textEditorDidChangeCaretPosition(juce::String const &text, int start, int end, Direction direction) {
    if (!fIgnoreCaretChange) {
      auto typing = TextEditorComponent::GetTypingAtCaret(text, start, end);
      fSignList->setTyping(typing);
      fHieroglyph->setSelectedRange(start, end, direction);
    }
  }

  void textEditorDidChangeText(juce::String const &text, int start, int end, Direction direction) {
    auto str = U32StringFromJuceString(text);
    auto c = std::make_shared<Content>(str, fFont);
    fHieroglyph->setContent(c);
    if (!fIgnoreCaretChange) {
      auto typing = TextEditorComponent::GetTypingAtCaret(text, start, end);
      fSignList->setTyping(typing);
      fHieroglyph->setSelectedRange(start, end, direction);
    }
  }

  void onClickSign(Sign const &sign) {
    fIgnoreCaretChange = true;
    auto result = fTextEditor->insert(sign.name);
    fTextEditor->focus();
    auto str = U32StringFromJuceString(result);
    auto c = std::make_shared<Content>(str, fFont);
    fHieroglyph->setContent(c);

    fIgnoreCaretChange = false;
  }

private:
  std::unique_ptr<SplitterComponent> fVerticalSplitter;
  std::unique_ptr<SplitterComponent> fHorizontalSplitter;
  std::unique_ptr<TextEditorComponent> fTextEditor;
  std::unique_ptr<HieroglyphComponent> fHieroglyph;
  std::unique_ptr<SignListComponent> fSignList;
  HbFontUniquePtr const &fFont;
  bool fIgnoreCaretChange = false;
  std::unique_ptr<juce::ApplicationCommandManager> fCommandManager;
  std::unique_ptr<juce::MenuBarModel> fMenuModel;
#if !JUCE_MAC
  std::unique_ptr<juce::MenuBarComponent> fMenuComponent;
#endif

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

} // namespace ksesh
