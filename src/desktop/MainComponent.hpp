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
    fTextEditor->onTextChange = [this](juce::String const &text, int caret) {
      this->textEditorDidChangeText(text, caret);
    };
    fTextEditor->onCaretPositionChange = [this](juce::String const &text, int caret) {
      this->textEditorDidChangeCaretPosition(text, caret);
    };

    fHieroglyph = std::make_unique<HieroglyphComponent>(font);
    fHieroglyph->setBounds(width / 2 + resizerSize / 2, 0, width / 2 - resizerSize / 2, height / 2 - resizerSize / 2);

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
  void textEditorDidChangeCaretPosition(juce::String const &text, int caret) {
    auto typing = getTypingAtCaret(text, caret);
    fSignList->setTyping(typing);
  }

  void textEditorDidChangeText(juce::String const &text, int caret) {
    auto str = U32StringFromJuceString(text);
    auto c = std::make_shared<Content>(str, fFont);
    fHieroglyph->setContent(c);
    auto typing = getTypingAtCaret(text, caret);
    fSignList->setTyping(typing);
  }

  juce::String getTypingAtCaret(juce::String const &text, int caret) {
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

  void onClickSign(Sign const &sign) {
    fTextEditor->insert(sign.name);
    fTextEditor->focus();
  }

private:
  std::unique_ptr<SplitterComponent> fVerticalSplitter;
  std::unique_ptr<SplitterComponent> fHorizontalSplitter;
  std::unique_ptr<TextEditorComponent> fTextEditor;
  std::unique_ptr<HieroglyphComponent> fHieroglyph;
  std::unique_ptr<SignListComponent> fSignList;
  HbFontUniquePtr const &fFont;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

} // namespace ksesh
