#pragma once

namespace ksesh {

class SignListComponent : public juce::Component {
  struct Category {
    explicit Category(juce::String const &name, juce::Path path = juce::Path()) : name(name), path(path) {
    }

    juce::String name;
    juce::Path path;
  };

  struct TabButton {
    int x;
    int y;
    int width;
    int height;
    juce::String name;
    juce::Path path;
  };

  struct SignButton {
    int x;
    int y;
    int width;
    int height;
    juce::String sign;
  };

  enum {
    tabButtonHeight = 28,
    tabButtonWidth = 60,
  };

public:
  explicit SignListComponent(HbFontUniquePtr const &font) : fFont(font) {
    using namespace std::literals::string_literals;
    fCategories.push_back(Category("typing"));
    fCategories.push_back(makeCategory("A", U"𓀀"s));
    fCategories.push_back(makeCategory("B", U"𓁐"s));
    fCategories.push_back(makeCategory("C", U"𓁚"s));
    fCategories.push_back(makeCategory("D", U"𓁶"s));
    fCategories.push_back(makeCategory("E", U"𓃒"s));
    fCategories.push_back(makeCategory("F", U"𓃾"s));
    fCategories.push_back(makeCategory("G", U"𓄿"s));
    fCategories.push_back(makeCategory("H", U"𓅿"s));
    fCategories.push_back(makeCategory("I", U"𓆈"s));
    fCategories.push_back(makeCategory("K", U"𓆛"s));
    fCategories.push_back(makeCategory("L", U"𓆣"s));
    fCategories.push_back(makeCategory("M", U"𓆭"s));
    fCategories.push_back(makeCategory("N", U"𓇯"s));
    fCategories.push_back(makeCategory("O", U"𓉐"s));
    fCategories.push_back(makeCategory("P", U"𓊛"s));
    fCategories.push_back(makeCategory("Q", U"𓊨"s));
    fCategories.push_back(makeCategory("R", U"𓊯"s));
    fCategories.push_back(makeCategory("S", U"𓋑"s));
    fCategories.push_back(makeCategory("T", U"𓌇"s));
    fCategories.push_back(makeCategory("U", U"𓌳"s));
    fCategories.push_back(makeCategory("V", U"𓍢"s));
    fCategories.push_back(makeCategory("W", U"𓎯"s));
    fCategories.push_back(makeCategory("X", U"𓏏"s));
    fCategories.push_back(makeCategory("Y", U"𓏛"s));
    fCategories.push_back(makeCategory("Z", U"𓏤"s));
    fCategories.push_back(makeCategory("Aa", U"𓐍"s));
    fCategories.push_back(Category("tall"));
    fCategories.push_back(Category("wide"));
    fCategories.push_back(Category("small"));
    // fCategories.push_back(makeCategory("NL", U"𓈠");
    // fCategories.push_back(makeCategory("NU", U"𓈶");
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(getLookAndFeel().findColour(juce::TextEditor::backgroundColourId));
    if (fTabButtons.empty()) {
      layout();
    }
    int const width = getWidth();

    auto textColor = getLookAndFeel().findColour(juce::TextEditor::textColourId);
    auto borderColor = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    //    auto highlightColor = getLookAndFeel().findColour(juce::TextEditor::highlightColourId);

    float scale = 1.0f / Harfbuzz::UnitsPerEm(fFont);
    float space = 2;

    for (auto const &tb : fTabButtons) {
      float textSize = 16;
      float signSize = 20;
      g.setFont(textSize);
      g.setColour(textColor);
      if (tb.path.isEmpty()) {
        g.drawFittedText(tb.name, tb.x, tb.y, tb.width, tb.height, juce::Justification::centred, 1);
      } else {
        float textWidth = g.getCurrentFont().getStringWidthFloat(tb.name);

        auto bounds = tb.path.getBoundsTransformed(juce::AffineTransform::scale(scale * signSize, scale * signSize));
        float totalWidth = textWidth + bounds.getWidth();
        float x0 = tb.x + tb.width * 0.5f - totalWidth * 0.5f;
        g.drawText(tb.name, juce::Rectangle<float>(x0 + textWidth - textWidth * 2 - space, tb.y, textWidth * 2, tabButtonHeight), juce::Justification::centredRight);
        g.saveState();
        float x = x0 + textWidth - bounds.getX() + space;
        float y = tb.y + tb.height * 0.5f - bounds.getHeight() * 0.5f - bounds.getY();
        g.addTransform(juce::AffineTransform(scale * signSize, 0, x, 0, scale * signSize, y));
        g.fillPath(tb.path);
        g.restoreState();
      }
      g.setColour(borderColor);
      g.drawLine(tb.x + tb.width + 1, tb.y, tb.x + tb.width + 1, tb.y + tb.height, 1);
    }
    g.setColour(borderColor);
    for (int i = 0; i < fRows; i++) {
      int y = (i + 1) * (tabButtonHeight + 1);
      g.drawLine(0, y, width, y, 1);
    }
  }

  void resized() override {
    layout();
  }

private:
  Category makeCategory(juce::String const &name, std::u32string const &sign) {
    return Category(name, Harfbuzz::CreatePath(sign, fFont));
  }

  void layout() {
    int const width = getWidth();
    //    int const height = getHeight();
    int x = 0;
    int y = 0;
    fTabButtons.clear();
    fRows = 1;
    for (auto const &cat : fCategories) {
      if (x + tabButtonWidth > width) {
        y += tabButtonHeight + 1;
        x = 0;
        fRows++;
      }
      TabButton tb;
      tb.x = x;
      tb.y = y;
      tb.width = tabButtonWidth;
      tb.height = tabButtonHeight;
      tb.name = cat.name;
      tb.path = cat.path;
      fTabButtons.push_back(tb);
      x += tabButtonWidth + 1;
    }
  }

private:
  HbFontUniquePtr const &fFont;
  std::vector<Category> fCategories;
  std::vector<TabButton> fTabButtons;
  std::vector<SignButton> fSignButtons;
  int fRows = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SignListComponent)
};

} // namespace ksesh
