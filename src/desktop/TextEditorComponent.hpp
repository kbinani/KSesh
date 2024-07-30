#pragma once

namespace ksesh {

class TextEditorComponent : public juce::Component {
public:
  class Delegate {
  public:
    virtual ~Delegate() = default;
    virtual void textEditorComponentDidChangeText(juce::String const &text) = 0;
  };

  TextEditorComponent() {
    fEditor = std::make_unique<juce::TextEditor>();
    fEditor->setMultiLine(true);
    fEditor->setFont(juce::Font(juce::FontOptions(24)));
    fEditor->setReturnKeyStartsNewLine(true);
    fEditor->onTextChange = [this]() {
      this->onTextChange();
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
  void onTextChange() {
    if (!fDelegate) {
      return;
    }
    auto text = fEditor->getText();
    fDelegate->textEditorComponentDidChangeText(text);
  }

public:
  Delegate *fDelegate;

private:
  std::unique_ptr<juce::TextEditor> fEditor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextEditorComponent)
};

} // namespace ksesh
