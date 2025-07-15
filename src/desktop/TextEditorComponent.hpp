#pragma once

namespace ksesh {

class TextEditorComponent : public juce::Component, public juce::ChangeListener {
  enum : int {
    defaultIndent = 4,
    clipExpand = 1,
  };

  class Editor : public TextEditor, public juce::ChangeListener {
    using Super = TextEditor;

  public:
    explicit Editor(std::shared_ptr<AppSetting> const &setting) : fSetting(setting) {
      setting->addChangeListener(this);
      (void)NEEDS_TRANS("Cut");
      (void)NEEDS_TRANS("Copy");
      (void)NEEDS_TRANS("Paste");
      (void)NEEDS_TRANS("Delete");
      (void)NEEDS_TRANS("Select All");
      (void)NEEDS_TRANS("Undo");
      (void)NEEDS_TRANS("Redo");
    }

    ~Editor() {
      fSetting->removeChangeListener(this);
    }

    void paintOverChildren(juce::Graphics &g) override {
      PresentationSetting setting = getRenderSetting();
      auto borderColor = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
      float lineHeight = setting.fFontSize + setting.lineSpacing();
      float dy = fSetting->getPresentationSetting().lineSpacing() * 0.5f;
      float textWidth = getTextWidth();
      int lines = (int)ceil(std::max<int>(getHeight(), getTextHeight()) / lineHeight);
      auto vp = getViewPosition();
      float scrollbarWidth = getLookAndFeel().getDefaultScrollbarWidth();
      float vScrollbarWidth = isVerticalScrollBarShown() ? scrollbarWidth + 2 : 0;
      float hScrollbarHeight = isHorizontalScrollBarShown() ? scrollbarWidth : 0;
      auto bounds = getLocalBounds();

      juce::Rectangle<int> clip(
          clipExpand,
          clipExpand,
          bounds.getWidth() - vScrollbarWidth - 2 * clipExpand,
          bounds.getHeight() - hScrollbarHeight - 2 * clipExpand);
      g.reduceClipRegion(clip);

      g.setColour(borderColor);
      for (int i = 0; i < lines; i++) {
        float y = setting.fPadding + (i + 1) * lineHeight;
        g.drawHorizontalLine(y - vp.getY() - dy, defaultIndent - vp.getX(), textWidth - defaultIndent - vp.getX());
      }

      auto document = fDocument.lock();
      if (!document) {
        return;
      }
      auto textColor = getLookAndFeel().findColour(juce::TextEditor::ColourIds::textColourId);
      auto highlightTextColor = getLookAndFeel().findColour(juce::TextEditor::ColourIds::highlightedTextColourId);
      auto caretColor = getLookAndFeel().findColour(juce::CaretComponent::caretColourId);
      auto highlightColor = getLookAndFeel().findColour(juce::TextEditor::highlightColourId);

      float constexpr caretWidth = 2;
      g.addTransform(juce::AffineTransform::translation(-vp.getX(), -vp.getY()));
      document->draw(
          g,
          fSelectedRange.getStart(),
          fSelectedRange.getEnd(),
          fDirection,
          setting,
          caretWidth,
          textColor,
          highlightTextColor,
          caretColor,
          highlightColor,
          getMaxWidth());
    }

    void setDocument(std::shared_ptr<Document> const &d) {
      fDocument = d;
      repaint();
    }

    void changeListenerCallback(juce::ChangeBroadcaster *source) override {
      if (source != fSetting.get()) {
        return;
      }
    }

    void focusGained(FocusChangeType) override {
      if (fOnFocusGained) {
        fOnFocusGained();
      }
    }

    void setSelectedRange(juce::Range<int> range, Direction direction) {
      fSelectedRange = range;
      fDirection = direction;
      repaint();
    }

    juce::RectangleList<int> getTextBounds(juce::Range<int> textRange) const override {
      juce::RectangleList<int> ret;
      for (auto const &range : Super::getTextBounds(textRange)) {
        ret.addWithoutMerging(range.withHeight(fSetting->getEditorFontSize()));
      }
      return ret;
    }

