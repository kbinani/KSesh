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
