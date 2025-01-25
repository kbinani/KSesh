#pragma once

#pragma GCC diagnostic ignored "-Wunused-variable"

namespace ksesh {

class SignListButtonContainer : public juce::Component {
  struct SignButton {
    int x;
    int y;
    int width;
    int height;
    juce::String name;
    juce::String mdcFirst;
    juce::String mdcTrailing;
    std::shared_ptr<juce::Path> path;
  };

  enum : int {
    signButtonFontSize = 14,
    signButtonMdCFontSize = 12,
    signButtonHeaderHeight = 16,
    signButtonSignSize = 40,
    signButtonWidth = 44,
    signButtonSignHeight = 60 - signButtonHeaderHeight,
    signButtonMdCHeight = 16,
    signButtonMinMargin = 4,

    signButtonMdCMultipleLinesOffset = 4,
  };

public:
  explicit SignListButtonContainer(std::shared_ptr<hb_font_t> const &font) : fFont(font) {
    for (auto const &it : SignList::Signs()) {
      auto s = makeSign(it.first, it.second);
      fAllSigns[s->name] = s;
    }
  }

  void paint(juce::Graphics &g) override {
    auto textColor = getLookAndFeel().findColour(juce::TextEditor::textColourId);
    auto borderColor = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    auto highlightColor = getLookAndFeel().findColour(juce::TextEditor::highlightColourId);
    auto highlightTextColor = getLookAndFeel().findColour(juce::TextButton::textColourOnId);
    auto activeColor = getLookAndFeel().findColour(juce::TextButton::buttonOnColourId);

    float scale = 1.0f / Harfbuzz::UnitsPerEm(fFont);

    for (int i = 0; i < (int)fSignButtons.size(); i++) {
      auto const &sb = fSignButtons[i];
      if (i == fMouseDownSign) {
        g.setColour(activeColor);
        g.fillRect(sb.x, sb.y, sb.width, sb.height);
      } else if (i == fHitSignButton) {
        g.setColour(highlightColor);
        g.fillRect(sb.x, sb.y, sb.width, sb.height);
      }
      g.setFont(signButtonFontSize);
      if (i == fMouseDownSign) {
        g.setColour(highlightTextColor);
      } else {
        g.setColour(textColor);
      }
      if (i == fSelectedSign) {
        g.drawRect(sb.x, sb.y, sb.width, sb.height);
      }
      g.drawText(sb.name, juce::Rectangle<float>(sb.x, sb.y, sb.width, signButtonHeaderHeight).reduced(1), juce::Justification::centred);
      if (fShowMdC && sb.mdcFirst.isNotEmpty()) {
        g.setColour(textColor);
        g.setFont(signButtonMdCFontSize);
        auto base = juce::Rectangle<float>(sb.x, sb.y + signButtonHeaderHeight + signButtonSignHeight, sb.width, signButtonMdCHeight).reduced(1);
        if (sb.mdcTrailing.isEmpty()) {
          g.drawText(sb.mdcFirst, juce::Rectangle<float>(base.getCentreX() - signButtonWidth, base.getY(), 2 * signButtonWidth, base.getHeight()), juce::Justification::centred);
        } else {
          g.drawText(sb.mdcFirst, juce::Rectangle<float>(base.getCentreX() - signButtonWidth, signButtonMdCMultipleLinesOffset + base.getY() - signButtonMdCFontSize, 2 * signButtonWidth, base.getHeight()), juce::Justification::centred);
          g.drawText(sb.mdcTrailing, juce::Rectangle<float>(base.getCentreX() - signButtonWidth, signButtonMdCMultipleLinesOffset + base.getY(), 2 * signButtonWidth, base.getHeight()), juce::Justification::centred);
        }
      }
      g.saveState();
      auto bounds = sb.path->getBoundsTransformed(juce::AffineTransform::scale(scale * signButtonSignSize, scale * signButtonSignSize));
      float x = sb.x + sb.width * 0.5f - bounds.getWidth() * 0.5f - bounds.getX();
      float y = sb.y + signButtonHeaderHeight + signButtonSignHeight * 0.5f - bounds.getHeight() * 0.5f - bounds.getY();
      g.addTransform(juce::AffineTransform(scale * signButtonSignSize, 0, x, 0, scale * signButtonSignSize, y));
      g.fillPath(*sb.path);
      g.restoreState();
    }
  }

  void mouseEnter(juce::MouseEvent const &e) override {
    updateButtonHit(e.getPosition());
  }

  void mouseMove(juce::MouseEvent const &e) override {
    updateButtonHit(e.getPosition());
  }

