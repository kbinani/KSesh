#pragma once

namespace ksesh {

class SignListComponent : public juce::Component {
  struct Category {
    explicit Category(juce::String const &name, std::shared_ptr<juce::Path> path = nullptr) : fName(name), fPath(path) {
    }

    juce::String fName;
    std::shared_ptr<juce::Path> fPath;
  };

  struct TabButton {
    int fX;
    int fY;
    int fWidth;
    int fHeight;
    juce::String fName;
    std::shared_ptr<juce::Path> fPath;
  };

  enum : int {
    tabButtonHeight = 28,
    tabButtonWidth = 60,

    tabButtonTextSize = 16,
    tabButtonSignSize = 22,

    scrollBarWidth = 8,
  };

public:
  explicit SignListComponent(std::shared_ptr<FontAdapter> const &font) : fFont(font), fOverlayColor(juce::Colours::transparentBlack) {
    CreateCategories(font, fCategories);

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

    auto font = fFont.lock();
    if (!font) {
      return;
    }

    float space = 2;

    for (int i = 0; i < (int)fTabButtons.size(); i++) {
      auto const &tb = fTabButtons[i];
      if (i == fMouseDownCategory) {
        g.setColour(activeColor);
        g.fillRect(tb.fX, tb.fY, tb.fWidth, tb.fHeight);
      } else if (i == fActiveCategory) {
        g.setColour(activeColor);
        g.fillRect(tb.fX, tb.fY, tb.fWidth, tb.fHeight);
      } else if (i == fHitTabButton) {
        g.setColour(highlightColor);
        g.fillRect(tb.fX, tb.fY, tb.fWidth, tb.fHeight);
      }
      g.setFont(tabButtonTextSize);
      if (i == fActiveCategory || i == fMouseDownCategory) {
        g.setColour(highlightTextColor);
      } else {
        g.setColour(textColor);
      }
      if (tb.fPath) {
        float textWidth = juce::GlyphArrangement::getStringWidth(g.getCurrentFont(), tb.fName);

        auto bounds = tb.fPath->getBoundsTransformed(juce::AffineTransform::scale(tabButtonSignSize, tabButtonSignSize));
        float totalWidth = textWidth + bounds.getWidth();
        float x0 = tb.fX + tb.fWidth * 0.5f - totalWidth * 0.5f;
        g.drawText(tb.fName, juce::Rectangle<float>(x0 + textWidth - textWidth * 2 - space, tb.fY, textWidth * 2, tabButtonHeight), juce::Justification::centredRight);
        g.saveState();
        float x = x0 + textWidth - bounds.getX() + space;
        float y = tb.fY + tb.fHeight * 0.5f - bounds.getHeight() * 0.5f - bounds.getY();
        g.addTransform(juce::AffineTransform(tabButtonSignSize, 0, x, 0, tabButtonSignSize, y));
        g.fillPath(*tb.fPath);
        g.restoreState();
      } else {
        g.drawFittedText(tb.fName, tb.fX, tb.fY, tb.fWidth, tb.fHeight, juce::Justification::centred, 1);
      }
      g.setColour(borderColor);
      g.drawLine(tb.fX + tb.fWidth + 1, tb.fY, tb.fX + tb.fWidth + 1, tb.fY + tb.fHeight, 1);
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
      if (juce::Rectangle<float>(tb.fX, tb.fY, tb.fWidth, tb.fHeight).contains(e.getPosition().toFloat())) {
        fMouseDownCategory = i;
        updateCursor();
        repaint();
        return;
      }
    }
    updateCursor();
  }

  void mouseUp(juce::MouseEvent const &e) override {
    bool hit = false;
    if (0 <= fMouseDownCategory && fMouseDownCategory < (int)fTabButtons.size()) {
      auto tb = fTabButtons[fMouseDownCategory];
      if (juce::Rectangle<float>(tb.fX, tb.fY, tb.fWidth, tb.fHeight).contains(e.getPosition().toFloat())) {
        hit = true;
        setActiveCategory(fMouseDownCategory);
        if (onClickCategory) {
          onClickCategory();
        }
      }
    }
    fMouseDownCategory = -1;
    updateCursor();
    repaint();
    if (!hit && e.mouseWasClicked() && onClickBackground) {
      onClickBackground();
    }
  }

  void setTyping(juce::String const &typing) {
    fTyping = typing;
    setActiveCategory(0);
  }

  void setOverlayColor(juce::Colour color) {
    fOverlayColor = color;
    repaint();
  }

