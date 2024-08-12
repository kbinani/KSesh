#pragma once

namespace ksesh {

class LookAndFeel : public juce::LookAndFeel_V4 {
  using super = juce::LookAndFeel_V4;

public:
  LookAndFeel() {
  }

  void drawTextEditorOutline(juce::Graphics &, int width, int height, juce::TextEditor &) override {
  }

  int getMenuWindowFlags() override {
    return 0;
  }

  int getDefaultMenuBarHeight() override {
    return 24;
  }

  void getIdealPopupMenuItemSize(juce::String const &text, const bool isSeparator, int standardMenuItemHeight, int &idealWidth, int &idealHeight) override {
    int w, h;
    super::getIdealPopupMenuItemSize(text, isSeparator, standardMenuItemHeight, w, h);
    idealWidth = w;
    if (isSeparator) {
      idealHeight = h;
    } else {
      idealHeight = std::max<int>(getDefaultMenuBarHeight(), h);
    }
  }
};

} // namespace ksesh
