#pragma once

namespace ksesh {

class HieroglyphComponent : public juce::Component {
public:
  explicit HieroglyphComponent(FontData const &font) : fFont(font) {
    hb_draw_funcs_t *funcs = hb_draw_funcs_create();
    hb_draw_funcs_set_move_to_func(
        funcs,
        [](auto *, void *data, auto *, float x, float y, auto *) {
          auto &path = *static_cast<juce::Path *>(data);
          path.startNewSubPath({x, y});
        },
        nullptr, nullptr);
    hb_draw_funcs_set_line_to_func(
        funcs,
        [](auto *, void *data, auto *, float x, float y, auto *) {
          auto &path = *static_cast<juce::Path *>(data);
          path.lineTo({x, y});
        },
        nullptr, nullptr);
    hb_draw_funcs_set_quadratic_to_func(
        funcs,
        [](auto *, void *data, auto *, float ctlX, float ctlY, float toX, float toY, auto *) {
          auto &path = *static_cast<juce::Path *>(data);
          path.quadraticTo({ctlX, ctlY}, {toX, toY});
        },
        nullptr, nullptr);
    hb_draw_funcs_set_cubic_to_func(
        funcs,
        [](auto *, void *data, auto *, float ctlX1, float ctlY1, float ctlX2, float ctlY2, float toX, float toY, auto *) {
          auto &path = *static_cast<juce::Path *>(data);
          path.cubicTo({ctlX1, ctlY1}, {ctlX2, ctlY2}, {toX, toY});
        },
        nullptr, nullptr);
    hb_draw_funcs_set_close_path_func(
        funcs,
        [](auto *, void *data, auto *, auto *) {
          auto &path = *static_cast<juce::Path *>(data);
          path.closeSubPath();
        },
        nullptr, nullptr);
    fDrawFuncs = funcs;
  }

  ~HieroglyphComponent() {
    hb_draw_funcs_destroy(fDrawFuncs);
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(juce::Colour(204, 204, 204));
    if (!fContent) {
      return;
    }
    g.setColour(juce::Colours::black);
    float fontSize = 48;
    float margin = 8;

    float scale = fontSize / (float)hb_face_get_upem(hb_font_get_face(fFont.hb.get()));
    g.addTransform(juce::AffineTransform::translation(margin, margin));
    g.addTransform(juce::AffineTransform::scale(scale, -scale));
    for (auto const &line : fContent->lines) {
      for (auto const &glyph : line->glyphs) {
        juce::Path path;
        hb_font_draw_glyph(fFont.hb.get(), glyph.glyphId, fDrawFuncs, &path);
        if (path.getBounds().isEmpty()) {
          continue;
        }
        g.saveState();
        g.addTransform(juce::AffineTransform::translation(glyph.x, glyph.y));
        g.fillPath(path);
        g.restoreState();
      }
    }
  }

  void setContent(std::shared_ptr<Content> const &c) {
    fContent = c;
    if (fContent) {
      repaint();
    }
  }

private:
  FontData const &fFont;
  std::shared_ptr<Content> fContent;
  hb_draw_funcs_t *fDrawFuncs;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HieroglyphComponent)
};

} // namespace ksesh
