#pragma once

namespace ksesh {

class MainComponent : public juce::Component {
  enum {
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

    setSize(width, height);
  }

  void resized() override {
    int const width = getWidth();
    int const height = getHeight();
    fHorizontalSplitter->setBounds(0, 0, width, height);
  }

private:
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
    auto c = std::make_shared<Content>(str, fFont);
    fHieroglyph->setContent(c);
    if (!fIgnoreCaretChange) {
      auto typing = TextEditorComponent::GetTypingAtCaret(text, start, end);
      fSignList->setTyping(typing);
      fHieroglyph->setSelectedRange(start, end, direction);
    }
  }

  void onClickSign(Sign const &sign) {
    fIgnoreCaretChange = true;
    auto result = fTextEditor->insert(sign.name);
    fTextEditor->focus();
    auto str = U32StringFromJuceString(result);
    auto c = std::make_shared<Content>(str, fFont);
    fHieroglyph->setContent(c);

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

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

} // namespace ksesh
