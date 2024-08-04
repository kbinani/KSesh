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

  ~MainComponent() {
#if JUCE_MAC
    juce::MenuBarModel::setMacMainMenu(nullptr);
#endif
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
    case commandFileNew:
      info.setInfo(TRANS("New"), {}, {}, 0);
      info.addDefaultKeypress('n', juce::ModifierKeys::commandModifier);
      break;
    case commandFileOpen:
      info.setInfo(TRANS("Open..."), {}, {}, 0);
      info.addDefaultKeypress('o', juce::ModifierKeys::commandModifier);
      break;
    case commandFileSave:
      info.setInfo(TRANS("Save"), {}, {}, 0);
      info.setActive((bool)fContent);
      info.addDefaultKeypress('s', juce::ModifierKeys::commandModifier);
      break;
    case commandFileSaveAs:
      info.setInfo(TRANS("Save As..."), {}, {}, 0);
      info.setActive((bool)fContent);
      info.addDefaultKeypress('s', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier);
      break;
    case commandFileExportAsPng1x:
      info.setInfo(TRANS("1x scale"), {}, {}, 0);
      info.setActive((bool)fContent);
      break;
    case commandFileExportAsPng2x:
      info.setInfo(TRANS("2x scale"), {}, {}, 0);
      info.setActive((bool)fContent);
      break;
    case commandFileExportAsPng4x:
      info.setInfo(TRANS("4x scale"), {}, {}, 0);
      info.setActive((bool)fContent);
      break;
    case commandFileExportAsPng8x:
      info.setInfo(TRANS("8x scale"), {}, {}, 0);
      info.setActive((bool)fContent);
      break;
    case commandFileExportAsPdf:
      info.setInfo(TRANS("Export as PDF"), {}, {}, 0);
      info.setActive((bool)fContent);
      break;
#if JUCE_WINDOWS
    case commandFileExportAsEmf:
      info.setInfo(TRANS("Export as EMF"), {}, {}, 0);
      info.setActive((bool)fContent);
      break;
    case commandEditCopyAsEmf:
      info.setInfo(TRANS("Copy as EMF"), {}, {}, 0);
      info.setActive((bool)fContent);
      break;
#endif
    case commandFileExit:
      info.setInfo(TRANS("Exit"), {}, {}, 0);
      break;
    case commandEditCopyAsImage1x:
      info.setInfo(TRANS("1x scale"), {}, {}, 0);
      info.setActive((bool)fContent);
      break;
    case commandEditCopyAsImage2x:
      info.setInfo(TRANS("2x scale"), {}, {}, 0);
      info.setActive((bool)fContent);
      break;
    case commandEditCopyAsImage4x:
      info.setInfo(TRANS("4x scale"), {}, {}, 0);
      info.setActive((bool)fContent);
      break;
    case commandEditCopyAsImage8x:
      info.setInfo(TRANS("8x scale"), {}, {}, 0);
      info.setActive((bool)fContent);
      break;
    }
  }

  bool perform(juce::ApplicationCommandTarget::InvocationInfo const &info) override {
    switch (info.commandID) {
    case commandFileNew:
      warnDirtyThen([this]() {
        newDocument();
      });
      return true;
    case commandFileOpen:
      warnDirtyThen([this]() {
        openDocument();
      });
      return true;
    case commandFileSave:
      saveDocument();
      return true;
    case commandFileSaveAs:
      saveDocumentWithNewName();
      return true;
    case commandFileExportAsPng1x:
      exportAsPng(1);
      return true;
    case commandFileExportAsPng2x:
      exportAsPng(2);
      return true;
    case commandFileExportAsPng4x:
      exportAsPng(4);
      return true;
    case commandFileExportAsPng8x:
      exportAsPng(8);
      return true;
    case commandFileExportAsPdf:
      exportAsPdf();
      return true;
#if JUCE_WINDOWS
    case commandFileExportAsEmf:
      exportAsEmf();
      return true;
    case commandEditCopyAsEmf:
      copyAsEmf();
      return true;
#endif
    case commandFileExit:
      warnDirtyThen([]() {
        juce::JUCEApplication::getInstance()->quit();
      });
      return true;
    case commandEditCopyAsImage1x:
    case commandEditCopyAsImage2x:
    case commandEditCopyAsImage4x:
    case commandEditCopyAsImage8x:
      float scale = powf(2,  info.commandID - commandEditCopyAsImage1x);
      copyAsPng(scale);
      return true;
    }
    return false;
  }

