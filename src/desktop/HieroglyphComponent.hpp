#pragma once

namespace ksesh {

class HieroglyphComponent : public juce::Component, public juce::ChangeListener {
public:
  explicit HieroglyphComponent(std::shared_ptr<AppSetting> const &setting) : fSetting(setting) {
    setMouseCursor(juce::MouseCursor::IBeamCursor);
    fSetting->addChangeListener(this);
  }

  ~HieroglyphComponent() {
    fSetting->removeChangeListener(this);
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
    fContent->draw(
        g,
        fStart,
        fEnd,
        fDirection,
        fSetting->getPresentationSetting(),
        caretWidth,
        textColor,
        highlightTextColor,
        caretColor,
        highlightColor);
  }

  void lookAndFeelChanged() override {
    repaint();
  }

  void mouseDown(juce::MouseEvent const &e) override {
    if (!fContent) {
      return;
    }
    if (e.mods.isLeftButtonDown()) {
      auto position = fContent->closestPosition(std::nullopt, e.getPosition().toFloat(), fSetting->getPresentationSetting());
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
        auto position = fContent->closestPosition(*fDown, e.getPosition().toFloat(), fSetting->getPresentationSetting());
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
      fCursor = fContent->cursor(start, end, direction, fSetting->getPresentationSetting());
    }
    repaint();
  }

  void changeListenerCallback(juce::ChangeBroadcaster *source) override {
    if (source != fSetting.get()) {
      return;
    }
    if (fContent) {
      fCursor = fContent->cursor(fStart, fEnd, fDirection, fSetting->getPresentationSetting());
      repaint();
    }
  }

public:
  std::function<void(int start, int end, Direction direction)> onSelectedRangeChange;

private:
  std::shared_ptr<AppSetting> fSetting;
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
