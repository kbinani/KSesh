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

  void insert(juce::String const &s) {
    auto text = fEditor->getText();
    auto selected = fEditor->getHighlightedRegion();
    auto str = s;
    if (selected.getEnd() >= text.length()) {
      str = s + " ";
    } else if (selected.getEnd() < text.length() && text.substring(selected.getEnd(), selected.getEnd() + 1) != " ") {
      str = s + " ";
    }
    fEditor->insertTextAtCaret(str);
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