  void mouseExit(juce::MouseEvent const &e) override {
    if (fHitSignButton >= 0) {
      fHitSignButton = -1;
      updateCursor();
      repaint();
    }
  }

  void mouseDown(juce::MouseEvent const &e) override {
    for (int i = 0; i < (int)fSignButtons.size(); i++) {
      auto const &sb = fSignButtons[i];
      if (juce::Rectangle<float>(sb.x, sb.y, sb.width, sb.height).contains(e.getPosition().toFloat())) {
        fMouseDownSign = i;
        updateCursor();
        repaint();
        return;
      }
    }
  }

  void mouseUp(juce::MouseEvent const &e) override {
    if (e.mods.isLeftButtonDown() && 0 <= fMouseDownSign && fMouseDownSign < (int)fSignButtons.size() && fMouseDownSign < (int)fSigns.size()) {
      auto const &sb = fSignButtons[fMouseDownSign];
      auto const &sign = fSigns[fMouseDownSign];
      if (juce::Rectangle<float>(sb.x, sb.y, sb.width, sb.height).contains(e.getPosition().toFloat()) && onClickSign) {
        onClickSign(*sign);
      }
    }
    fMouseDownSign = -1;
    updateCursor();
    repaint();
  }

  int layout(int width) {
    int y = 0;
    int x = 0;
    int maxY = 0;
    float scale = signButtonSignSize / (float)Harfbuzz::UnitsPerEm(fFont);
    fSignButtons.clear();
    float rowHeight = signButtonHeaderHeight + signButtonSignHeight;
    if (fShowMdC) {
      rowHeight += signButtonMdCHeight;
    }
    int maxNumColumns = 1;
    int columns = 0;
    for (auto const &it : fSigns) {
      auto bounds = it->path->getBoundsTransformed(juce::AffineTransform::scale(scale, scale));
      int buttonWidth = signButtonWidth;
      if (bounds.getWidth() + signButtonMinMargin * 2 > signButtonWidth) {
        buttonWidth = (int)ceil(bounds.getWidth() + signButtonMinMargin * 2);
      }
      columns += 1;
      if (x + buttonWidth > width) {
        x = 0;
        y += rowHeight + 1;
        columns = 1;
      }
      maxNumColumns = std::max(maxNumColumns, columns);
      SignButton sb;
      sb.x = x;
      sb.y = y;
      sb.width = buttonWidth;
      sb.height = rowHeight;
      sb.name = it->name;
      sb.path = it->path;
      sb.mdcFirst = it->mdcFirst;
      sb.mdcTrailing = it->mdcTrailing;
      fSignButtons.push_back(sb);
      x += buttonWidth + 1;
      maxY = y + rowHeight;
    }
    fNumColumns = maxNumColumns;
    setBounds(0, 0, width, maxY);
    return maxY;
  }

  void setShowMdC(bool show) {
    if (show == fShowMdC) {
      return;
    }
    fShowMdC = show;
    layout(getWidth());
  }

  bool isShowMdC() const {
    return fShowMdC;
  }

