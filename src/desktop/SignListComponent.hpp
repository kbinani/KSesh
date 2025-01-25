#pragma once

#pragma GCC diagnostic ignored "-Wunused-variable"

namespace ksesh {

class SignListComponent : public juce::Component {
  struct Category {
    explicit Category(juce::String const &name, std::shared_ptr<juce::Path> path = nullptr) : name(name), path(path) {
    }

    juce::String name;
    std::shared_ptr<juce::Path> path;
  };

  struct TabButton {
    int x;
    int y;
    int width;
    int height;
    juce::String name;
    std::shared_ptr<juce::Path> path;
  };

  enum : int {
    tabButtonHeight = 28,
    tabButtonWidth = 60,

    tabButtonTextSize = 16,
    tabButtonSignSize = 22,

    scrollBarWidth = 8,
  };

public:
  explicit SignListComponent(std::shared_ptr<hb_font_t> const &font) : fFont(font), fOverlayColor(juce::Colours::transparentBlack) {
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

    fViewport = std::make_unique<juce::Viewport>();
    addAndMakeVisible(*fViewport);
    fContainer = std::make_unique<SignListButtonContainer>(font);
    fContainer->onClickSign = [this](Sign const &sign) {
      if (onClickSign) {
        onClickSign(sign);
      }
    };
    addAndMakeVisible(*fContainer);
    fViewport->setViewedComponent(fContainer.get(), false);
    fViewport->setScrollBarsShown(true, false, true, false);

    setActiveCategory(1);
    layout();
    setFocusContainerType(juce::Component::FocusContainerType::keyboardFocusContainer);
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(getLookAndFeel().findColour(juce::TextEditor::backgroundColourId));
    if (fTabButtons.empty()) {
      layout();
    }
    int const width = getWidth();
    int const height = getHeight();

    auto textColor = getLookAndFeel().findColour(juce::TextEditor::textColourId);
    auto borderColor = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    auto highlightColor = getLookAndFeel().findColour(juce::TextEditor::highlightColourId);
    auto highlightTextColor = getLookAndFeel().findColour(juce::TextButton::textColourOnId);
    auto activeColor = getLookAndFeel().findColour(juce::TextButton::buttonOnColourId);

    float scale = 1.0f / Harfbuzz::UnitsPerEm(fFont);
    float space = 2;

    for (int i = 0; i < (int)fTabButtons.size(); i++) {
      auto const &tb = fTabButtons[i];
      if (i == fMouseDownCategory) {
        g.setColour(activeColor);
        g.fillRect(tb.x, tb.y, tb.width, tb.height);
      } else if (i == fActiveCategory) {
        g.setColour(activeColor);
        g.fillRect(tb.x, tb.y, tb.width, tb.height);
      } else if (i == fHitTabButton) {
        g.setColour(highlightColor);
        g.fillRect(tb.x, tb.y, tb.width, tb.height);
      }
      g.setFont(tabButtonTextSize);
      if (i == fActiveCategory || i == fMouseDownCategory) {
        g.setColour(highlightTextColor);
      } else {
        g.setColour(textColor);
      }
      if (tb.path) {
        float textWidth = g.getCurrentFont().getStringWidthFloat(tb.name);

        auto bounds = tb.path->getBoundsTransformed(juce::AffineTransform::scale(scale * tabButtonSignSize, scale * tabButtonSignSize));
        float totalWidth = textWidth + bounds.getWidth();
        float x0 = tb.x + tb.width * 0.5f - totalWidth * 0.5f;
        g.drawText(tb.name, juce::Rectangle<float>(x0 + textWidth - textWidth * 2 - space, tb.y, textWidth * 2, tabButtonHeight), juce::Justification::centredRight);
        g.saveState();
        float x = x0 + textWidth - bounds.getX() + space;
        float y = tb.y + tb.height * 0.5f - bounds.getHeight() * 0.5f - bounds.getY();
        g.addTransform(juce::AffineTransform(scale * tabButtonSignSize, 0, x, 0, scale * tabButtonSignSize, y));
        g.fillPath(*tb.path);
        g.restoreState();
      } else {
        g.drawFittedText(tb.name, tb.x, tb.y, tb.width, tb.height, juce::Justification::centred, 1);
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

  void paintOverChildren(juce::Graphics &g) override {
    if (fOverlayColor.isTransparent()) {
      return;
    }
    g.fillAll(fOverlayColor);
  }

  void resized() override {
    int width = getWidth();
    int height = getHeight();
    auto bounds = getLocalBounds();
    layout();
    int const h = fRows * (tabButtonHeight + 1);
    fContainer->layout(bounds.getWidth());
    bounds.removeFromTop(h);
    fViewport->setBounds(bounds);
  }

  void mouseEnter(juce::MouseEvent const &e) override {
    updateButtonHit(e.getPosition());
  }

  void mouseMove(juce::MouseEvent const &e) override {
    updateButtonHit(e.getPosition());
  }

  void mouseExit(juce::MouseEvent const &e) override {
    if (fHitTabButton >= 0) {
      fHitTabButton = -1;
      updateCursor();
      repaint();
    }
  }

  void mouseDown(juce::MouseEvent const &e) override {
    fMouseDownCategory = -1;
    bool hit = false;
    for (int i = 0; i < (int)fTabButtons.size(); i++) {
      auto const &tb = fTabButtons[i];
      if (juce::Rectangle<float>(tb.x, tb.y, tb.width, tb.height).contains(e.getPosition().toFloat())) {
        fMouseDownCategory = i;
        updateCursor();
        repaint();
        return;
      }
    }
    updateCursor();
  }

  void mouseUp(juce::MouseEvent const &e) override {
    if (0 <= fMouseDownCategory && fMouseDownCategory < (int)fTabButtons.size()) {
      auto tb = fTabButtons[fMouseDownCategory];
      if (juce::Rectangle<float>(tb.x, tb.y, tb.width, tb.height).contains(e.getPosition().toFloat())) {
        setActiveCategory(fMouseDownCategory);
        if (onClickCategory) {
          onClickCategory();
        }
      }
    }
    fMouseDownCategory = -1;
    updateCursor();
    repaint();
  }

  void setTyping(juce::String const &typing) {
    fTyping = typing;
    setActiveCategory(0);
  }

  void setShowMdC(bool show) {
    fContainer->setShowMdC(show);
  }

  bool isShowMdC() const {
    return fContainer->isShowMdC();
  }

  void setOverlayColor(juce::Colour color) {
    fOverlayColor = color;
    repaint();
  }

private:
  void updateButtonHit(juce::Point<int> const &p) {
    int hitTabButton = -1;
    for (int i = 0; i < (int)fTabButtons.size(); i++) {
      auto const &tb = fTabButtons[i];
      if (juce::Rectangle<float>(tb.x, tb.y, tb.width, tb.height).contains(p.toFloat())) {
        hitTabButton = i;
        break;
      }
    }
    if (fHitTabButton != hitTabButton) {
      fHitTabButton = hitTabButton;
      repaint();
    }
    updateCursor();
  }

  void updateCursor() {
    if (fHitTabButton >= 0 || fMouseDownCategory >= 0) {
      setMouseCursor(juce::MouseCursor::PointingHandCursor);
    } else {
      setMouseCursor(juce::MouseCursor::NormalCursor);
    }
  }

  Category makeCategory(juce::String const &name, std::u32string const &sign) {
    auto path = std::make_shared<juce::Path>();
    *path = Harfbuzz::CreatePath(sign, fFont);
    return Category(name, path);
  }

  void layout() {
    using namespace std;
    int const width = getWidth();
    int const height = getHeight();
    int x = 0;
    int y = 0;
    fTabButtons.clear();
    fRows = 1;
    for (auto const &cat : fCategories) {
      if (x + tabButtonWidth > width) {
        y += tabButtonHeight + 1;
        x = 0;
        if (y > height) {
          break;
        }
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
    fContainer->layout(width);
  }

  void setActiveCategory(int index) {
    if (fActiveCategory != index || index == 0) {
      fActiveCategory = index;
      if (index != 0) {
        fTyping.clear();
      }
      if (0 <= index && index < (int)fCategories.size()) {
        fContainer->updateFilter(fCategories[index].name, fTyping);
      }
      layout();
      repaint();
    }
  }

public:
  std::function<void(Sign const &)> onClickSign;
  std::function<void()> onClickCategory;

private:
  std::unique_ptr<juce::Viewport> fViewport;
  std::unique_ptr<SignListButtonContainer> fContainer;
  std::shared_ptr<hb_font_t> fFont;
  std::vector<Category> fCategories;
  std::vector<TabButton> fTabButtons;

  int fRows = 0;
  int fActiveCategory = 0;
  int fHitTabButton = -1;
  int fMouseDownCategory = -1;
  juce::String fTyping;
  juce::Colour fOverlayColor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SignListComponent)
};

} // namespace ksesh