  void setActiveCategory(int index) {
    if (fActiveCategory != index || index == 0) {
      fActiveCategory = index;
      if (index != 0) {
        fTyping.clear();
      }
      if (0 <= index && index < (int)fCategories.size()) {
        fContainer->updateFilter(fCategories[index].fName, fTyping);
      }
      layout();
      fContainer->resetSignSelection();
      repaint();
    }
  }

  void moveSignSelection(int dx, int dy) {
    fContainer->moveSignSelection(dx, dy);
  }

  void resetSignSelection() {
    fContainer->resetSignSelection();
  }

  void moveCategory(int delta) {
    int next = fActiveCategory + delta;
    int max = (int)fCategories.size() - 1;
    if (next <= 0) {
      next = max;
    } else if (max < next) {
      next = 1;
    }
    setActiveCategory(next);
  }

  void useSelectedSign() {
    fContainer->useSelectedSign();
  }

  void setFont(std::shared_ptr<FontAdapter> const &font) {
    fContainer->setFont(font);
    fFont = font;
    std::vector<Category> categories;
    CreateCategories(font, categories);
    fCategories.swap(categories);
    layout();
    repaint();
  }

private:
  static void CreateCategories(std::shared_ptr<FontAdapter> const &font, std::vector<Category> &categories) {
    using namespace std::literals::string_literals;
    categories.push_back(Category("typing"));
    AppendCategory(font, "A", categories);
    AppendCategory(font, "B", categories);
    AppendCategory(font, "C", categories);
    AppendCategory(font, "D", categories);
    AppendCategory(font, "E", categories);
    AppendCategory(font, "F", categories);
    AppendCategory(font, "G", categories);
    AppendCategory(font, "H", categories);
    AppendCategory(font, "I", categories);
    AppendCategory(font, "K", categories);
    AppendCategory(font, "L", categories);
    AppendCategory(font, "M", categories);
    AppendCategory(font, "N", categories);
    AppendCategory(font, "O", categories);
    AppendCategory(font, "P", categories);
    AppendCategory(font, "Q", categories);
    AppendCategory(font, "R", categories);
    AppendCategory(font, "S", categories);
    AppendCategory(font, "T", categories);
    AppendCategory(font, "U", categories);
    AppendCategory(font, "V", categories);
    AppendCategory(font, "W", categories);
    AppendCategory(font, "X", categories);
    AppendCategory(font, "Y", categories);
    AppendCategory(font, "Z", categories);
    AppendCategory(font, "Aa", categories);
    categories.push_back(Category("tall"));
    categories.push_back(Category("wide"));
    categories.push_back(Category("small"));
    AppendCategory(font, "NL", categories);
    AppendCategory(font, "NU", categories);
  }

  void updateButtonHit(juce::Point<int> const &p) {
    int hitTabButton = -1;
    for (int i = 0; i < (int)fTabButtons.size(); i++) {
      auto const &tb = fTabButtons[i];
      if (juce::Rectangle<float>(tb.fX, tb.fY, tb.fWidth, tb.fHeight).contains(p.toFloat())) {
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

  static void AppendCategory(std::shared_ptr<FontAdapter> const &font, juce::String const &name, std::vector<Category> &categories) {
    for (auto [mdc, codepoint] : SignList::Signs()) {
      if (codepoint.size() != 1) {
        continue;
      }
      auto m = JuceStringFromU32String(mdc);
      if (!m.startsWith(name)) {
        continue;
      }
      if (m.length() <= name.length()) {
        continue;
      }
      auto first = m.substring(name.length(), name.length() + 1);
      if (!first.containsAnyOf("0123456789")) {
        continue;
      }
      char32_t cp = codepoint[0];
      if (!font->fFont->has_glyph(cp)) {
        continue;
      }
      auto path = std::make_shared<juce::Path>();
      *path = Harfbuzz::CreatePath(codepoint, font->fFont.get());
      path->applyTransform(juce::AffineTransform::translation(0, -font->fY).scaled(font->fScale));
      Category category(name, path);
      categories.push_back(category);
      break;
    }
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
      tb.fX = x;
      tb.fY = y;
      tb.fWidth = tabButtonWidth;
      tb.fHeight = tabButtonHeight;
      tb.fName = cat.fName;
      tb.fPath = cat.fPath;
      fTabButtons.push_back(tb);
      x += tabButtonWidth + 1;
    }
    fContainer->layout(width);
  }

public:
  std::function<void(Sign const &)> onClickSign;
  std::function<void()> onClickCategory;
  std::function<void()> onClickBackground;

private:
  std::unique_ptr<juce::Viewport> fViewport;
  std::unique_ptr<SignListButtonContainer> fContainer;
  std::weak_ptr<FontAdapter> fFont;
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
