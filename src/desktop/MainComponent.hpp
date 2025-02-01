#pragma once

namespace ksesh {

class MainComponent : public juce::Component, public juce::Timer, public juce::ApplicationCommandTarget, public TextEditorComponent::Delegate {
  enum : int {
    resizerSize = 8,
    bottomBarHeight = 24,
  };

  enum class FocusOwner {
    textEditor,
    signList,
  };

  struct ModalFinishDetector : public juce::ModalComponentManager::Callback {
    explicit ModalFinishDetector(std::shared_ptr<int> counter) : fCounter(counter) {
    }

    void modalStateFinished(int returnValue) override {
      *fCounter = *fCounter - 1;
    }

    std::shared_ptr<int> const fCounter;
  };

public:
  MainComponent(std::shared_ptr<hb_font_t> const &font, std::unique_ptr<juce::ApplicationCommandManager> const &commandManager, std::shared_ptr<AppSetting> appSetting) : fFont(font), fAppSetting(appSetting), fNumModalComponents(std::make_shared<int>(0)), fNativeMessageBoxCoroner(fNumModalComponents) {
    int const width = 1280;
    int const height = 720;

    fTextEditor = std::make_unique<TextEditorComponent>(font, appSetting);
    fTextEditor->setBounds(0, 0, width / 2 - resizerSize / 2, height / 2 - resizerSize / 2);
    fTextEditor->fDelegate = this;

    fHieroglyph = std::make_unique<HieroglyphComponent>(appSetting);
    fHieroglyph->setBounds(width / 2 + resizerSize / 2, 0, width / 2 - resizerSize / 2, height / 2 - resizerSize / 2);
    fHieroglyph->onSelectedRangeChange = [this](int start, int end, Direction direction) {
      this->hieroglyphDidChangeSelectedRange(start, end, direction);
    };

    fVerticalSplitter = std::make_unique<SplitterComponent>(fTextEditor.get(), fHieroglyph.get(), true);
    fVerticalSplitter->setBounds(width / 2 - resizerSize / 2, 0, resizerSize, height / 2 - resizerSize / 2);
    fHieroglyph->setVisible(appSetting->isPreviewEnabled());

    fSignList = std::make_unique<SignListComponent>(font);
    fSignList->setBounds(0, height / 2 + resizerSize / 2, width, height / 2 - resizerSize / 2);
    fSignList->onClickSign = [this](Sign const &sign) {
      onClickSign(sign);
    };
    fSignList->onClickCategory = [this]() {
      if (fFocusOwner == FocusOwner::textEditor) {
        fTextEditor->focus();
      }
    };
    fSignList->onClickBackground = [this]() {
      setFocusOwner(FocusOwner::signList);
    };

    fHorizontalSplitter = std::make_unique<SplitterComponent>(fVerticalSplitter.get(), fSignList.get(), false);
    fHorizontalSplitter->setBounds(0, height / 2 + resizerSize / 2, width, resizerSize);
    addAndMakeVisible(*fHorizontalSplitter);

    fBottomToolBar = std::make_unique<BottomToolBar>();
    addAndMakeVisible(*fBottomToolBar);

    commandManager->registerAllCommandsForTarget(this);
    addKeyListener(commandManager->getKeyMappings());

    fMenuModel = std::make_unique<MenuBarModel>(commandManager.get(), fAppSetting);
#if defined(JUCE_MAC)
    juce::MenuBarModel::setMacMainMenu(fMenuModel.get());
#else
    fMenuComponent = std::make_unique<juce::MenuBarComponent>(fMenuModel.get());
    addAndMakeVisible(*fMenuComponent);
#endif
    fMenuModel->onRecentFileClicked = [this](juce::File file) {
      if (fSave == file) {
        return;
      }
      warnDirtyThen([this, file]() {
        open(file);
      });
    };

    setSize(width, height);
    startTimerHz(1);
    updateOverlayColors();
  }

  ~MainComponent() {
#if defined(JUCE_MAC)
    juce::MenuBarModel::setMacMainMenu(nullptr);
#endif
    removeAllChildren();
    fHorizontalSplitter.reset();
  }

