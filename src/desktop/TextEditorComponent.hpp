#pragma once

namespace ksesh {

class TextEditorComponent : public juce::Component {
  class TextEditor : public juce::TextEditor {
  public:
    void focusLost(juce::Component::FocusChangeType reason) override {
      if (reason == juce::Component::focusChangedByTabKey) {
        grabKeyboardFocus();
      }
    }
  };

public:
  TextEditorComponent() {
    fEditor = std::make_unique<TextEditor>();
    fEditor->setMultiLine(true);
    fEditor->setFont(juce::Font(juce::FontOptions(24)));
    fEditor->setReturnKeyStartsNewLine(true);
    bind();
    addAndMakeVisible(fEditor.get());
  }

  void resized() override {
    fEditor->setBounds(0, 0, getWidth(), getHeight());
  }

  void lookAndFeelChanged() override {
    fEditor->applyColourToAllText(getLookAndFeel().findColour(juce::TextEditor::textColourId));
  }

  static juce::String GetTypingAtCaret(juce::String const &text, int start, int end) {
    if (start < 1) {
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
    fEditor->setHighlightedRegion(juce::Range<int>(s.length(), s.length()));
    fEditor->setCaretPosition(s.length());
    bind();
  }

private:
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
    if (onCaretPositionChange) {
      auto range = getSelectedRange();
      onCaretPositionChange(fEditor->getText(), range.getStart(), range.getEnd(), fDirection);
    }
  }

  void _onTextChange() {
    if (!onTextChange) {
      return;
    }
    auto text = fEditor->getText();
    auto range = getSelectedRange();
    onTextChange(text, range.getStart(), range.getEnd(), fDirection);
  }

public:
  std::function<void(juce::String const &, int start, int end, Direction)> onTextChange;
  std::function<void(juce::String const &, int start, int end, Direction)> onCaretPositionChange;

private:
  std::unique_ptr<TextEditor> fEditor;
  int fPrev = 0;
  Direction fDirection = Direction::Forward;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextEditorComponent)
};

} // namespace ksesh
