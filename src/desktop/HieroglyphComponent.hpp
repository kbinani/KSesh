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
    auto caretColor = getLookAndFeel().findColour(juce::TextEditor::highlightColourId);

    g.saveState();
    g.setColour(caretColor.withAlpha(0.2f));
    for (auto const &rect : fCursor.selectionRects) {
      g.fillRect(rect.expanded(cursorPadding * 0.5f, cursorPadding));
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
      g.setColour(caretColor.withAlpha(0.5f));
      g.fillRect(fCursor.rect->expanded(cursorPadding * 0.5f, cursorPadding));
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
      fCursor = fContent->cursor(start, end, fFont, fSetting.fontSize, fSetting.lineSpacing, fSetting.padding, direction);
    }
    repaint();
  }

private:
  PresentationSetting fSetting;
  HbFontUniquePtr const &fFont;
  std::shared_ptr<Content> fContent;
  Cursor fCursor;
  static float constexpr cursorPadding = 4;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HieroglyphComponent)
};

} // namespace ksesh