    int indexAtPosition(float x, float y) const override {
      auto ret = Super::indexAtPosition(x, y);
      auto document = fDocument.lock();
      if (!document) {
        return ret;
      }
      if (document->fLines.empty()) {
        return ret;
      }
      auto font = document->fFont.lock();
      if (!font) {
        return ret;
      }
      auto p = fSetting->getPresentationSetting();
      float tx = x;
      float ty = y + p.fFontSize;
      float lineHeight = p.fFontSize + p.lineSpacing() + fSetting->getEditorFontSize();
      int lineIndex = std::clamp<int>((int)floor(ty / lineHeight), 0, (int)document->fLines.size() - 1);
      float offset = ty - lineIndex * lineHeight;
      if (0 <= offset && offset <= p.fFontSize) {
        auto setting = getRenderSetting();
        auto maxWidth = getMaxWidth();
        float const fontSize = setting.fFontSize;
        auto line = document->fLines[lineIndex];
        float xMax = line->fWidth * font->fScale * fontSize;
        float drawScale = 1;
        if (xMax > maxWidth) {
          drawScale = maxWidth / xMax;
        }
        auto pos = document->closestPosition(getCaretPosition(), {tx / drawScale, ty}, setting);
        return pos.fLocation;
      } else {
        return ret;
      }
    }

  private:
    float getMaxWidth() const {
      float scrollbarWidth = getLookAndFeel().getDefaultScrollbarWidth();
      return getTextWidth() - scrollbarWidth;
    }

    PresentationSetting getRenderSetting() const {
      PresentationSetting base = fSetting->getPresentationSetting();
      PresentationSetting setting;
      setting.fPadding = defaultIndent;
      setting.fFontSize = base.fFontSize;
      setting.fLineSpacingRatio = (fSetting->getEditorFontSize() + base.lineSpacing()) / base.fFontSize;
      return setting;
    }

  public:
    std::function<void()> fOnFocusGained;
    std::function<void()> fOnEscape;

  private:
    std::shared_ptr<AppSetting> fSetting;
    std::weak_ptr<Document> fDocument;
    juce::Range<int> fSelectedRange;
    Direction fDirection = Direction::Forward;
  };

public:
  struct Delegate {
    virtual ~Delegate() {};
    virtual void textEditorComponentDidChangeDocument(std::shared_ptr<Document> const &document, std::optional<juce::String> typing, int start, int end, Direction) = 0;
    virtual void textEditorComponentDidChangeCaretPosition(juce::String const &typing, int start, int end, Direction) = 0;
    virtual void textEditorComponentDidGainFocus() = 0;
    virtual void textEditorComponentDidLostFocus() = 0;
  };

  TextEditorComponent(std::shared_ptr<FontAdapter> const &font, std::shared_ptr<AppSetting> const &setting) : fSetting(setting), fFont(font) {
    fDocument = std::make_shared<Document>(U"", font);
    fEditor = std::make_unique<Editor>(setting);
    fEditor->setMultiLine(true, false);
    fEditor->setReturnKeyStartsNewLine(true);
    bind();
    addAndMakeVisible(fEditor.get());
    applySetting();
    setting->addChangeListener(this);
  }

  ~TextEditorComponent() {
    fSetting->removeChangeListener(this);
  }

  void resized() override {
    fEditor->setBounds(0, 0, getWidth(), getHeight());
  }

  void lookAndFeelChanged() override {
    fEditor->applyColourToAllText(getLookAndFeel().findColour(juce::TextEditor::textColourId));
  }

