#pragma once

namespace ksesh {

class HieroglyphComponent : public juce::Component {
public:
  explicit HieroglyphComponent(HbFontUniquePtr const &font) : fFont(font) {
    setMouseCursor(juce::MouseCursor::IBeamCursor);
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(getLookAndFeel().findColour(juce::TextEditor::ColourIds::backgroundColourId));
    if (!fContent) {
      return;
    }
    auto textColor = getLookAndFeel().findColour(juce::TextEditor::ColourIds::textColourId);
    auto highlightTextColor = getLookAndFeel().findColour(juce::TextEditor::ColourIds::highlightedTextColourId);
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
    juce::Range<int> selection(fStart, fEnd);
    for (auto const &line : fContent->lines) {
      if (fStart == fEnd) {
        g.setColour(textColor);
      }
      for (auto const &glyph : line->glyphs) {
        auto path = Harfbuzz::CreatePath(glyph.glyphId, fFont, glyph.x + dx, glyph.y + dy);
        if (path.getBounds().isEmpty()) {
          continue;
        }
        if (fStart != fEnd) {
          bool selected = false;
          for (auto const &ch : line->chars) {
            if (!ch.sign || ch.cluster != (int)glyph.cluster) {
              continue;
            }
            if (selection.getIntersectionWith({line->rawOffset + ch.rawOffset, line->rawOffset + ch.rawOffset + (int)ch.raw.size()}).getLength() == (int)ch.raw.size()) {
              selected = true;
              break;
            }
          }
          g.setColour(selected ? highlightTextColor : textColor);
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

  void lookAndFeelChanged() override {
    repaint();
  }

  void mouseDown(juce::MouseEvent const &e) override {
    if (!fContent) {
      return;
    }
    if (e.mods.isLeftButtonDown()) {
      auto position = fContent->closestPosition(std::nullopt, e.getPosition().toFloat(), fFont, fSetting);
      fDown = position.location;
      setSelectedRange(position.location, position.location, position.direction);
      if (onSelectedRangeChange) {
        onSelectedRangeChange(position.location, position.location, position.direction);
      }
    }
  }

  void mouseDrag(juce::MouseEvent const &e) override {
    if (!fContent) {
      return;
    }
    if (e.mods.isLeftButtonDown()) {
      if (fDown) {
        auto position = fContent->closestPosition(*fDown, e.getPosition().toFloat(), fFont, fSetting);
        int start = std::min<int>(position.location, *fDown);
        int end = std::max<int>(position.location, *fDown);
        setSelectedRange(start, end, position.direction);
        if (onSelectedRangeChange) {
          onSelectedRangeChange(start, end, position.direction);
        }
      }
    }
  }

  void setContent(std::shared_ptr<Content> const &c) {
    fContent = c;
    if (fContent) {
      repaint();
    }
  }

  void setSelectedRange(int start, int end, Direction direction) {
    fStart = start;
    fEnd = end;
    fDirection = direction;
    if (fContent) {
      fCursor = fContent->cursor(start, end, direction, fFont, fSetting);
    }
    repaint();
  }

public:
  std::function<void(int start, int end, Direction direction)> onSelectedRangeChange;

private:
  PresentationSetting fSetting;
  HbFontUniquePtr const &fFont;
  std::shared_ptr<Content> fContent;
  Cursor fCursor;
  std::optional<int> fDown;
  int fStart = 0;
  int fEnd = 0;
  Direction fDirection = Direction::Forward;
  static float constexpr caretWidth = 2;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HieroglyphComponent)
};

} // namespace ksesh
