#pragma once

namespace ksesh {

class TextEditorComponent : public juce::Component, public juce::ChangeListener {
  enum : int {
    defaultIndent = 4,
    clipExpand = 1,
  };

  class TextEditor : public juce::TextEditor, public juce::ChangeListener {
    using super = juce::TextEditor;

  public:
    explicit TextEditor(std::shared_ptr<AppSetting> const &setting) : fSetting(setting) {
      setting->addChangeListener(this);
      (void)NEEDS_TRANS("Cut");
      (void)NEEDS_TRANS("Copy");
      (void)NEEDS_TRANS("Paste");
      (void)NEEDS_TRANS("Delete");
      (void)NEEDS_TRANS("Select All");
      (void)NEEDS_TRANS("Undo");
      (void)NEEDS_TRANS("Redo");
    }

    ~TextEditor() {
      fSetting->removeChangeListener(this);
    }

    void focusLost(juce::Component::FocusChangeType reason) override {
      if (reason == juce::Component::focusChangedByTabKey) {
        grabKeyboardFocus();
      }
    }

    void paintOverChildren(juce::Graphics &g) override {
      PresentationSetting setting = getRenderSetting();
      auto borderColor = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
      float lineHeight = setting.fontSize + setting.lineSpacing();
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
        float y = setting.padding + (i + 1) * lineHeight;
        g.drawHorizontalLine(y - vp.getY() - dy, defaultIndent - vp.getX(), textWidth - defaultIndent - vp.getX());
      }

      if (!fContent) {
        return;
      }
      auto textColor = getLookAndFeel().findColour(juce::TextEditor::ColourIds::textColourId);
      auto highlightTextColor = getLookAndFeel().findColour(juce::TextEditor::ColourIds::highlightedTextColourId);
      auto caretColor = getLookAndFeel().findColour(juce::CaretComponent::caretColourId);
      auto highlightColor = getLookAndFeel().findColour(juce::TextEditor::highlightColourId);

      float constexpr caretWidth = 2;
      g.addTransform(juce::AffineTransform::translation(-vp.getX(), -vp.getY()));
      fContent->draw(
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

    void setContent(std::shared_ptr<Content> const &c) {
      fContent = c;
      repaint();
    }

    void changeListenerCallback(juce::ChangeBroadcaster *source) override {
      if (source != fSetting.get()) {
        return;
      }
    }

    void setSelectedRange(juce::Range<int> range, Direction direction) {
      fSelectedRange = range;
      fDirection = direction;
      repaint();
    }

    juce::RectangleList<int> getTextBounds(juce::Range<int> textRange) const override {
      juce::RectangleList<int> ret;
      for (auto const &range : juce::TextEditor::getTextBounds(textRange)) {
        ret.addWithoutMerging(range.withHeight(fSetting->getEditorFontSize()));
      }
      return ret;
    }

    int indexAtPosition(float x, float y) const override {
      auto ret = super::indexAtPosition(x, y);
      if (!fContent) {
        return ret;
      }
      if (fContent->lines.empty()) {
        return ret;
      }
      auto p = fSetting->getPresentationSetting();
      float tx = x;
      float ty = y + p.fontSize;
      float lineHeight = p.fontSize + p.lineSpacing() + fSetting->getEditorFontSize();
      int lineIndex = std::clamp<int>((int)floor(ty / lineHeight), 0, (int)fContent->lines.size() - 1);
      float offset = ty - lineIndex * lineHeight;
      if (0 <= offset && offset <= p.fontSize) {
        auto setting = getRenderSetting();
        auto maxWidth = getMaxWidth();
        float const upem = (float)fContent->unitsPerEm;
        float const scale = setting.fontSize / upem;
        auto line = fContent->lines[lineIndex];
        auto bounds = line->boundingBox * scale;
        float drawScale = 1;
        if (bounds.getWidth() > maxWidth) {
          drawScale = maxWidth / bounds.getWidth();
        }
        auto pos = fContent->closestPosition(getCaretPosition(), {tx / drawScale, ty}, setting);
        return pos.location;
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
      setting.padding = defaultIndent;
      setting.fontSize = base.fontSize;
      setting.lineSpacingRatio = (fSetting->getEditorFontSize() + base.lineSpacing()) / base.fontSize;
      return setting;
    }

  private:
    std::shared_ptr<AppSetting> fSetting;
    std::shared_ptr<Content> fContent;
    juce::Range<int> fSelectedRange;
    Direction fDirection = Direction::Forward;
  };

public:
  struct Delegate {
    virtual ~Delegate(){};
    virtual void textEditorComponentDidChangeContent(std::shared_ptr<Content> content, juce::String const &typing, int start, int end, Direction) = 0;
    virtual void textEditorComponentDidChangeCaretPosition(juce::String const &typing, int start, int end, Direction) = 0;
  };

  explicit TextEditorComponent(std::shared_ptr<hb_font_t> const &font, std::shared_ptr<AppSetting> const &setting) : fFont(font), fSetting(setting) {
    fEditor = std::make_unique<TextEditor>(setting);
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

  juce::String insert(juce::String const &s) {
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
      fEditor->setText(next, false);
      fEditor->setCaretPosition(nextCaret);
      return next;
    } else {
      auto str = s;
      if (selected.getEnd() >= text.length()) {
        str = s + " ";
      } else if (selected.getEnd() < text.length() && text.substring(selected.getEnd(), selected.getEnd() + 1) != " ") {
        str = s + " ";
      }
      auto next = text.substring(0, selected.getStart()) + str + text.substring(selected.getEnd());
      fEditor->setText(next, false);
      fEditor->setCaretPosition(selected.getStart() + str.length());
      return next;
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

  void resetText(juce::String const &s) {
    unbind();
    fEditor->setText(s);
    auto c = std::make_shared<Content>(U32StringFromJuceString(s), fFont);
    fEditor->setHighlightedRegion(juce::Range<int>(s.length(), s.length()));
    fEditor->setCaretPosition(s.length());
    fEditor->setContent(c);
    bind();
  }

  void changeListenerCallback(juce::ChangeBroadcaster *source) override {
    if (source != fSetting.get()) {
      return;
    }
    applySetting();
  }

private:
  void applySetting() {
    auto presentation = fSetting->getPresentationSetting();
    float editorFontSize = fSetting->getEditorFontSize();
    auto fontSize = presentation.fontSize;
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
      this->_onTextChange();
    };
    fEditor->onCaretPositionChange = [this]() {
      this->_onCaretPositionChange();
    };
    fEditor->onSelectionChange = [this]() {
      this->_onCaretPositionChange();
    };
  }

  void unbind() {
    fEditor->onTextChange = nullptr;
    fEditor->onCaretPositionChange = nullptr;
    fEditor->onSelectionChange = nullptr;
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
    if (!fDelegate) {
      return;
    }
    auto text = fEditor->getText();
    auto range = getSelectedRange();
    auto c = std::make_shared<Content>(U32StringFromJuceString(text), fFont);
    auto typing = GetTypingAtCaret(text, range.getStart(), range.getEnd());
    fEditor->setContent(c);
    fEditor->setSelectedRange(range, fDirection);
    fDelegate->textEditorComponentDidChangeContent(c, typing, range.getStart(), range.getEnd(), fDirection);
  }

public:
  Delegate *fDelegate = nullptr;

private:
  std::unique_ptr<TextEditor> fEditor;
  int fPrev = 0;
  Direction fDirection = Direction::Forward;
  std::shared_ptr<hb_font_t> fFont;
  std::shared_ptr<AppSetting> fSetting;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextEditorComponent)
};

} // namespace ksesh