  void onClickSign(juce::String const &s) {
    unbind();
    auto text = fEditor->getText();
    auto selected = getSelectedRange();
    int caret = fEditor->getCaretPosition();
    auto typing = GetTypingAtCaret(text, selected.getStart(), selected.getEnd());
    if (typing.isNotEmpty() && selected.isEmpty() && s.startsWith(typing)) {
      juce::String next;
      int nextCaret = caret - typing.length() + s.length() + 1;
      auto leading = text.substring(0, caret - typing.length());
      if (text.substring(caret - typing.length(), caret - typing.length() + s.length()) == s) {
        auto trailing = text.substring(caret - typing.length() + s.length());
        if (trailing.startsWith(" ")) {
          next = leading + s + trailing;
        } else {
          next = leading + s + " " + trailing;
        }
      } else {
        auto trailing = text.substring(caret);
        if (trailing.startsWith(" ")) {
          next = leading + s + trailing;
        } else {
          next = leading + s + " " + trailing;
        }
      }
      if (caret < nextCaret) {
        fDirection = Direction::Forward;
      } else if (nextCaret < caret) {
        fDirection = Direction::Backward;
      }
      auto document = std::make_shared<Document>(U32StringFromJuceString(next), fFont);
      fEditor->setDocument(document);
      fEditor->setText(next, false);
      fEditor->setCaretPosition(nextCaret);
      fEditor->setSelectedRange(juce::Range<int>(nextCaret, nextCaret), fDirection);
      if (fDelegate) {
        fDelegate->textEditorComponentDidChangeDocument(document, std::nullopt, nextCaret, nextCaret, fDirection);
      }
      fDocument = document;
      fDirty = true;
      bind();
    } else {
      auto str = s;
      if (selected.getEnd() >= text.length()) {
        str = s + " ";
      } else if (selected.getEnd() < text.length() && text.substring(selected.getEnd(), selected.getEnd() + 1) != " ") {
        str = s + " ";
      }
      auto next = text.substring(0, selected.getStart()) + str + text.substring(selected.getEnd());
      auto nextCaret = selected.getStart() + str.length();
      if (caret < nextCaret) {
        fDirection = Direction::Forward;
      } else if (nextCaret < caret) {
        fDirection = Direction::Backward;
      }
      auto document = std::make_shared<Document>(U32StringFromJuceString(next), fFont);
      fEditor->setDocument(document);
      fEditor->setText(next, false);
      fEditor->setCaretPosition(nextCaret);
      fEditor->setSelectedRange(juce::Range<int>(nextCaret, nextCaret), fDirection);
      if (fDelegate) {
        fDelegate->textEditorComponentDidChangeDocument(document, std::nullopt, nextCaret, nextCaret, fDirection);
      }
      fDocument = document;
      fDirty = true;
      bind();
    }
  }

  void focus() {
    fEditor->grabKeyboardFocus();
  }

  void blur() {
    fEditor->giveAwayKeyboardFocus();
  }

  void setSelectedRange(int start, int end, Direction direction) {
    fEditor->setHighlightedRegion(juce::Range<int>(start, end));
  }

  std::optional<juce::String> getSelectedText() {
    juce::Range<int> range = fEditor->getHighlightedRegion();
    if (range.isEmpty()) {
      return std::nullopt;
    }
    juce::String all = fEditor->getText();
    return all.substring(range.getStart(), range.getEnd());
  }

  void resetText(juce::String const &s) {
    unbind();
    fEditor->setText(s);
    auto document = std::make_shared<Document>(U32StringFromJuceString(s), fFont);
    auto nextCaret = s.length();
    fEditor->setDocument(document);
    fEditor->setHighlightedRegion(juce::Range<int>(nextCaret, nextCaret));
    fEditor->setCaretPosition(nextCaret);
    if (fDelegate) {
      fDelegate->textEditorComponentDidChangeDocument(document, std::nullopt, nextCaret, nextCaret, fDirection);
    }
    fDocument = document;
    fDirty = false;
    bind();
  }

  void changeListenerCallback(juce::ChangeBroadcaster *source) override {
    if (source != fSetting.get()) {
      return;
    }
    applySetting();
  }

  void changeFont(std::shared_ptr<FontAdapter> const &font) {
    auto document = std::make_shared<Document>(fDocument->fRaw, font);
    fEditor->setDocument(document);
    fDocument = document;
    fFont = font;
    if (fDelegate) {
      auto range = getSelectedRange();
      fDelegate->textEditorComponentDidChangeDocument(document, std::nullopt, range.getStart(), range.getEnd(), fDirection);
    }
  }

  bool isDirty() const {
    return fDirty;
  }

