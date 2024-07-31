#pragma once

namespace ksesh {

class TextEditorComponent : public juce::Component {
public:
  TextEditorComponent() {
    fEditor = std::make_unique<juce::TextEditor>();
    fEditor->setMultiLine(true);
    fEditor->setFont(juce::Font(juce::FontOptions(24)));
    fEditor->setReturnKeyStartsNewLine(true);
    fEditor->onTextChange = [this]() {
      this->_onTextChange();
    };
    fEditor->onCaretPositionChange = [this]() {
      this->_onCaretPositionChange();
    };
    addAndMakeVisible(fEditor.get());
  }

  void resized() override {
    fEditor->setBounds(0, 0, getWidth(), getHeight());
  }

  void lookAndFeelChanged() override {
    fEditor->applyColourToAllText(getLookAndFeel().findColour(juce::TextEditor::textColourId));
  }

  static juce::String GetTypingAtCaret(juce::String const &text, int caret) {
    if (caret < 1) {
      return "";
    }
    auto leading = U32StringFromJuceString(text.substring(0, caret));
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
    int caret = fEditor->getCaretPosition();
    auto typing = GetTypingAtCaret(text, caret);
    auto selected = fEditor->getHighlightedRegion();
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

private:
  void _onCaretPositionChange() {
    if (onCaretPositionChange) {
      onCaretPositionChange(fEditor->getText(), fEditor->getCaretPosition());
    }
  }

  void _onTextChange() {
    if (!onTextChange) {
      return;
    }
    auto text = fEditor->getText();
    auto caret = fEditor->getCaretPosition();
    onTextChange(text, caret);
  }

public:
  std::function<void(juce::String const &, int)> onTextChange;
  std::function<void(juce::String const &, int)> onCaretPositionChange;

private:
  std::unique_ptr<juce::TextEditor> fEditor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextEditorComponent)
};

} // namespace ksesh
