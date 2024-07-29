#pragma once

namespace ksesh {

class HieroglyphComponent : public juce::Component {
public:
  explicit HieroglyphComponent(HbFontUniquePtr const &font) : fFont(font) {}

  void paint(juce::Graphics &g) override {
    g.fillAll(getLookAndFeel().findColour(juce::TextEditor::ColourIds::backgroundColourId));
    if (!fContent) {
      return;
    }
    g.setColour(getLookAndFeel().findColour(juce::TextEditor::ColourIds::textColourId));
    float fontSize = 48;
    float margin = 8;

    float scale = fontSize / (float)Harfbuzz::UnitsPerEm(fFont);
    g.addTransform(juce::AffineTransform::translation(margin, margin));
    g.addTransform(juce::AffineTransform::scale(scale, scale));
    for (auto const &line : fContent->lines) {
      for (auto const &glyph : line->glyphs) {
        auto path = Harfbuzz::CreatePath(glyph.glyphId, fFont, glyph.x, glyph.y);
        if (path.getBounds().isEmpty()) {
          continue;
        }
        g.fillPath(path);
      }
    }
  }

  void setContent(std::shared_ptr<Content> const &c) {
    fContent = c;
    if (fContent) {
      repaint();
    }
  }

  void lookAndFeelChanged() override {
    repaint();
  }

private:
  HbFontUniquePtr const &fFont;
  std::shared_ptr<Content> fContent;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HieroglyphComponent)
};

} // namespace ksesh
