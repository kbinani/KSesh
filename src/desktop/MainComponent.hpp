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
    fHieroglyph->setPresentationSetting(fSetting);

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
    for (int id = kCommandIDFirst + 1; id < kCommandIDMax; id++) {
      commands.add(id);
    }
  }

  void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &info) override {
    switch (commandID) {
    case CommandID::commandFileExportAsPng:
      info.setInfo(TRANS("Export as PNG"), {}, {}, 0);
      info.setActive((bool)fContent);
      break;
    case CommandID::commandFileExportAsPdf:
      info.setInfo(TRANS("Export as PDF"), {}, {}, 0);
      info.setActive((bool)fContent);
      break;
    case CommandID::commandFileExit:
      info.setInfo(TRANS("Exit"), {}, {}, 0);
      break;
    }
  }

  bool perform(juce::ApplicationCommandTarget::InvocationInfo const &info) override {
    switch (info.commandID) {
    case CommandID::commandFileExportAsPng:
      exportAsPng(1);
      return true;
    case CommandID::commandFileExportAsPdf:
      exportAsPdf();
      return true;
    case CommandID::commandFileExit:
      juce::JUCEApplication::getInstance()->quit();
      return true;
    }
    return false;
  }

private:
  void exportAsPng(float scale) {
    if (!fExportPngFileChooser) {
      fExportPngFileChooser = std::make_unique<juce::FileChooser>(TRANS("Export as PNG"), juce::File(), "*.png");
    }
    fExportPngFileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting, [this](juce::FileChooser const &chooser) {
      auto file = chooser.getResult();
      if (file == juce::File() || !fContent) {
        return;
      }
      PresentationSetting setting = fSetting;
      auto [widthf, heightf] = fContent->getSize(setting);
      int width = (int)ceil(widthf);
      int height = (int)ceil(heightf);
      juce::Image img(juce::Image::PixelFormat::ARGB, width, height, true);
      {
        juce::Graphics g(img);
        fContent->draw(g, fFont, setting);
      }
      auto stream = file.createOutputStream();
      auto title = TRANS("Error");
      auto message = TRANS("Failed to export as PNG");
      if (!stream || stream->failedToOpen()) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (stream->truncate().failed()) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (!stream->setPosition(0)) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      juce::PNGImageFormat format;
      if (!format.writeImageToStream(img, *stream)) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
    });
  }

  void exportAsPdf() {
    if (!fExportPdfFileChooser) {
      fExportPdfFileChooser = std::make_unique<juce::FileChooser>(TRANS("Export as PDF"), juce::File(), "*.pdf");
    }
    fExportPdfFileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting, [this](juce::FileChooser const &chooser) {
      auto file = chooser.getResult();
      if (file == juce::File() || !fContent) {
        return;
      }
      auto str = fContent->toPDF(fFont, fSetting);
      auto stream = file.createOutputStream();
      auto title = TRANS("Error");
      auto message = TRANS("Failed to export as PDF");
      if (str.empty() || !stream || stream->failedToOpen()) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (stream->truncate().failed()) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (!stream->setPosition(0)) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (!stream->write(str.c_str(), str.size())) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
    });
  }

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
    setContent(std::make_shared<Content>(str, fFont));
    if (!fIgnoreCaretChange) {
      auto typing = TextEditorComponent::GetTypingAtCaret(text, start, end);
      fSignList->setTyping(typing);
      fHieroglyph->setSelectedRange(start, end, direction);
    }
  }

  void setContent(std::shared_ptr<Content> content) {
    fContent = content;
    fHieroglyph->setContent(content);
  }

  void onClickSign(Sign const &sign) {
    fIgnoreCaretChange = true;
    auto result = fTextEditor->insert(sign.name);
    fTextEditor->focus();
    auto str = U32StringFromJuceString(result);
    setContent(std::make_shared<Content>(str, fFont));

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
  std::unique_ptr<juce::FileChooser> fExportPdfFileChooser;
  std::shared_ptr<Content> fContent;
  PresentationSetting fSetting;
  std::unique_ptr<juce::FileChooser> fExportPngFileChooser;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

} // namespace ksesh
