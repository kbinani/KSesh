#pragma once

namespace ksesh {

class SplitterComponent : public juce::Component {
  enum {
    resizerSize = 8,
  };

  class Resizer : public juce::Component {
  public:
    explicit Resizer(bool vertical) : fVertical(vertical) {
      setRepaintsOnMouseActivity(true);
      setMouseCursor(vertical ? juce::MouseCursor::LeftRightResizeCursor
                              : juce::MouseCursor::UpDownResizeCursor);
    }

    void paint(juce::Graphics &g) override {
      getLookAndFeel().drawStretchableLayoutResizerBar(g, getWidth(), getHeight(), fVertical,
                                                       isMouseOver(), isMouseButtonDown());
    }

  private:
    bool const fVertical;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Resizer)
  };

public:
  explicit SplitterComponent(juce::Component *leading, juce::Component *trailing, bool vertical) : fLeading(leading), fTrailing(trailing), fVertical(vertical) {
    fResizer = std::make_unique<Resizer>(vertical);
    addAndMakeVisible(fResizer.get());
    addAndMakeVisible(leading);
    addAndMakeVisible(trailing);
  }

  void resized() override {
    int const width = getWidth();
    int const height = getHeight();
    if (fVertical) {
      int w = (width - resizerSize) * fRatio;
      fLeading->setBounds(0, 0, w, height);
      fResizer->setBounds(w, 0, resizerSize, height);
      fTrailing->setBounds(w + resizerSize, 0, width - w - resizerSize, height);
    } else {
      int h = (height - resizerSize) * fRatio;
      fLeading->setBounds(0, 0, width, h);
      fResizer->setBounds(0, h, width, resizerSize);
      fTrailing->setBounds(0, h + resizerSize, width, height - h - resizerSize);
    }
  }

private:
  juce::Component *const fLeading;
  juce::Component *const fTrailing;
  bool const fVertical;
  std::unique_ptr<Resizer> fResizer;
  float fRatio = 0.5;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SplitterComponent)
};

} // namespace ksesh