  void clearDirty() {
    fDirty = false;
  }

private:
  void applySetting() {
    auto presentation = fSetting->getPresentationSetting();
    float editorFontSize = fSetting->getEditorFontSize();
    auto fontSize = presentation.fFontSize;
    fEditor->setIndents(defaultIndent, fontSize + defaultIndent);
    fEditor->setLineSpacing((editorFontSize + fontSize + presentation.lineSpacing()) / editorFontSize);
    fEditor->setFont(juce::Font(juce::FontOptions(editorFontSize)));
  }

  static juce::String GetTypingAtCaret(juce::String const &text, int start, int end) {
    if (start < 1 || text.isEmpty()) {
      return "";
    }
    auto leading = U32StringFromJuceString(text.substring(0, start));
    auto first = leading.back();
    int offset = (int)leading.size();
    if (U'0' <= first && first <= U'9') {
      for (int i = offset - 1; i >= 0; i--) {
        auto ch = leading[i];
        offset = i;
        if (ch < U'0' || U'9' < ch) {
          break;
        }
      }
    }
    for (int i = offset - 1; i >= 0; i--) {
      auto ch = leading[i];
      if ((U'A' <= ch && ch <= U'Z') || (U'a' <= ch && ch <= U'z')) {
        offset = i;
      } else {
        break;
      }
    }
    auto typing = leading.substr(offset);
    return JuceStringFromU32String(typing);
  }

  void bind() {
    fEditor->onTextChange = [this]() {
      _onTextChange();
    };
    fEditor->onCaretPositionChange = [this]() {
      _onCaretPositionChange();
    };
    fEditor->onSelectionChange = [this]() {
      _onCaretPositionChange();
    };
    fEditor->fOnFocusGained = [this]() {
      if (fDelegate) {
        fDelegate->textEditorComponentDidGainFocus();
      }
    };
    fEditor->onFocusLost = [this]() {
      if (fDelegate) {
        fDelegate->textEditorComponentDidLostFocus();
      }
    };
    fEditor->onEscapeKey = [this]() {
      if (fOnEscapeKey) {
        fOnEscapeKey();
      }
    };
  }

  void unbind() {
    fEditor->onTextChange = nullptr;
    fEditor->onCaretPositionChange = nullptr;
    fEditor->onSelectionChange = nullptr;
    fEditor->fOnFocusGained = nullptr;
    fEditor->onFocusLost = nullptr;
  }

  juce::Range<int> getSelectedRange() const {
    auto range = fEditor->getHighlightedRegion();
    auto caret = fEditor->getCaretPosition();
    if (range.getLength() == 0) {
      return juce::Range<int>(caret, caret);
    } else {
      return range;
    }
  }

  void _onCaretPositionChange() {
    auto pos = fEditor->getCaretPosition();
    if (pos < fPrev) {
      fDirection = Direction::Backward;
    } else if (fPrev < pos) {
      fDirection = Direction::Forward;
    }
    fPrev = pos;
    auto range = getSelectedRange();
    if (fDelegate) {
      auto typing = GetTypingAtCaret(fEditor->getText(), range.getStart(), range.getEnd());
      fDelegate->textEditorComponentDidChangeCaretPosition(typing, range.getStart(), range.getEnd(), fDirection);
    }
    fEditor->setSelectedRange(range, fDirection);
  }

  void _onTextChange() {
    auto text = fEditor->getText();
    auto range = getSelectedRange();
    auto document = std::make_shared<Document>(U32StringFromJuceString(text), fFont);
    auto typing = GetTypingAtCaret(text, range.getStart(), range.getEnd());
    fDirty = true;
    fEditor->setDocument(document);
    fEditor->setSelectedRange(range, fDirection);
    if (fDelegate) {
      fDelegate->textEditorComponentDidChangeDocument(document, typing, range.getStart(), range.getEnd(), fDirection);
    }
    fDocument = document;
  }

public:
  Delegate *fDelegate = nullptr;
  std::function<void()> fOnEscapeKey;

private:
  std::unique_ptr<Editor> fEditor;
  int fPrev = 0;
  Direction fDirection = Direction::Forward;
  std::shared_ptr<AppSetting> fSetting;
  std::shared_ptr<FontAdapter> fFont;
  std::shared_ptr<Document> fDocument;
  bool fDirty = false;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextEditorComponent)
};

} // namespace ksesh