private:
  void warnDirtyThen(std::function<void()> then) {
    if (!then) {
      return;
    }
    if (!fDirty) {
      then();
      return;
    }
    auto options = juce::MessageBoxOptions()
                       .withIconType(juce::MessageBoxIconType::WarningIcon)
                       .withTitle(TRANS("Warning"))
                       .withMessage(TRANS("Do you want to save the changes you made to new document?"))
                       .withButton(TRANS("Save"))       // yes
                       .withButton(TRANS("Don't Save")) // no
                       .withButton(TRANS("Cancel"));    // cancel
    juce::NativeMessageBox::showAsync(options, [this, then](int result) {
      switch (result) {
      case 0:
        // save
        saveDocument(then);
        break;
      case 1:
        then();
        // don't save
        break;
      case 2:
        // cancel
        break;
      }
    });
  }

  void newDocument() {
    fSave = juce::File();
    fDirty = false;
    auto c = std::make_shared<Content>(U"", fFont);
    setContent(c);
    fHieroglyph->setSelectedRange(0, 0, Direction::Forward);
    fTextEditor->resetText("");
    if (onSaveFilePathChanged) {
      onSaveFilePathChanged(fSave, false);
    }
  }

  void openDocument() {
    if (!fOpenFileChooser) {
      fOpenFileChooser = std::make_unique<juce::FileChooser>(TRANS("Open"), juce::File(), "*.txt");
    }
    fOpenFileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [this](juce::FileChooser const &chooser) {
      auto file = chooser.getResult();
      if (file == juce::File()) {
        return;
      }
      auto stream = file.createInputStream();
      if (!stream || stream->failedToOpen()) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, TRANS("Error"), TRANS("Failed to open"));
        return;
      }
      auto str = stream->readString();
      auto c = std::make_shared<Content>(U32StringFromJuceString(str), fFont);
      setContent(c);
      fSave = file;
      fDirty = false;
      fHieroglyph->setSelectedRange(str.length(), str.length(), Direction::Forward);
      fTextEditor->resetText(str);
      if (onSaveFilePathChanged) {
        onSaveFilePathChanged(file, false);
      }
    });
  }

  void saveDocumentWithNewName(std::function<void()> then = nullptr) {
    if (!fContent) {
      return;
    }
    if (!fSaveFileChooser) {
      fSaveFileChooser = std::make_unique<juce::FileChooser>(TRANS("Save"), juce::File(), "*.txt");
    }
    fSaveFileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting, [this, then](juce::FileChooser const &chooser) {
      auto file = chooser.getResult();
      if (file == juce::File() || !fContent) {
        return;
      }
      if (saveDocumentTo(file)) {
        fDirty = false;
        fSave = file;
        if (onSaveFilePathChanged) {
          onSaveFilePathChanged(file, false);
        }
        if (then) {
          waitModalThen(then);
        }
      } else {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, TRANS("Error"), TRANS("Failed to save"));
      }
    });
  }

  void waitModalThen(std::function<void()> then) {
    std::function<void()> func;
    func = [then, func]() {
      if (juce::Component::getNumCurrentlyModalComponents() != 0) {
        func();
      } else {
        then();
      }
    };
    juce::Timer::callAfterDelay(100, func);
  }

  void saveDocument(std::function<void()> then = nullptr) {
    if (fSave != juce::File()) {
      if (saveDocumentTo(fSave)) {
        fDirty = false;
        if (onSaveFilePathChanged) {
          onSaveFilePathChanged(fSave, false);
        }
        if (then) {
          waitModalThen(then);
        }
      } else {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, TRANS("Error"), TRANS("Failed to save"));
      }
    } else {
      saveDocumentWithNewName(then);
    }
  }

  bool saveDocumentTo(juce::File const &file) const {
    if (!fContent) {
      return false;
    }
    if (file == juce::File()) {
      return false;
    }
    std::u32string ret;
    for (auto const &line : fContent->lines) {
      if (!ret.empty()) {
        ret += U"\n";
      }
      ret += line->raw;
    }
    auto stream = file.createOutputStream();
    if (!stream || stream->failedToOpen()) {
      return false;
    }
    if (!stream->setPosition(0)) {
      return false;
    }
    if (stream->truncate().failed()) {
      return false;
    }
    auto u8 = U8StringFromU32String(ret);
    return stream->write(u8.c_str(), u8.size());
  }

  void exportAsPng(float scale) {
    if (!fExportPngFileChooser) {
      fExportPngFileChooser = std::make_unique<juce::FileChooser>(TRANS("Export as PNG"), juce::File(), "*.png");
    }
    fExportPngFileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting, [this, scale](juce::FileChooser const &chooser) {
      auto file = chooser.getResult();
      if (file == juce::File() || !fContent) {
        return;
      }
      auto img = fContent->toImage(fFont, fSetting, scale);
      auto stream = file.createOutputStream();
      auto title = TRANS("Error");
      auto message = TRANS("Failed to export as PNG");
      if (!stream || stream->failedToOpen()) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (!stream->setPosition(0)) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (stream->truncate().failed()) {
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
      if (!stream->setPosition(0)) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (stream->truncate().failed()) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (!stream->write(str.c_str(), str.size())) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
    });
  }

  void exportAsEmf() {
    if (!fExportEmfFileChooser) {
      fExportEmfFileChooser = std::make_unique<juce::FileChooser>(TRANS("Export as EMF"), juce::File(), "*.emf");
    }
    fExportEmfFileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting, [this](juce::FileChooser const &chooser) {
      auto file = chooser.getResult();
      if (file == juce::File() || !fContent) {
        return;
      }
      auto str = fContent->toEMF(fFont, fSetting);
      auto stream = file.createOutputStream();
      auto title = TRANS("Error");
      auto message = TRANS("Failed to export as EMF");
      if (str.empty() || !stream || stream->failedToOpen()) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (!stream->setPosition(0)) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (stream->truncate().failed()) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (!stream->write(str.c_str(), str.size())) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
    });
  }

  void copyAsEmf() {
    if (!fContent) {
      return;
    }
    auto str = fContent->toEMF(fFont, fSetting);
    writeToClipboard(str, Clipboard::Type::Emf);
  }

  void copyAsPng(float scale) {
    auto img = fContent->toImage(fFont, fSetting, scale);
    juce::PNGImageFormat format;
    juce::MemoryOutputStream stream;
    format.writeImageToStream(img, stream);
    std::string_view data((char const *)stream.getData(), stream.getDataSize());
    writeToClipboard(data, Clipboard::Type::Png);
  }

  void writeToClipboard(std::string_view data, Clipboard::Type type) {
    if (!Clipboard::Store(data, type)) {
      auto title = TRANS("Error");
      auto message = TRANS("Failed to write clipboard");
      juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
    }
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
    fMenuModel->menuItemsChanged();
    bool wasDirty = fDirty;
    fDirty = true;
    if (!wasDirty && fSave != juce::File() && onSaveFilePathChanged) {
      onSaveFilePathChanged(fSave, fDirty);
    }
  }

  void onClickSign(Sign const &sign) {
    fIgnoreCaretChange = true;
    auto result = fTextEditor->insert(sign.name);
    fTextEditor->focus();
    auto str = U32StringFromJuceString(result);
    setContent(std::make_shared<Content>(str, fFont));

    fIgnoreCaretChange = false;
  }

public:
  std::function<void(juce::File, bool modified)> onSaveFilePathChanged;

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
  std::unique_ptr<juce::FileChooser> fSaveFileChooser;
  juce::File fSave;
  bool fDirty = false;
  std::unique_ptr<juce::FileChooser> fOpenFileChooser;
  std::unique_ptr<juce::FileChooser> fExportEmfFileChooser;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

} // namespace ksesh