  void updateFilter(juce::String const &activeCategory, juce::String const &typing) {
    std::vector<std::shared_ptr<Sign>> signs;
    if (activeCategory == "typing") {
      if (typing.isNotEmpty()) {
        auto search = U32StringFromJuceString(typing);
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
    } else if (activeCategory == "tall" || activeCategory == "wide" || activeCategory == "small") {
      std::vector<juce::String> list;
      // clang-format off
      if (activeCategory == "tall") {
        list = {
          "M40", "Aa28", "Aa29", "P11", "D16", "T34", "T35", "U28", "U29", "U32", "U33", "S43", "U36", "T8",
          "T8A", "M13", "M17", "H6", "H6A", "M4", "M12", "S29", "M29", "M30", "S37", "R14", "R15", "R16", "R17", "P6", "S40",
          "R19", "S41", "F10", "F11", "F12", "S38", "S39", "T14", "T15", "T13", "Aa26", "O30", "Aa21", "U39", "F45", "O44",
          "Aa27", "R8", "R9", "T7A", "T3", "T4", "V24", "V25", "U23", "S42", "U34", "S36", "F28", "U26", "U27", "U24", "U25", "Y8",
          "F35", "F36", "U41", "W19", "P8", "T22", "T23", "Z11", "S44", "Aa25", "M44", "V38", "Aa31", "Aa30", "Aa20", "V36",
          "F31", "M32", "L7", "V17", "V18", "S34", "V39", "Q7", "T18", "T19", "T20", "R21", "R11", "O28", "O11", "O36", "Aa32",
          "V28", "V29",
        };
      } else if (activeCategory == "wide") {
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
      } else if (activeCategory == "small") {
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
        if (key.length() < activeCategory.length() + 1 || !key.startsWith(activeCategory)) {
          continue;
        }
        auto ch = key.substring(activeCategory.length(), activeCategory.length() + 1).toRawUTF8()[0];
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
    fSigns.swap(signs);
    layout(getWidth());
  }

  void moveSignSelection(int dx, int dy) {
    if (fSelectedSign < 0) {
      fSelectedSign = 0;
      repaint();
      return;
    }
    int const num = (int)fSignButtons.size();
    if (fSelectedSign == 0 && dx == -1 && dy == 0) {
      fSelectedSign = num - 1;
      repaint();
      return;
    }
    int const numRows = (int)ceil(num / (float)fNumColumns);
    int const y = fSelectedSign / fNumColumns;
    int const x = fSelectedSign - y * fNumColumns;
    int nx = x + dx;
    int ny = y + dy;
    if (x + dx >= fNumColumns) {
      nx = 0;
      ny = y + 1;
    } else if (x + dx < 0) {
      nx = fNumColumns - 1;
      ny = y - 1;
    } else if (y + dy < 0) {
      ny = numRows - 1;
      nx = x - 1;
    } else if (y + dy >= numRows) {
      ny = 0;
      nx = x + 1;
    }
    int n = ny * fNumColumns + nx;
    if (n == num && dx == 1 && dy == 0) {
      n = 0;
    } else if (n >= num && y < numRows && dy != 0) {
      if (dy > 0 && x + 1 >= fNumColumns) {
        ny = 0;
        nx = 0;
      } else if (dy < 0) {
        ny = numRows - 2;
        nx = x - 1;
      } else {
        ny = 0;
        nx = x + 1;
      }
      n = ny * fNumColumns + nx;
    }
    int next = n;
    if (n == num) {
      next = 0;
    } else if (n < 0) {
      next = num - 1;
      return;
    } else if (n >= num) {
      next = 0;
      return;
    }
    fSelectedSign = next;
    repaint();
  }

  void resetSignSelection() {
    fSelectedSign = -1;
    repaint();
  }

private:
  std::shared_ptr<Sign> makeSign(std::u32string const &name, std::u32string const &sign) {
    auto s = std::make_shared<Sign>();
    s->name = JuceStringFromU32String(name);
    s->path = std::make_shared<juce::Path>();
    *s->path = Harfbuzz::CreatePath(sign, fFont);

    auto found = SignList::MapReverse(sign);
    std::vector<juce::String> mdc;
    mdc.reserve(found.size());
    for (size_t i = 0; i < found.size(); i++) {
      mdc.push_back(JuceStringFromU32String(found[i]));
    }
    if (mdc.size() == 1) {
      s->mdcFirst = mdc[0];
    } else if (mdc.size() > 1) {
      for (int i = 0; i < (int)mdc.size(); i++) {
        if (i < 2) {
          if (i > 0) {
            s->mdcFirst += ",";
          }
          s->mdcFirst += mdc[i];
        } else {
          if (i > 2) {
            s->mdcTrailing += ",";
          }
          s->mdcTrailing += mdc[i];
        }
      }
    }
    return s;
  }

  void updateButtonHit(juce::Point<int> const &p) {
    int hitTabButton = -1;
    int hitSignButton = -1;
    for (int i = 0; i < (int)fSignButtons.size(); i++) {
      auto const &sb = fSignButtons[i];
      if (juce::Rectangle<float>(sb.x, sb.y, sb.width, sb.height).contains(p.toFloat())) {
        hitSignButton = i;
        break;
      }
    }
    if (fHitSignButton != hitSignButton) {
      fHitSignButton = hitSignButton;
      repaint();
    }
    updateCursor();
  }

  void updateCursor() {
    if (fHitSignButton >= 0 || fMouseDownSign >= 0) {
      setMouseCursor(juce::MouseCursor::PointingHandCursor);
    } else {
      setMouseCursor(juce::MouseCursor::NormalCursor);
    }
  }

public:
  std::function<void(Sign const &)> onClickSign;

private:
  std::shared_ptr<hb_font_t> fFont;
  std::vector<std::shared_ptr<Sign>> fSigns;
  std::vector<SignButton> fSignButtons;
  int fHitSignButton = -1;
  int fMouseDownSign = -1;
  int fSelectedSign = -1;
  int fNumColumns = 1;
  bool fShowMdC = false;
  std::unordered_map<juce::String, std::shared_ptr<Sign>> fAllSigns;
};

} // namespace ksesh
