#pragma once

#include <juce_graphics/fonts/harfbuzz/hb.hh>
#include <juce_graphics/fonts/juce_FunctionPointerDestructor.h>

namespace ksesh {

using HbBlobUniquePtr = std::unique_ptr<hb_blob_t, juce::FunctionPointerDestructor<hb_blob_destroy>>;
using HbFaceUniquePtr = std::unique_ptr<hb_face_t, juce::FunctionPointerDestructor<hb_face_destroy>>;
using HbFontUniquePtr = std::unique_ptr<hb_font_t, juce::FunctionPointerDestructor<hb_font_destroy>>;
using HbBufferUniquePtr = std::unique_ptr<hb_buffer_t, juce::FunctionPointerDestructor<hb_buffer_destroy>>;
using HbDrawFuncsUniquePtr = std::unique_ptr<hb_draw_funcs_t, juce::FunctionPointerDestructor<hb_draw_funcs_destroy>>;

inline std::shared_ptr<hb_font_t> HbMakeSharedFontPtr(hb_font_t *ptr) {
  return std::shared_ptr<hb_font_t>(ptr, hb_font_destroy);
}

struct GlyphInformation {
  hb_codepoint_t glyphId;
  hb_position_t x;
  hb_position_t y;
  uint32_t cluster;
};

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
  static juce::Path CreatePath(int glyphId, std::shared_ptr<hb_font_t> const &font, int dx = 0, int dy = 0) {
    Data data;
    data.dx = dx;
    data.dy = dy;
    hb_font_draw_glyph(font.get(), glyphId, GetDrawFuncs(), &data);
    return data.path;
  }

  static juce::Path CreatePath(std::u32string const &t, std::shared_ptr<hb_font_t> const &font) {
    juce::Path ret;
    HbBufferUniquePtr buffer(CreateBuffer(t, font));
    std::vector<GlyphInformation> glyphs;
    CreateGlyphInformations(buffer, font, glyphs);
    for (auto const &glyph : glyphs) {
      auto p = CreatePath(glyph.glyphId, font, glyph.x, glyph.y);
      if (!p.isEmpty()) {
        ret.addPath(p);
      }
    }
    return ret;
  }

  static hb_buffer_t *CreateBuffer(std::u32string const &t, std::shared_ptr<hb_font_t> const &font) {
    std::u8string utf8 = U8StringFromU32String(t);

    HbBufferUniquePtr buffer(hb_buffer_create());
    hb_buffer_add_utf8(buffer.get(), (char const *)utf8.c_str(), -1, 0, -1);
    hb_buffer_set_direction(buffer.get(), HB_DIRECTION_LTR);
    hb_buffer_set_script(buffer.get(), HB_SCRIPT_EGYPTIAN_HIEROGLYPHS);
    hb_buffer_set_cluster_level(buffer.get(), HB_BUFFER_CLUSTER_LEVEL_CHARACTERS);
    hb_shape(font.get(), buffer.get(), nullptr, 0);

    return buffer.release();
  }

  static void CreateGlyphInformations(HbBufferUniquePtr const &buffer, std::shared_ptr<hb_font_t> const &font, std::vector<GlyphInformation> &out) {
    out.clear();

    hb_font_extents_t extents{};
    hb_font_get_h_extents(font.get(), &extents);
    auto descender = extents.descender;
    int unitsPerEm = hb_face_get_upem(hb_font_get_face(font.get()));

    unsigned int numGlyphs = hb_buffer_get_length(buffer.get());
    hb_glyph_info_t *glyphInfo = hb_buffer_get_glyph_infos(buffer.get(), nullptr);
    hb_glyph_position_t *glyphPos = hb_buffer_get_glyph_positions(buffer.get(), nullptr);
    hb_position_t cursorX = 0;
    hb_position_t cursorY = -(unitsPerEm + descender);
    for (unsigned int i = 0; i < numGlyphs; i++) {
      GlyphInformation info;
      info.glyphId = glyphInfo[i].codepoint;
      info.cluster = glyphInfo[i].cluster;
      auto xOffset = glyphPos[i].x_offset;
      auto yOffset = glyphPos[i].y_offset;
      auto xAdvance = glyphPos[i].x_advance;
      auto yAdvance = glyphPos[i].y_advance;
      info.x = cursorX + xOffset;
      info.y = -(cursorY + yOffset);
      out.push_back(info);
      cursorX += xAdvance;
      cursorY += yAdvance;
    }
  }

  static unsigned int UnitsPerEm(std::shared_ptr<hb_font_t> const &font) {
    return hb_face_get_upem(hb_font_get_face(font.get()));
  }
};

} // namespace ksesh
