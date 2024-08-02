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
    auto textColor = getLookAndFeel().findColour(juce::TextEditor::ColourIds::textColourId);
    auto caretColor = getLookAndFeel().findColour(juce::CaretComponent::caretColourId);
    auto highlightColor = getLookAndFeel().findColour(juce::TextEditor::highlightColourId);

    g.saveState();
    g.setColour(highlightColor);
    for (auto const &rect : fCursor.selectionRects) {
      g.fillRect(rect);
    }
    g.restoreState();

    float const upem = (float)Harfbuzz::UnitsPerEm(fFont);
    float const scale = fSetting.fontSize / upem;
    float const padding = fSetting.padding / scale;
    float const lineSpacing = fSetting.lineSpacing / scale;
    float dx = padding;
    float dy = padding;
    g.saveState();
    g.addTransform(juce::AffineTransform::scale(scale, scale));
    for (auto const &line : fContent->lines) {
      g.setColour(textColor);
      for (auto const &glyph : line->glyphs) {
        auto path = Harfbuzz::CreatePath(glyph.glyphId, fFont, glyph.x + dx, glyph.y + dy);
        if (path.getBounds().isEmpty()) {
          continue;
        }
        g.fillPath(path);
      }
      dy += lineSpacing + upem;
    }
    g.restoreState();

    if (fCursor.rect) {
      g.setColour(caretColor);
      g.fillRect(fCursor.rect->expanded(caretWidth * 0.5f, 0));
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

  void setSelectedRange(int start, int end, Direction direction) {
    if (fContent) {
      fCursor = fContent->cursor(start, end, direction, fFont, fSetting);
    }
    repaint();
  }

private:
  PresentationSetting fSetting;
  HbFontUniquePtr const &fFont;
  std::shared_ptr<Content> fContent;
  Cursor fCursor;
  static float constexpr caretWidth = 2;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HieroglyphComponent)
};

} // namespace ksesh
