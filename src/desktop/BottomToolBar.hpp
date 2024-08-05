#pragma once

namespace ksesh {

class BottomToolBar : public juce::Component {
public:
  BottomToolBar() {
    fShowMdCButton = std::make_unique<juce::ToggleButton>();
    fShowMdCButton->setButtonText(TRANS("MdC code"));
    fShowMdCButton->setClickingTogglesState(true);
    fShowMdCButton->onStateChange = [this]() {
      if (onChangeShowMdC) {
        onChangeShowMdC(fShowMdCButton->getToggleState());
      }
    };
    fShowMdCButton->setBounds(0, 0, 200, 24);
    addAndMakeVisible(*fShowMdCButton);
    fShowMdCButton->changeWidthToFitText();
  }

  void resized() override {
    auto bounds = getLocalBounds();
    bounds.removeFromRight(getHeight());
    fShowMdCButton->setBounds(bounds.removeFromRight(fShowMdCButton->getWidth()));
  }

  void initShowMdC(bool show) {
    fShowMdCButton->setToggleState(show, juce::dontSendNotification);
  }

public:
  std::function<void(bool show)> onChangeShowMdC;

private:
  std::unique_ptr<juce::ToggleButton> fShowMdCButton;
};

} // namespace ksesh
