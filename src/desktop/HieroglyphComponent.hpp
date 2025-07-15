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
    auto document = fDocument.lock();
    if (!document) {
      return;
    }
    auto textColor = getLookAndFeel().findColour(juce::TextEditor::ColourIds::textColourId);
    auto highlightTextColor = getLookAndFeel().findColour(juce::TextEditor::ColourIds::highlightedTextColourId);
    auto caretColor = getLookAndFeel().findColour(juce::CaretComponent::caretColourId);
    auto highlightColor = getLookAndFeel().findColour(juce::TextEditor::highlightColourId);
    document->draw(
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
    auto document = fDocument.lock();
    if (!document) {
      return;
    }
    if (e.mods.isLeftButtonDown()) {
      auto position = document->closestPosition(std::nullopt, e.getPosition().toFloat(), fSetting->getPresentationSetting());
      fDown = position.fLocation;
      setSelectedRange(position.fLocation, position.fLocation, position.fDirection);
      if (onSelectedRangeChange) {
        onSelectedRangeChange(position.fLocation, position.fLocation, position.fDirection);
      }
    }
  }

  void mouseDrag(juce::MouseEvent const &e) override {
    auto document = fDocument.lock();
    if (!document) {
      return;
    }
    if (e.mods.isLeftButtonDown()) {
      if (fDown) {
        auto position = document->closestPosition(*fDown, e.getPosition().toFloat(), fSetting->getPresentationSetting());
        int start = std::min<int>(position.fLocation, *fDown);
        int end = std::max<int>(position.fLocation, *fDown);
        setSelectedRange(start, end, position.fDirection);
        if (onSelectedRangeChange) {
          onSelectedRangeChange(start, end, position.fDirection);
        }
      }
    }
  }

  void setDocument(std::shared_ptr<Document> const &d) {
    fDocument = d;
    if (d) {
      repaint();
    }
  }

  void setSelectedRange(int start, int end, Direction direction) {
    fStart = start;
    fEnd = end;
    fDirection = direction;
    auto document = fDocument.lock();
    if (document) {
      fCursor = document->cursor(start, end, direction, fSetting->getPresentationSetting());
    }
    repaint();
  }

  void changeListenerCallback(juce::ChangeBroadcaster *source) override {
    if (source != fSetting.get()) {
      return;
    }
    auto document = fDocument.lock();
    if (document) {
      fCursor = document->cursor(fStart, fEnd, fDirection, fSetting->getPresentationSetting());
      repaint();
    }
  }

public:
  std::function<void(int start, int end, Direction direction)> onSelectedRangeChange;

private:
  std::shared_ptr<AppSetting> fSetting;
  std::weak_ptr<Document> fDocument;
  Cursor fCursor;
  std::optional<int> fDown;
  int fStart = 0;
  int fEnd = 0;
  Direction fDirection = Direction::Forward;
  static float constexpr caretWidth = 2;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HieroglyphComponent)
};

} // namespace ksesh
