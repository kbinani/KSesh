#pragma once

#include <juce_graphics/fonts/harfbuzz/hb.hh>
#include <juce_graphics/fonts/juce_FunctionPointerDestructor.h>

namespace ksesh {

using HbBlobUniquePtr = std::unique_ptr<hb_blob_t, juce::FunctionPointerDestructor<hb_blob_destroy>>;
using HbFaceUniquePtr = std::unique_ptr<hb_face_t, juce::FunctionPointerDestructor<hb_face_destroy>>;
using HbFontUniquePtr = std::unique_ptr<hb_font_t, juce::FunctionPointerDestructor<hb_font_destroy>>;
using HbBufferUniquePtr = std::unique_ptr<hb_buffer_t, juce::FunctionPointerDestructor<hb_buffer_destroy>>;
using HbDrawFuncsUniquePtr = std::unique_ptr<hb_draw_funcs_t, juce::FunctionPointerDestructor<hb_draw_funcs_destroy>>;

class Harfbuzz {
  Harfbuzz() = delete;

  struct Data {
    juce::Path path;
    int dx;
    int dy;
  };

  static hb_draw_funcs_t *CreateDrawFuncs() {
    hb_draw_funcs_t *funcs = hb_draw_funcs_create();
    hb_draw_funcs_set_move_to_func(
        funcs,
        [](auto *, void *data, auto *, float x, float y, auto *) {
          auto &d = *static_cast<Data *>(data);
          d.path.startNewSubPath({x + d.dx, -y + d.dy});
        },
        nullptr, nullptr);
    hb_draw_funcs_set_line_to_func(
        funcs,
        [](auto *, void *data, auto *, float x, float y, auto *) {
          auto &d = *static_cast<Data *>(data);
          d.path.lineTo({x + d.dx, -y + d.dy});
        },
        nullptr, nullptr);
    hb_draw_funcs_set_quadratic_to_func(
        funcs,
        [](auto *, void *data, auto *, float ctlX, float ctlY, float toX, float toY, auto *) {
          auto &d = *static_cast<Data *>(data);
          d.path.quadraticTo({ctlX + d.dx, -ctlY + d.dy}, {toX + d.dx, -toY + d.dy});
        },
        nullptr, nullptr);
    hb_draw_funcs_set_cubic_to_func(
        funcs,
        [](auto *, void *data, auto *, float ctlX1, float ctlY1, float ctlX2, float ctlY2, float toX, float toY, auto *) {
          auto &d = *static_cast<Data *>(data);
          d.path.cubicTo({ctlX1 + d.dx, -ctlY1 + d.dy}, {ctlX2 + d.dx, -ctlY2 + d.dy}, {toX + d.dx, -toY + d.dy});
        },
        nullptr, nullptr);
    hb_draw_funcs_set_close_path_func(
        funcs,
        [](auto *, void *data, auto *, auto *) {
          auto &d = *static_cast<Data *>(data);
          d.path.closeSubPath();
        },
        nullptr, nullptr);
    return funcs;
  }

  static hb_draw_funcs_t *GetDrawFuncs() {
    static HbDrawFuncsUniquePtr s(CreateDrawFuncs());
    return s.get();
  }

public:
  static juce::Path CreatePath(int glyphId, HbFontUniquePtr const &font, int dx = 0, int dy = 0) {
    Data data;
    data.dx = dx;
    data.dy = dy;
    hb_font_draw_glyph(font.get(), glyphId, GetDrawFuncs(), &data);
    return data.path;
  }

  static unsigned int UnitsPerEm(HbFontUniquePtr const &font) {
    return hb_face_get_upem(hb_font_get_face(font.get()));
  }
};

} // namespace ksesh