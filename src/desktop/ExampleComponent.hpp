#pragma once

namespace ksesh {

class ExampleComponent : public juce::Component {
  enum : int {
    margin = 24,
    padding = 60,
  };

public:
  ExampleComponent(std::shared_ptr<hb_font_t> const &font, std::shared_ptr<AppSetting> const &setting) : fFont(font) {
    fEditor = std::make_unique<TextEditorComponent>();
    fEditor->onTextChange = [this](juce::String const &s, int start, int end, Direction direction) {
      auto c = std::make_shared<Content>(U32StringFromJuceString(s), fFont);
      fViewer->setContent(c);
    };
    addAndMakeVisible(*fEditor);

    fViewer = std::make_unique<HieroglyphComponent>(font);
    addAndMakeVisible(*fViewer);

    fClose = std::make_unique<juce::ShapeButton>("", juce::Colours::grey, juce::Colours::grey.brighter(), juce::Colours::grey.darker());
    juce::Path path;
    path.startNewSubPath(5, 5);
    path.lineTo(15, 15);
    path.startNewSubPath(15, 5);
    path.lineTo(5, 15);
    fClose->setShape(path, false, true, false);
    fClose->setOutline(getLookAndFeel().findColour(juce::TextEditor::textColourId), 5);
    fClose->onClick = [this]() {
      if (onClickClose) {
        onClickClose();
      }
    };
    addAndMakeVisible(*fClose);
  }

  void resized() override {
    auto bounds = getLocalBounds().reduced(margin);
    fClose->setBounds(juce::Rectangle<int>(bounds.removeFromTop(20).removeFromRight(20)).translated(-10, 10));
    bounds.reduce(padding, padding);
    fEditor->setBounds(bounds.removeFromLeft(bounds.getWidth() / 2));
    fViewer->setBounds(bounds);
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(juce::Colours::black.withAlpha(0.5f));
    auto bounds = getLocalBounds().reduced(margin);

    g.setColour(getLookAndFeel().findColour(juce::TextButton::buttonColourId));
    g.fillRoundedRectangle(bounds.toFloat(), 8);
  }

public:
  std::function<void()> onClickClose;
  std::unique_ptr<TextEditorComponent> fEditor;
  std::unique_ptr<juce::ShapeButton> fClose;
  std::unique_ptr<HieroglyphComponent> fViewer;
  std::shared_ptr<hb_font_t> fFont;
};

} // namespace ksesh
