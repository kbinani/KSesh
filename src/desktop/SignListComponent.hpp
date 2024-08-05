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

  enum {
    tabButtonHeight = 28,
    tabButtonWidth = 60,

    tabButtonTextSize = 16,
    tabButtonSignSize = 22,

    scrollBarWidth = 8,
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

    for (auto const &it : SignList::Signs()) {
      auto s = makeSign(it.first, it.second);
      fAllSigns[s.name] = s;
    }

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
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(getLookAndFeel().findColour(juce::TextEditor::backgroundColourId));
    if (fTabButtons.empty()) {
      layout();
    }
    int const width = getWidth();

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

  void resized() override {
    int const width = getWidth();
    int const height = getHeight();
    layout();
    int const h = fRows * (tabButtonHeight + 1);
    int const preferredHeight = fContainer->layout(width);
    fViewport->setBounds(0, h, width, height - h);
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

  Sign makeSign(std::u32string const &name, std::u32string const &sign) {
    Sign s;
    s.name = JuceStringFromU32String(name);
    s.path = std::make_shared<juce::Path>();
    *s.path = Harfbuzz::CreatePath(sign, fFont);
    auto found = SignList::MapReverse(sign);
    s.mcd.reserve(found.size());
    for (size_t i = 0; i < found.size(); i++) {
      s.mcd.push_back(JuceStringFromU32String(found[i]));
    }
    return s;
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

  void filterSigns() {
    std::vector<Sign> signs;
    auto activeCategory = fCategories[fActiveCategory];
    if (activeCategory.name == "typing") {
      if (fTyping.isNotEmpty()) {
        auto search = U32StringFromJuceString(fTyping);
        for (auto const &it : SignList::Signs()) {
          if (!it.first.starts_with(search)) {
            continue;
          }
          if (auto found = fAllSigns.find(JuceStringFromU32String(it.first)); found != fAllSigns.end()) {
            signs.push_back(found->second);
          }
        }
        for (auto const &it : SignList::Special()) {
          if (!it.first.starts_with(search)) {
            continue;
          }
          auto code = SignList::CodeFromSign(it.second);
          if (!code) {
            continue;
          }
          if (auto found = fAllSigns.find(JuceStringFromU32String(*code)); found != fAllSigns.end()) {
            signs.push_back(found->second);
          }
        }
      }
    } else if (activeCategory.name == "tall" || activeCategory.name == "wide" || activeCategory.name == "small") {
      std::vector<juce::String> list;
      // clang-format off
      if (activeCategory.name == "tall") {
        list = {
          "M40", "Aa28", "Aa29", "P11", "D16", "T34", "T35", "U28", "U29", "U32", "U33", "S43", "U36", "T8",
          "T8A", "M13", "M17", "H6", "H6A", "M4", "M12", "S29", "M29", "M30", "S37", "R14", "R15", "R16", "R17", "P6", "S40",
          "R19", "S41", "F10", "F11", "F12", "S38", "S39", "T14", "T15", "T13", "Aa26", "O30", "Aa21", "U39", "F45", "O44",
          "Aa27", "R8", "R9", "T7A", "T3", "T4", "V24", "V25", "U23", "S42", "U34", "S36", "F28", "U26", "U27", "U24", "U25", "Y8",
          "F35", "F36", "U41", "W19", "P8", "T22", "T23", "Z11", "S44", "Aa25", "M44", "V38", "Aa31", "Aa30", "Aa20", "V36",
          "F31", "M32", "L7", "V17", "V18", "S34", "V39", "Q7", "T18", "T19", "T20", "R21", "R11", "O28", "O11", "O36", "Aa32",
          "V28", "V29",
        };
      } else if (activeCategory.name == "wide") {
        list = {
          "N1", "N37", "N38", "N39", "S32", "N18", "X4", "X5", "N17", "N16", "N20",
          "Aa10", "Aa11", "Aa12", "Aa13", "Aa14", "Aa15", "N35", "Aa8", "Aa9", "V26", "V27", "R24", "W8", "V32",
          "Y1", "Y2", "R4", "N11", "N12", "F42", "D24", "D25", "D13", "D15", "F20", "Z6", "F33", "T2",
          "T7", "F30", "V22", "V23", "R5", "R6", "O34", "V2", "V3", "S24", "R22", "R23", "T11", "O29",
          "T1", "T21", "U20", "U19", "U21", "D17", "U31", "T9", "T9A", "T10", "F32", "V13", "V14", "F46",
          "F47", "F48", "F49", "M11", "U17", "U18", "U14", "Aa7", "F18", "D51", "U15", "U16", "Aa24", "N31",
          "O31", "N36", "D14", "D21", "D22", "T30", "T31", "T33", "D48", "V30", "V31", "V31A", "W3", "S12",
          "N30", "O42", "O43", "V16"
        };
      } else if (activeCategory.name == "small") {
        list = {
          "Q3", "O39", "Z8", "O47", "N22", "N21", "N23", "N29", "X7", "O45", "O46", "Y6", "M35", "X3", "X2",
          "X1", "N28", "Aa17", "I6", "W10", "W10A", "Aa4", "R7", "M39", "M36", "F43", "F41", "N34", "U30", "W11", "W12",
          "W13", "T28", "N41", "N42", "V37", "M31", "F34", "W6", "W7", "W21", "W20", "V6", "V33", "V34", "V7", "V8", "S20",
          "V20", "V19", "Aa19", "Aa2", "Aa3", "N32", "F52", "V35", "H8", "M41", "F51", "D11", "K6", "L6", "F21", "D26", "N33",
          "D12", "S21", "N5", "N9", "N10", "Aa1", "O50", "O49", "O48", "X6", "V9", "S10", "N6", "N8", "S11", "N15", "M42",
          "F38", "V1", "Z7", "Aa16", "Z9", "Z10",
        };
      }
      // clang-format on
      for (auto const &key : list) {
        auto sign = fAllSigns.find(key);
        if (sign == fAllSigns.end()) {
          continue;
        }
        signs.push_back(sign->second);
      }
    } else {
      for (auto const &it : SignList::Signs()) {
        auto key = JuceStringFromU32String(it.first);
        if (key.length() < activeCategory.name.length() + 1 || !key.startsWith(activeCategory.name)) {
          continue;
        }
        auto ch = key.substring(activeCategory.name.length(), activeCategory.name.length() + 1).toRawUTF8()[0];
        if (ch < '0' || '9' < ch) {
          continue;
        }
        auto sign = fAllSigns.find(key);
        if (sign == fAllSigns.end()) {
          continue;
        }
        signs.push_back(sign->second);
      }
    }
    fContainer->setSigns(signs);
  }

  void setActiveCategory(int index) {
    if (fActiveCategory != index || index == 0) {
      fActiveCategory = index;
      if (index != 0) {
        fTyping.clear();
      }
      filterSigns();
      layout();
      repaint();
    }
  }

public:
  std::function<void(Sign const &)> onClickSign;

private:
  std::unique_ptr<juce::Viewport> fViewport;
  std::unique_ptr<SignListButtonContainer> fContainer;
  HbFontUniquePtr const &fFont;
  std::vector<Category> fCategories;
  std::vector<TabButton> fTabButtons;
  std::unordered_map<juce::String, Sign> fAllSigns;

  int fRows = 0;
  int fActiveCategory = 0;
  int fHitTabButton = -1;
  int fMouseDownCategory = -1;
  juce::String fTyping;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SignListComponent)
};

} // namespace ksesh
