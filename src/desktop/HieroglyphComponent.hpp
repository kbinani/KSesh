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

    float const upem = (float)Harfbuzz::UnitsPerEm(fFont);
    float const scale = fSetting.fontSize / upem;
    float const padding = fSetting.padding / scale;
    float const lineSpacing = fSetting.lineSpacing / scale;
    float dx = padding;
    float dy = padding;
    g.addTransform(juce::AffineTransform::scale(scale, scale));
    for (auto const &line : fContent->lines) {
      for (auto const &glyph : line->glyphs) {
        auto path = Harfbuzz::CreatePath(glyph.glyphId, fFont, glyph.x + dx, glyph.y + dy);
        if (path.getBounds().isEmpty()) {
          continue;
        }
        g.fillPath(path);
      }
      dy += lineSpacing + upem;
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
  PresentationSetting fSetting;
  HbFontUniquePtr const &fFont;
  std::shared_ptr<Content> fContent;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HieroglyphComponent)
};

} // namespace ksesh
