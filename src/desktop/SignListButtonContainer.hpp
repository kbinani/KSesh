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
    juce::String mcdFirst;
    juce::String mcdTrailing;
    std::shared_ptr<juce::Path> path;
  };

  enum : int {
    signButtonFontSize = 14,
    signButtonMcDFontSize = 12,
    signButtonHeaderHeight = 16,
    signButtonSignSize = 40,
    signButtonWidth = 44,
    signButtonSignHeight = 60 - 16,
    signButtonMcDHeight = 16,
    signButtonMinMargin = 4,
  };

public:
  explicit SignListButtonContainer(HbFontUniquePtr const &font) : fFont(font) {}

  void setSigns(std::vector<Sign> const &signs) {
    fSigns = signs;
    layout(getWidth());
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
      g.drawText(sb.name, juce::Rectangle<float>(sb.x, sb.y, sb.width, signButtonHeaderHeight).reduced(1), juce::Justification::centred);
      if (fShowMcD && sb.mcdFirst.isNotEmpty()) {
        g.setColour(textColor);
        g.setFont(signButtonMcDFontSize);
        auto base = juce::Rectangle<float>(sb.x, sb.y + signButtonHeaderHeight + signButtonSignHeight, sb.width, signButtonMcDHeight).reduced(1);
        if (sb.mcdTrailing.isEmpty()) {
          g.drawText(sb.mcdFirst, juce::Rectangle<float>(base.getCentreX() - signButtonWidth, base.getY(), 2 * signButtonWidth, base.getHeight()), juce::Justification::centred);
        } else {
          g.drawText(sb.mcdFirst, juce::Rectangle<float>(base.getCentreX() - signButtonWidth, base.getY() - signButtonMcDFontSize, 2 * signButtonWidth, base.getHeight()), juce::Justification::centred);
          g.drawText(sb.mcdTrailing, juce::Rectangle<float>(base.getCentreX() - signButtonWidth, base.getY(), 2 * signButtonWidth, base.getHeight()), juce::Justification::centred);
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
        onClickSign(sign);
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
    if (fShowMcD) {
      rowHeight += signButtonMcDHeight;
    }
    for (auto const &it : fSigns) {
      auto bounds = it.path->getBoundsTransformed(juce::AffineTransform::scale(scale, scale));
      int buttonWidth = signButtonWidth;
      if (bounds.getWidth() + signButtonMinMargin * 2 > signButtonWidth) {
        buttonWidth = (int)ceil(bounds.getWidth() + signButtonMinMargin * 2);
      }
      if (x + buttonWidth > width) {
        x = 0;
        y += rowHeight + 1;
      }
      SignButton sb;
      sb.x = x;
      sb.y = y;
      sb.width = buttonWidth;
      sb.height = rowHeight;
      sb.name = it.name;
      sb.path = it.path;
      if (it.mcd.size() == 1) {
        sb.mcdFirst = it.mcd[0];
      } else if (it.mcd.size() > 1) {
        for (int i = 0; i < it.mcd.size(); i++) {
          if (i < 2) {
            if (i > 0) {
              sb.mcdFirst += ",";
            }
            sb.mcdFirst += it.mcd[i];
          } else {
            if (i > 2) {
              sb.mcdTrailing += ",";
            }
            sb.mcdTrailing += it.mcd[i];
          }
        }
      }
      fSignButtons.push_back(sb);
      x += buttonWidth + 1;
      maxY = y + rowHeight;
    }
    setBounds(0, 0, width, maxY);
    return maxY;
  }

  void setShowMcD(bool show) {
    if (show == fShowMcD) {
      return;
    }
    fShowMcD = show;
    layout(getWidth());
  }

private:
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
  HbFontUniquePtr const &fFont;
  std::vector<Sign> fSigns;
  std::vector<SignButton> fSignButtons;
  int fHitSignButton = -1;
  int fMouseDownSign = -1;
  bool fShowMcD = true;
};

} // namespace ksesh