  void resized() override {
    auto bounds = getLocalBounds();
#if !defined(JUCE_MAC)
    auto menuBarHeight = getLookAndFeel().getDefaultMenuBarHeight();
    fMenuComponent->setBounds(bounds.removeFromTop(menuBarHeight));
#endif
    fBottomToolBar->setBounds(bounds.removeFromBottom(bottomBarHeight));
    fHorizontalSplitter->setBounds(bounds);
    if (fAbout) {
      fAbout->setBounds(getLocalBounds());
    }
    if (fExample) {
      fExample->setBounds(getLocalBounds());
    }
  }

  void timerCallback() override {
    if (auto root = getTopLevelComponent(); root && root->isOnDesktop()) {
      fTextEditor->focus();
      updateMenuModel();
      stopTimer();
    }
  }

  juce::ApplicationCommandTarget *getNextCommandTarget() override {
    return nullptr;
  }

  void getAllCommands(juce::Array<juce::CommandID> &commands) override {
    for (int id = mainComponentCommandIDBegin; id < mainComponentCommandIDEnd; id++) {
      commands.add(id);
    }
  }

  void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &info) override {
    switch (commandID) {
    case commandFileNew:
      info.setInfo(TRANS("New"), {}, {}, 0);
      info.addDefaultKeypress('n', juce::ModifierKeys::commandModifier);
      return;
    case commandFileOpen:
      info.setInfo(TRANS("Open") + "...", {}, {}, 0);
      info.addDefaultKeypress('o', juce::ModifierKeys::commandModifier);
      return;
    case commandFileSave:
      info.setInfo(TRANS("Save"), {}, {}, 0);
      info.setActive((bool)fContent);
      info.addDefaultKeypress('s', juce::ModifierKeys::commandModifier);
      return;
    case commandFileSaveAs:
      info.setInfo(TRANS("Save As") + "...", {}, {}, 0);
      info.setActive((bool)fContent);
      info.addDefaultKeypress('s', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier);
      return;
    case commandFileExportAsPng1x:
      attachScaledMenuInfo(1, info);
      return;
    case commandFileExportAsPng2x:
      attachScaledMenuInfo(2, info);
      return;
    case commandFileExportAsPng4x:
      attachScaledMenuInfo(4, info);
      return;
    case commandFileExportAsPng8x:
      attachScaledMenuInfo(8, info);
      return;
    case commandFileExportAsPdf:
      info.setInfo(TRANS("Export as PDF"), {}, {}, 0);
      info.setActive((bool)fContent);
      return;
    case commandEditCopyAsUnicodeWithoutFormatControl:
      info.setInfo(TRANS("Copy text"), {}, {}, 0);
      info.setActive((bool)fContent);
      return;
    case commandEditCopyAsUnicodeWithFormatControl:
      info.setInfo(TRANS("Copy text with format control"), {}, {}, 0);
      info.setActive((bool)fContent);
      return;
#if defined(JUCE_WINDOWS)
    case commandFileExportAsEmf:
      info.setInfo(TRANS("Export as EMF"), {}, {}, 0);
      info.setActive((bool)fContent);
      return;
    case commandEditCopyAsEmf:
      info.setInfo(TRANS("Copy as EMF"), {}, {}, 0);
      info.setActive((bool)fContent);
      return;
#endif
    case commandFileExit:
      info.setInfo(TRANS("Exit"), {}, {}, 0);
      return;
    case commandEditSwitchFocus:
      switch (fFocusOwner) {
      case FocusOwner::textEditor:
        info.setInfo(TRANS("Focus sign list"), {}, {}, 0);
        break;
      case FocusOwner::signList:
        info.setInfo(TRANS("Focus text editor"), {}, {}, 0);
        break;
      }
      info.addDefaultKeypress('\t', 0);
      return;
    case commandEditSignListUseSign:
      info.setInfo(TRANS("Use selected sign"), {}, {}, 0);
      info.addDefaultKeypress(juce::KeyPress::returnKey, 0);
      info.setActive(fFocusOwner == FocusOwner::signList);
      return;
    case commandEditSignListMoveLeft:
      info.setInfo(TRANS("Select the left sign"), {}, {}, juce::ApplicationCommandInfo::hiddenFromKeyEditor);
      info.addDefaultKeypress(juce::KeyPress::leftKey, 0);
      info.setActive(fFocusOwner == FocusOwner::signList);
      return;
    case commandEditSignListMoveUp:
      info.setInfo(TRANS("Select the sign above"), {}, {}, juce::ApplicationCommandInfo::hiddenFromKeyEditor);
      info.addDefaultKeypress(juce::KeyPress::upKey, 0);
      info.setActive(fFocusOwner == FocusOwner::signList);
      return;
    case commandEditSignListMoveRight:
      info.setInfo(TRANS("Select the right sign"), {}, {}, juce::ApplicationCommandInfo::hiddenFromKeyEditor);
      info.addDefaultKeypress(juce::KeyPress::rightKey, 0);
      info.setActive(fFocusOwner == FocusOwner::signList);
      return;
    case commandEditSignListMoveDown:
      info.setInfo(TRANS("Select the sign below"), {}, {}, juce::ApplicationCommandInfo::hiddenFromKeyEditor);
      info.addDefaultKeypress(juce::KeyPress::downKey, 0);
      info.setActive(fFocusOwner == FocusOwner::signList);
      return;
    case commandEditSignListCategoryNext:
      info.setInfo(TRANS("Select next category"), {}, {}, juce::ApplicationCommandInfo::hiddenFromKeyEditor);
#if defined(JUCE_WINDOWS)
      info.addDefaultKeypress('\t', juce::ModifierKeys::ctrlModifier);
#else
      info.addDefaultKeypress('}', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier);
#endif
      info.setActive(fFocusOwner == FocusOwner::signList);
      return;
    case commandEditSignListCategoryPrev:
      info.setInfo(TRANS("Select previous category"), {}, {}, juce::ApplicationCommandInfo::hiddenFromKeyEditor);
#if defined(JUCE_WINDOWS)
      info.addDefaultKeypress('\t', juce::ModifierKeys::ctrlModifier | juce::ModifierKeys::shiftModifier);
#else
      info.addDefaultKeypress('{', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier);
#endif
      info.setActive(fFocusOwner == FocusOwner::signList);
      return;
    case commandEditSignListCategoryA:
    case commandEditSignListCategoryB:
    case commandEditSignListCategoryC:
    case commandEditSignListCategoryD:
    case commandEditSignListCategoryE:
    case commandEditSignListCategoryF:
    case commandEditSignListCategoryG:
    case commandEditSignListCategoryH:
    case commandEditSignListCategoryI: {
      char8_t ch = u8'A' + (commandID - commandEditSignListCategoryA);
      std::u8string v;
      v += ch;
      info.setInfo(TRANS("Select ") + JuceStringFromU8String(v) + TRANS(" category"), {}, {}, juce::ApplicationCommandInfo::hiddenFromKeyEditor);
      info.addDefaultKeypress('a' + (commandID - commandEditSignListCategoryA), 0);
      info.setActive(fFocusOwner == FocusOwner::signList);
      return;
    }
    case commandEditSignListCategoryK:
    case commandEditSignListCategoryL:
    case commandEditSignListCategoryM:
    case commandEditSignListCategoryN:
    case commandEditSignListCategoryO:
    case commandEditSignListCategoryP:
    case commandEditSignListCategoryQ:
    case commandEditSignListCategoryR:
    case commandEditSignListCategoryS:
    case commandEditSignListCategoryT:
    case commandEditSignListCategoryU:
    case commandEditSignListCategoryV:
    case commandEditSignListCategoryW:
    case commandEditSignListCategoryX:
    case commandEditSignListCategoryY:
    case commandEditSignListCategoryZ: {
      char8_t ch = u8'K' + (commandID - commandEditSignListCategoryK);
      std::u8string v;
      v += ch;
      info.setInfo(TRANS("Select ") + JuceStringFromU8String(v) + TRANS(" category"), {}, {}, juce::ApplicationCommandInfo::hiddenFromKeyEditor);
      info.addDefaultKeypress('k' + (commandID - commandEditSignListCategoryK), 0);
      info.setActive(fFocusOwner == FocusOwner::signList);
      return;
    }
    case commandEditSignListCategoryAa:
      info.setInfo(TRANS("Select ") + juce::String("Aa") + TRANS(" category"), {}, {}, juce::ApplicationCommandInfo::hiddenFromKeyEditor);
      info.addDefaultKeypress('a', juce::ModifierKeys::shiftModifier);
      info.setActive(fFocusOwner == FocusOwner::signList);
      return;
    case commandEditSignListCategoryTall:
      info.setInfo(TRANS("Select ") + juce::String("tall") + TRANS(" category"), {}, {}, juce::ApplicationCommandInfo::hiddenFromKeyEditor);
      info.addDefaultKeypress('t', juce::ModifierKeys::shiftModifier);
      info.setActive(fFocusOwner == FocusOwner::signList);
      return;
    case commandEditSignListCategoryWide:
      info.setInfo(TRANS("Select ") + juce::String("wide") + TRANS(" category"), {}, {}, juce::ApplicationCommandInfo::hiddenFromKeyEditor);
      info.addDefaultKeypress('w', juce::ModifierKeys::shiftModifier);
      info.setActive(fFocusOwner == FocusOwner::signList);
      return;
    case commandEditSignListCategorySmall:
      info.setInfo(TRANS("Select ") + juce::String("small") + TRANS(" category"), {}, {}, juce::ApplicationCommandInfo::hiddenFromKeyEditor);
      info.addDefaultKeypress('s', juce::ModifierKeys::shiftModifier);
      info.setActive(fFocusOwner == FocusOwner::signList);
      return;
    case commandEditCopyAsImage1x:
      attachScaledMenuInfo(1, info);
      return;
    case commandEditCopyAsImage2x:
      attachScaledMenuInfo(2, info);
      return;
    case commandEditCopyAsImage4x:
      attachScaledMenuInfo(4, info);
      return;
    case commandEditCopyAsImage8x:
      attachScaledMenuInfo(8, info);
      return;
#if defined(JUCE_MAC)
    case commandEditCopyAsPdf:
      info.setInfo(TRANS("Copy as PDF"), {}, {}, 0);
      info.setActive((bool)fContent);
      return;
#endif
    case commandUpdateMenuModel:
      info.setInfo("Update menu model", {}, {}, juce::ApplicationCommandInfo::hiddenFromKeyEditor);
      return;
    case commandViewTogglePreviewVisibility:
      info.setInfo(TRANS("Preview"), {}, {}, 0);
      info.setTicked(fHieroglyph->isVisible());
      return;
    case commandHelpAbout:
      info.setInfo(TRANS("About"), {}, {}, 0);
      return;
    case commandHelpExample:
      info.setInfo(TRANS("Example"), {}, {}, 0);
      return;
    default:
      return;
    }
  }

