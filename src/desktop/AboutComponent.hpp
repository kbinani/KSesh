#pragma once

namespace ksesh {

class AboutComponent : public juce::Component {
  enum : int {
    aboutComponentMargin = 24,
    aboutComponentPadding = 60,
  };

  class AttributedStringComponent : public juce::Component {
  public:
    void paint(juce::Graphics &g) override {
      auto bounds = getLocalBounds().toFloat();
      auto padded = bounds.reduced(aboutComponentPadding);
      fString.draw(g, padded);
    }

    float getTextHeight(float width) {
      juce::TextLayout layout;
      layout.createLayout(fString, width);
      return layout.getHeight() + 2 * aboutComponentPadding;
    }

    juce::AttributedString fString;
  };

  void addParagraph(juce::String const &title, std::optional<juce::String> url, juce::String const &lines, juce::AttributedString &str) {
    auto color = getLookAndFeel().findColour(juce::TextEditor::textColourId);
    juce::Font regular(juce::FontOptions(14));
    juce::Font bold = regular.boldened();
    str.append(title + "\n", bold, color);
    str.append("\n", regular, color);
    if (url) {
      str.append(*url + "\n", regular, color);
      str.append("\n", regular, color);
    }
    for (auto const &line : juce::StringArray::fromLines(lines.trimStart())) {
      str.append(line + "\n", regular, color);
    }
    str.append("\n", regular, color);
  }

public:
  AboutComponent() {
    fContainer = std::make_unique<juce::Viewport>();
    fContainer->setScrollBarsShown(true, false, true, false);
    addAndMakeVisible(*fContainer);
    fContent = std::make_unique<AttributedStringComponent>();
    fContainer->setViewedComponent(fContent.get(), false);
    fCloseButton = std::make_unique<juce::ShapeButton>("", juce::Colours::grey, juce::Colours::grey.brighter(), juce::Colours::grey.darker());
    juce::Path path;
    path.startNewSubPath(5, 5);
    path.lineTo(15, 15);
    path.startNewSubPath(15, 5);
    path.lineTo(5, 15);
    fCloseButton->setShape(path, false, true, false);
    fCloseButton->setOutline(getLookAndFeel().findColour(juce::TextEditor::textColourId), 5);
    fCloseButton->onClick = []() {
      juce::JUCEApplication::getInstance()->invoke(CommandID::commandHelpAboutClose, true);
    };
    addAndMakeVisible(*fCloseButton);

    addParagraph("About KSesh",
                 "https://github.com/kbinani/KSesh",
                 juce::String::fromUTF8(BinaryData::KSesh_txt, BinaryData::KSesh_txtSize),
                 fContent->fString);
    addParagraph("Egyptian Text Font",
                 "https://github.com/microsoft/font-tools/tree/main/EgyptianOpenType/font",
                 juce::String::fromUTF8(BinaryData::Egyptian_Text_Font_txt, BinaryData::Egyptian_Text_Font_txtSize),
                 fContent->fString);
    addParagraph("JUCE Framework",
                 "https://github.com/juce-framework/JUCE, https://github.com/kbinani/JUCE",
                 juce::String::fromUTF8(BinaryData::JUCE_Framework_txt, BinaryData::JUCE_Framework_txtSize),
                 fContent->fString);
    addParagraph("PDFGen",
                 "https://github.com/AndreRenaud/PDFGen",
                 juce::String::fromUTF8(BinaryData::PDFGen_txt, BinaryData::PDFGen_txtSize),
                 fContent->fString);
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(juce::Colours::black.withAlpha(0.5f));
    g.setColour(getLookAndFeel().findColour(juce::TextButton::buttonColourId));
    auto bounds = getLocalBounds().reduced(aboutComponentMargin).toFloat();
    g.fillRoundedRectangle(bounds, 8);
  }

  void resized() override {
    auto bounds = getLocalBounds();
    bounds.reduce(aboutComponentMargin, aboutComponentMargin);
    fContainer->setBounds(bounds);
    auto b = fContent->getBounds().withWidth(bounds.getWidth());
    auto textHeight = std::max<float>(bounds.getHeight(), fContent->getTextHeight(bounds.getWidth() - 2 * aboutComponentPadding));
    fContent->setBounds(b.withHeight(textHeight));
    fCloseButton->setBounds(juce::Rectangle<int>(bounds.removeFromTop(20).removeFromRight(20)).translated(-10, 10));
  }

  bool keyPressed(juce::KeyPress const &ev) override {
    if (ev.getModifiers() == juce::ModifierKeys::noModifiers && ev.getKeyCode() == juce::KeyPress::escapeKey) {
      juce::JUCEApplication::getInstance()->invoke(CommandID::commandHelpAboutClose, true);
      return true;
    }
    return false;
  }

private:
  std::unique_ptr<juce::Viewport> fContainer;
  std::unique_ptr<AttributedStringComponent> fContent;
  std::unique_ptr<juce::ShapeButton> fCloseButton;
};

} // namespace ksesh
