#pragma once

namespace ksesh {

class ExampleComponent : public juce::Component {
  enum : int {
    margin = 24,
    padding = 60,
    comboBoxHeight = 40,
    space = 8,
  };

  struct Example {
    juce::String description;
    juce::String content;
  };

public:
  ExampleComponent(std::shared_ptr<hb_font_t> const &font, std::shared_ptr<AppSetting> const &setting) : fFont(font) {
    fEditor = std::make_unique<TextEditorComponent>(font, setting);
    fEditor->fOnEscapeKey = []() {
      juce::JUCEApplication::getInstance()->invoke(CommandID::commandHelpExampleClose, true);
    };
    addAndMakeVisible(*fEditor);

    fExamples = {
        {TRANS("Basic"), "i w Y3 Y1v A1 r:1 n km m t:niwt"},
        {TRANS("Cartouche, ligature, and grouping"), R"(< i mn:n t&w&t anx >
(d:d)&D&t m Hm 1:n stp:V16 pr anx DA s)"},
        {TRANS("Horizontal mirroring and cartouche"), "< C12\\ C2 N36:ms*s*s >"},
        {TRANS("Shading"), R"(A#1 A#2 A#3 A#4
A#12 A#34
A#1234 A#4132)"},
        {TRANS("Rotating glyph"), R"(mAa xrw
mAa\r1 xrw
mAa\r2 xrw
mAa\r3 xrw)"}};
    fSelector = std::make_unique<juce::ComboBox>();
    for (int i = 0; i < (int)fExamples.size(); i++) {
      fSelector->addItem(fExamples[i].description, i + 1);
    }
    addAndMakeVisible(*fSelector);
    auto e = fExamples[0];
    auto c = std::make_shared<Content>(U32StringFromJuceString(e.content), font);
    fEditor->resetText(e.content);
    fSelector->setSelectedId(1);
    fSelector->onChange = [this]() {
      onSelectorChange();
    };

    fClose = std::make_unique<juce::ShapeButton>("", juce::Colours::grey, juce::Colours::grey.brighter(), juce::Colours::grey.darker());
    juce::Path path;
    path.startNewSubPath(5, 5);
    path.lineTo(15, 15);
    path.startNewSubPath(15, 5);
    path.lineTo(5, 15);
    fClose->setShape(path, false, true, false);
    fClose->setOutline(getLookAndFeel().findColour(juce::TextEditor::textColourId), 5);
    fClose->onClick = []() {
      juce::JUCEApplication::getInstance()->invoke(CommandID::commandHelpExampleClose, true);
    };
    addAndMakeVisible(*fClose);
  }

  void resized() override {
    auto bounds = getLocalBounds().reduced(margin);
    fClose->setBounds(juce::Rectangle<int>(bounds.removeFromTop(20).removeFromRight(20)).translated(-10, 10));

    bounds.reduce(padding, padding);

    auto top = bounds.removeFromTop(comboBoxHeight);
    fSelector->setBounds(top);

    bounds.removeFromTop(space);

    fEditor->setBounds(bounds);
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(juce::Colours::black.withAlpha(0.5f));
    auto bounds = getLocalBounds().reduced(margin);

    g.setColour(getLookAndFeel().findColour(juce::TextButton::buttonColourId));
    g.fillRoundedRectangle(bounds.toFloat(), 8);
  }

private:
  void onSelectorChange() {
    int index = fSelector->getSelectedId() - 1;
    if (0 <= index && index < (int)fExamples.size()) {
      auto e = fExamples[index];
      auto c = std::make_shared<Content>(U32StringFromJuceString(e.content), fFont);
      fEditor->resetText(e.content);
      fEditor->focus();
    }
  }

private:
  std::unique_ptr<TextEditorComponent> fEditor;
  std::unique_ptr<juce::ShapeButton> fClose;
  std::shared_ptr<hb_font_t> fFont;
  std::unique_ptr<juce::ComboBox> fSelector;
  std::vector<Example> fExamples;
};

} // namespace ksesh