  void attachScaledMenuInfo(int scale, juce::ApplicationCommandInfo &info) {
    juce::String leading;
    switch (scale) {
    case 1:
      leading = TRANS("1x scale");
      break;
    case 2:
      leading = TRANS("2x scale");
      break;
    case 4:
      leading = TRANS("4x scale");
      break;
    case 8:
      leading = TRANS("8x scale");
      break;
    default:
      assert(false);
      return;
    }
    if (fContent) {
      auto [w, h] = getPngSize(scale);
      info.setInfo(leading + juce::String(" (") + juce::String(w) + juce::String(" x ") + juce::String(h) + juce::String(")"), {}, {}, 0);
      info.setActive(true);
    } else {
      info.setInfo(leading, {}, {}, 0);
      info.setActive(false);
    }
  }

  bool perform(juce::ApplicationCommandTarget::InvocationInfo const &info) override {
    defer {
      updateMenuModel();
    };
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
    case commandEditCopyAsUnicodeWithoutFormatControl:
      copyWithoutFormatControl();
      return true;
    case commandEditCopyAsUnicodeWithFormatControl:
      copyWithFormatControl();
      return true;
#if defined(JUCE_WINDOWS)
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
    case commandEditSwitchFocus:
      switch (fFocusOwner) {
      case FocusOwner::textEditor:
        setFocusOwner(FocusOwner::signList);
        break;
      case FocusOwner::signList:
        setFocusOwner(FocusOwner::textEditor);
        break;
      }
      return true;
    case commandEditSignListUseSign:
      fSignList->useSelectedSign();
      return true;
    case commandEditSignListMoveLeft:
      fSignList->moveSignSelection(-1, 0);
      return true;
    case commandEditSignListMoveUp:
      fSignList->moveSignSelection(0, -1);
      return true;
    case commandEditSignListMoveRight:
      fSignList->moveSignSelection(1, 0);
      return true;
    case commandEditSignListMoveDown:
      fSignList->moveSignSelection(0, 1);
      return true;
    case commandEditSignListCategoryNext:
      fSignList->moveCategory(1);
      return true;
    case commandEditSignListCategoryPrev:
      fSignList->moveCategory(-1);
      return true;
    case commandEditSignListCategoryA:
    case commandEditSignListCategoryB:
    case commandEditSignListCategoryC:
    case commandEditSignListCategoryD:
    case commandEditSignListCategoryE:
    case commandEditSignListCategoryF:
    case commandEditSignListCategoryG:
    case commandEditSignListCategoryH:
    case commandEditSignListCategoryI:
    case commandEditSignListCategoryK:
    case commandEditSignListCategoryL:
    case commandEditSignListCategoryM:
    case commandEditSignListCategoryN:
    case commandEditSignListCategoryO:
    case commandEditSignListCategoryP:
    case commandEditSignListCategoryQ:
    case commandEditSignListCategoryR:
    case commandEditSignListCategoryS:
    case commandEditSignListCategoryT:
    case commandEditSignListCategoryU:
    case commandEditSignListCategoryV:
    case commandEditSignListCategoryW:
    case commandEditSignListCategoryX:
    case commandEditSignListCategoryY:
    case commandEditSignListCategoryZ:
    case commandEditSignListCategoryAa:
    case commandEditSignListCategoryTall:
    case commandEditSignListCategoryWide:
    case commandEditSignListCategorySmall: {
      int index = info.commandID - commandEditSignListCategoryA + 1;
      fSignList->setActiveCategory(index);
      return true;
    }
    case commandEditCopyAsImage1x:
    case commandEditCopyAsImage2x:
    case commandEditCopyAsImage4x:
    case commandEditCopyAsImage8x: {
      float scale = powf(2, info.commandID - commandEditCopyAsImage1x);
      copyAsPng(scale);
      return true;
    }
#if defined(JUCE_MAC)
    case commandEditCopyAsPdf:
      copyAsPdf();
      return true;
#endif
    case commandUpdateMenuModel:
      return true;
    case commandHelpAbout:
      showAboutComponent();
      return true;
    case commandHelpExample:
      showExampleComponent();
      return true;
    case commandViewTogglePreviewVisibility: {
      bool visible = !fHieroglyph->isVisible();
      fHieroglyph->setVisible(visible);
      fAppSetting->setPreviewEnabled(visible);
      return true;
    }
    }
    return false;
  }

  void lookAndFeelChanged() override {
    updateOverlayColors();
  }

