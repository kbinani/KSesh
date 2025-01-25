#pragma once

namespace ksesh {

class SplitterComponent : public juce::Component, public juce::ComponentListener {
  enum : int {
    resizerSize = 8,
  };

  class Resizer : public juce::Component {
  public:
    explicit Resizer(bool vertical) : fVertical(vertical) {
      setInterceptsMouseClicks(false, false);
    }

    void paint(juce::Graphics &g) override {
      getLookAndFeel().drawStretchableLayoutResizerBar(g, getWidth(), getHeight(), fVertical, fMouseOver, (bool)fMouseDown);
    }

  public:
    bool fMouseOver = false;
    std::optional<juce::Point<int>> fMouseDown;

  private:
    bool const fVertical;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Resizer)
  };

public:
  explicit SplitterComponent(juce::Component *leading, juce::Component *trailing, bool vertical) : fLeading(leading), fTrailing(trailing), fVertical(vertical), fOverlayColor(juce::Colours::transparentBlack) {
    fResizer = std::make_unique<Resizer>(vertical);
    addAndMakeVisible(fResizer.get());
    addAndMakeVisible(leading);
    addAndMakeVisible(trailing);
    setMouseCursor(vertical ? juce::MouseCursor::LeftRightResizeCursor
                            : juce::MouseCursor::UpDownResizeCursor);
    fLeading->addComponentListener(this);
    fTrailing->addComponentListener(this);
  }

  ~SplitterComponent() {
    fLeading->removeComponentListener(this);
    fTrailing->removeComponentListener(this);
  }

  void resized() override {
    int const width = getWidth();
    int const height = getHeight();
    if (!fLeading->isVisible() && !fTrailing->isVisible()) {
      fResizer->setVisible(false);
      return;
    } else if (!fLeading->isVisible() || !fTrailing->isVisible()) {
      fResizer->setVisible(false);
      if (fLeading->isVisible()) {
        fLeading->setBounds(0, 0, width, height);
      } else if (fTrailing->isVisible()) {
        fTrailing->setBounds(0, 0, width, height);
      }
      return;
    }
    fResizer->setVisible(true);
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

  void mouseEnter(juce::MouseEvent const &e) override {
    if (fResizer->getBoundsInParent().contains(e.getPosition())) {
      fResizer->fMouseOver = true;
      fResizer->repaint();
    }
  }

  void mouseDown(juce::MouseEvent const &e) override {
    if (e.mods.isLeftButtonDown() && fResizer->getBoundsInParent().contains(e.getPosition())) {
      fResizer->fMouseDown = e.getPosition() - fResizer->getBoundsInParent().getTopLeft();
      fResizer->repaint();
    }
  }

  void mouseDrag(juce::MouseEvent const &e) override {
    if (fResizer->fMouseDown) {
      if (fVertical) {
        float width = getWidth();
        int x = (e.getPosition() - *fResizer->fMouseDown).getX() + resizerSize / 2;
        fRatio = std::min<float>(std::max<float>(x / width, 0), 1);
      } else {
        float height = getHeight();
        int y = (e.getPosition() - *fResizer->fMouseDown).getY() + resizerSize / 2;
        fRatio = std::min<float>(std::max<float>(y / height, 0), 1);
      }
      resized();
    }
  }

  void mouseUp(juce::MouseEvent const &e) override {
    fResizer->fMouseDown = std::nullopt;
    fResizer->repaint();
  }

  void mouseExit(juce::MouseEvent const &e) override {
    fResizer->fMouseOver = false;
    fResizer->repaint();
  }

  void componentVisibilityChanged(juce::Component &comp) override {
    resized();
  }

  void paintOverChildren(juce::Graphics &g) override {
    if (fOverlayColor.isTransparent()) {
      return;
    }
    g.fillAll(fOverlayColor);
  }

  void setOverlayColor(juce::Colour color) {
    fOverlayColor = color;
    repaint();
  }

private:
  juce::Component *const fLeading;
  juce::Component *const fTrailing;
  bool const fVertical;
  std::unique_ptr<Resizer> fResizer;
  float fRatio = 0.5;
  juce::Colour fOverlayColor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SplitterComponent)
};

} // namespace ksesh