private:
  void setFocusOwner(FocusOwner next) {
    if (next == fFocusOwner) {
      return;
    }
    fFocusOwner = next;
    switch (fFocusOwner) {
    case FocusOwner::textEditor:
      fSignList->resetSignSelection();
      fSignList->giveAwayKeyboardFocus();
      fTextEditor->focus();
      break;
    case FocusOwner::signList:
      fTextEditor->blur();
      fSignList->grabKeyboardFocus();
      fSignList->moveSignSelection(0, 0);
      break;
    }
    updateOverlayColors();
  }

  void updateOverlayColors() {
    auto textColor = getLookAndFeel().findColour(juce::TextEditor::textColourId);
    auto color = textColor.withAlpha(0.03f);
    switch (fFocusOwner) {
    case FocusOwner::textEditor:
      fVerticalSplitter->setOverlayColor(juce::Colours::transparentBlack);
      fSignList->setOverlayColor(color);
      break;
    case FocusOwner::signList:
      fVerticalSplitter->setOverlayColor(color);
      fSignList->setOverlayColor(juce::Colours::transparentBlack);
      break;
    }
    repaint();
  }

  void copyWithoutFormatControl() {
    if (!fContent) {
      return;
    }
    juce::String s;
    for (auto const &line : fContent->lines) {
      std::u32string l;
      for (char32_t ch : line->result) {
        if (0x13430 <= ch && ch <= 0x13455) {
          continue;
        }
        l += ch;
      }
      if (s.isNotEmpty()) {
        s += "\n";
      }
      s += JuceStringFromU32String(l);
    }
    juce::SystemClipboard::copyTextToClipboard(s);
  }

  void copyWithFormatControl() {
    if (!fContent) {
      return;
    }
    juce::String s;
    for (auto const &line : fContent->lines) {
      if (s.isNotEmpty()) {
        s += "\n";
      }
      s += JuceStringFromU32String(line->result);
    }
    juce::SystemClipboard::copyTextToClipboard(s);
  }

  void showExampleComponent() {
    if (fAbout || fExample) {
      return;
    }
    auto prevFocus = fFocusOwner;
    fTextEditor->blur();
    fExample = std::make_unique<ExampleComponent>(fFont, fAppSetting);
    fExample->setBounds(getLocalBounds());
    fExample->onClickClose = [prevFocus, this]() {
      hideExampleComponent();
      *fNumModalComponents -= 1;
      setFocusOwner(prevFocus);
    };
    *fNumModalComponents += 1;
    addAndMakeVisible(*fExample);
  }

  void hideExampleComponent() {
    removeChildComponent(fExample.get());
    fExample.reset();
    fTextEditor->focus();
  }

  void showAboutComponent() {
    if (fAbout || fExample) {
      return;
    }
    fTextEditor->blur();
    fAbout = std::make_unique<AboutComponent>();
    fAbout->setBounds(getLocalBounds());
    fAbout->onClickClose = [this]() {
      hideAboutComponent();
    };
    addAndMakeVisible(*fAbout);
  }

  void hideAboutComponent() {
    removeChildComponent(fAbout.get());
    fAbout.reset();
    fTextEditor->focus();
  }

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
    showAsync(options, [this, then](int result) {
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
    launchFileChooser(*fOpenFileChooser, juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [this](juce::FileChooser const &chooser) {
      auto file = chooser.getResult();
      if (file == juce::File()) {
        return;
      }
      open(file);
      addToRecentFile(file);
    });
  }

  void addToRecentFile(juce::File const &file) {
    fAppSetting->addToRecentFile(file);
    updateMenuModel();
  }

  void updateMenuModel() {
    fMenuModel->menuItemsChanged();
  }

  void showMessageBoxAsync(juce::MessageBoxIconType iconType, juce::String const &title, juce::String const &message, juce::Component *associatedComponent = nullptr) {
    *fNumModalComponents += 1;
    juce::NativeMessageBox::showMessageBoxAsync(iconType, title, message, associatedComponent, &fNativeMessageBoxCoroner);
  }

  void showAsync(juce::MessageBoxOptions const &options, std::function<void(int)> callback) {
    *fNumModalComponents += 1;
    juce::NativeMessageBox::showAsync(options, [this, callback](int result) {
      callback(result);
      *fNumModalComponents -= 1;
    });
  }

  void launchFileChooser(juce::FileChooser &chooser, int flags, std::function<void(juce::FileChooser const &)> callback) {
    *fNumModalComponents += 1;
    chooser.launchAsync(flags, [this, callback](juce::FileChooser const &ch) {
      callback(ch);
      *fNumModalComponents -= 1;
    });
  }

  void open(juce::File const &file) {
    auto stream = file.createInputStream();
    if (!stream || stream->failedToOpen()) {
      showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, TRANS("Error"), TRANS("Failed to open"));
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
  }

  void saveDocumentWithNewName(std::function<void()> then = nullptr) {
    if (!fContent) {
      return;
    }
    if (!fSaveFileChooser) {
      fSaveFileChooser = std::make_unique<juce::FileChooser>(TRANS("Save"), juce::File(), "*.txt");
    }
    launchFileChooser(*fSaveFileChooser, juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting, [this, then](juce::FileChooser const &chooser) {
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
        addToRecentFile(file);
        if (then) {
          waitModalThen(then);
        }
      } else {
        showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, TRANS("Error"), TRANS("Failed to save"));
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
        showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, TRANS("Error"), TRANS("Failed to save"));
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

  std::pair<int, int> getPngSize(float scale) {
    auto [widthf, heightf] = fContent->getSize(fAppSetting->getPresentationSetting());
    int width = (int)ceil(widthf * scale);
    int height = (int)ceil(heightf * scale);
    return std::make_pair(width, height);
  }

  void exportAsPng(float scale) {
    if (!fExportPngFileChooser) {
      fExportPngFileChooser = std::make_unique<juce::FileChooser>(TRANS("Export as PNG"), juce::File(), "*.png");
    }
    launchFileChooser(*fExportPngFileChooser, juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting, [this, scale](juce::FileChooser const &chooser) {
      auto file = chooser.getResult();
      if (file == juce::File() || !fContent) {
        return;
      }
      auto [width, height] = getPngSize(scale);
      juce::Image img(juce::Image::PixelFormat::ARGB, width, height, true);
      {
        juce::Graphics g(img);
        g.addTransform(juce::AffineTransform::scale(scale, scale));
        fContent->draw(g, fAppSetting->getPresentationSetting());
      }
      auto stream = file.createOutputStream();
      auto title = TRANS("Error");
      auto message = TRANS("Failed to export as PNG");
      if (!stream || stream->failedToOpen()) {
        showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (!stream->setPosition(0)) {
        showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (stream->truncate().failed()) {
        showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      juce::PNGImageFormat format;
      if (!format.writeImageToStream(img, *stream)) {
        showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
    });
  }

  void exportAsPdf() {
    if (!fExportPdfFileChooser) {
      fExportPdfFileChooser = std::make_unique<juce::FileChooser>(TRANS("Export as PDF"), juce::File(), "*.pdf");
    }
    launchFileChooser(*fExportPdfFileChooser, juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting, [this](juce::FileChooser const &chooser) {
      auto file = chooser.getResult();
      if (file == juce::File() || !fContent) {
        return;
      }
      auto str = fContent->toPDF(fAppSetting->getPresentationSetting());
      auto stream = file.createOutputStream();
      auto title = TRANS("Error");
      auto message = TRANS("Failed to export as PDF");
      if (str.empty() || !stream || stream->failedToOpen()) {
        showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (!stream->setPosition(0)) {
        showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (stream->truncate().failed()) {
        showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (!stream->write(str.c_str(), str.size())) {
        showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
    });
  }

#if defined(JUCE_MAC)
  void copyAsPdf() {
    if (!fContent) {
      return;
    }
    auto str = fContent->toPDF(fAppSetting->getPresentationSetting());
    Clipboard::Store(str, Clipboard::Type::Pdf);
  }
#endif

  void exportAsEmf() {
    if (!fExportEmfFileChooser) {
      fExportEmfFileChooser = std::make_unique<juce::FileChooser>(TRANS("Export as EMF"), juce::File(), "*.emf");
    }
    launchFileChooser(*fExportEmfFileChooser, juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting, [this](juce::FileChooser const &chooser) {
      auto file = chooser.getResult();
      if (file == juce::File() || !fContent) {
        return;
      }
      auto str = fContent->toEMF(fAppSetting->getPresentationSetting());
      auto stream = file.createOutputStream();
      auto title = TRANS("Error");
      auto message = TRANS("Failed to export as EMF");
      if (str.empty() || !stream || stream->failedToOpen()) {
        showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (!stream->setPosition(0)) {
        showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (stream->truncate().failed()) {
        showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
      if (!stream->write(str.c_str(), str.size())) {
        showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
        return;
      }
    });
  }

#if defined(JUCE_WINDOWS)
  void copyAsEmf() {
    if (!fContent) {
      return;
    }
    auto str = fContent->toEMF(fAppSetting->getPresentationSetting());
    writeToClipboard(str, Clipboard::Type::Emf);
  }
#endif

  void copyAsPng(float scale) {
    auto [widthf, heightf] = fContent->getSize(fAppSetting->getPresentationSetting());
    int width = (int)ceil(widthf * scale);
    int height = (int)ceil(heightf * scale);
    juce::Image img(juce::Image::PixelFormat::ARGB, width, height, true);
    {
      juce::Graphics g(img);
      g.fillAll(juce::Colours::white);
      g.addTransform(juce::AffineTransform::scale(scale, scale));
      fContent->draw(g, fAppSetting->getPresentationSetting());
    }
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
      showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message);
    }
  }

  void hieroglyphDidChangeSelectedRange(int start, int end, Direction direction) {
    fTextEditor->setSelectedRange(start, end, direction);
  }

  void textEditorComponentDidChangeCaretPosition(juce::String const &typing, int start, int end, Direction direction) override {
    fSignList->setTyping(typing);
    fHieroglyph->setSelectedRange(start, end, direction);
  }

  void textEditorComponentDidChangeContent(std::shared_ptr<Content> content, std::optional<juce::String> typing, int start, int end, Direction direction) override {
    setContent(content);
    if (typing && fFocusOwner == FocusOwner::textEditor) {
      fSignList->setTyping(*typing);
    }
    fHieroglyph->setSelectedRange(start, end, direction);
  }

  void textEditorComponentDidGainFocus() override {
    setFocusOwner(FocusOwner::textEditor);
  }

  void textEditorComponentDidLostFocus() override {
    if (fFocusOwner == FocusOwner::textEditor) {
      if (*fNumModalComponents == 0) {
        fTextEditor->focus();
      }
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
    fTextEditor->onClickSign(sign.name);
  }

public:
  std::function<void(juce::File, bool modified)> onSaveFilePathChanged;

private:
  std::unique_ptr<SplitterComponent> fVerticalSplitter;
  std::unique_ptr<SplitterComponent> fHorizontalSplitter;
  std::unique_ptr<TextEditorComponent> fTextEditor;
  std::unique_ptr<HieroglyphComponent> fHieroglyph;
  std::unique_ptr<SignListComponent> fSignList;
  std::unique_ptr<BottomToolBar> fBottomToolBar;
  std::shared_ptr<hb_font_t> fFont;
  std::unique_ptr<MenuBarModel> fMenuModel;
#if !defined(JUCE_MAC)
  std::unique_ptr<juce::MenuBarComponent> fMenuComponent;
#endif
  std::unique_ptr<juce::FileChooser> fExportPdfFileChooser;
  std::shared_ptr<Content> fContent;
  std::unique_ptr<juce::FileChooser> fExportPngFileChooser;
  std::unique_ptr<juce::FileChooser> fSaveFileChooser;
  juce::File fSave;
  bool fDirty = false;
  std::unique_ptr<juce::FileChooser> fOpenFileChooser;
  std::unique_ptr<juce::FileChooser> fExportEmfFileChooser;
  std::shared_ptr<AppSetting> fAppSetting;
  std::unique_ptr<AboutComponent> fAbout;
  std::unique_ptr<ExampleComponent> fExample;
  FocusOwner fFocusOwner = FocusOwner::textEditor;
  std::shared_ptr<int> fNumModalComponents;
  ModalFinishDetector fNativeMessageBoxCoroner;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

} // namespace ksesh
