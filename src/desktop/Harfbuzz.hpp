#pragma once

#pragma GCC diagnostic push

#include <juce_graphics/fonts/harfbuzz/hb.hh>
#include <juce_graphics/fonts/juce_FunctionPointerDestructor.h>

#pragma GCC diagnostic pop

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
  hb_codepoint_t fGlyphId;
  hb_position_t fX;
  hb_position_t fY;
  uint32_t fCluster;
};

class Harfbuzz {
  Harfbuzz() = delete;

  struct Data {
    juce::Path fPath;
    int fDx;
    int fDy;
  };

  static hb_draw_funcs_t *CreateDrawFuncs() {
    hb_draw_funcs_t *funcs = hb_draw_funcs_create();
    hb_draw_funcs_set_move_to_func(
        funcs,
        [](auto *, void *data, auto *, float x, float y, auto *) {
          auto &d = *static_cast<Data *>(data);
          d.fPath.startNewSubPath({x + d.fDx, -y + d.fDy});
        },
        nullptr, nullptr);
    hb_draw_funcs_set_line_to_func(
        funcs,
        [](auto *, void *data, auto *, float x, float y, auto *) {
          auto &d = *static_cast<Data *>(data);
          d.fPath.lineTo({x + d.fDx, -y + d.fDy});
        },
        nullptr, nullptr);
    hb_draw_funcs_set_quadratic_to_func(
        funcs,
        [](auto *, void *data, auto *, float ctlX, float ctlY, float toX, float toY, auto *) {
          auto &d = *static_cast<Data *>(data);
          d.fPath.quadraticTo({ctlX + d.fDx, -ctlY + d.fDy}, {toX + d.fDx, -toY + d.fDy});
        },
        nullptr, nullptr);
    hb_draw_funcs_set_cubic_to_func(
        funcs,
        [](auto *, void *data, auto *, float ctlX1, float ctlY1, float ctlX2, float ctlY2, float toX, float toY, auto *) {
          auto &d = *static_cast<Data *>(data);
          d.fPath.cubicTo({ctlX1 + d.fDx, -ctlY1 + d.fDy}, {ctlX2 + d.fDx, -ctlY2 + d.fDy}, {toX + d.fDx, -toY + d.fDy});
        },
        nullptr, nullptr);
    hb_draw_funcs_set_close_path_func(
        funcs,
        [](auto *, void *data, auto *, auto *) {
          auto &d = *static_cast<Data *>(data);
          d.fPath.closeSubPath();
        },
        nullptr, nullptr);
    return funcs;
  }

  static hb_draw_funcs_t *GetDrawFuncs() {
    static HbDrawFuncsUniquePtr s(CreateDrawFuncs());
    return s.get();
  }

public:
  static juce::Path CreatePath(int glyphId, hb_font_t *font, juce::AffineTransform const &tx) {
    Data data;
    data.fDx = 0;
    data.fDy = 0;
    hb_font_draw_glyph(font, glyphId, GetDrawFuncs(), &data);
    data.fPath.applyTransform(tx);
    return data.fPath;
  }

  static juce::Path CreatePath(int glyphId, hb_font_t *font, int dx = 0, int dy = 0) {
    Data data;
    data.fDx = dx;
    data.fDy = dy;
    hb_font_draw_glyph(font, glyphId, GetDrawFuncs(), &data);
    return data.fPath;
  }

  static juce::Path CreatePath(std::u32string const &t, hb_font_t *font) {
    juce::Path ret;
    HbBufferUniquePtr buffer(CreateBuffer(t, font));
    std::vector<GlyphInformation> glyphs;
    CreateGlyphInformations(buffer, font, glyphs);
    for (auto const &glyph : glyphs) {
      auto p = CreatePath(glyph.fGlyphId, font, glyph.fX, glyph.fY);
      if (!p.isEmpty()) {
        ret.addPath(p);
      }
    }
    return ret;
  }

  static hb_buffer_t *CreateBuffer(std::u32string const &t, hb_font_t *font) {
    HbBufferUniquePtr buffer(hb_buffer_create());
    hb_buffer_add_utf32(buffer.get(), (uint32_t const *)t.c_str(), t.size(), 0, -1);
    hb_buffer_set_direction(buffer.get(), HB_DIRECTION_LTR);
    hb_buffer_set_script(buffer.get(), HB_SCRIPT_EGYPTIAN_HIEROGLYPHS);
    hb_buffer_set_cluster_level(buffer.get(), HB_BUFFER_CLUSTER_LEVEL_CHARACTERS);
    hb_shape(font, buffer.get(), nullptr, 0);

    return buffer.release();
  }

  static void CreateGlyphInformations(HbBufferUniquePtr const &buffer, hb_font_t *font, std::vector<GlyphInformation> &out) {
    unsigned int const numGlyphs = hb_buffer_get_length(buffer.get());
    hb_glyph_info_t *glyphInfo = hb_buffer_get_glyph_infos(buffer.get(), nullptr);
    hb_glyph_position_t *glyphPos = hb_buffer_get_glyph_positions(buffer.get(), nullptr);
    hb_position_t cursorX = 0;
    hb_position_t cursorY = 0;
    for (unsigned int i = 0; i < numGlyphs; i++) {
      GlyphInformation info;
      info.fGlyphId = glyphInfo[i].codepoint;
      info.fCluster = glyphInfo[i].cluster;
      auto xOffset = glyphPos[i].x_offset;
      auto yOffset = glyphPos[i].y_offset;
      auto xAdvance = glyphPos[i].x_advance;
      auto yAdvance = glyphPos[i].y_advance;
      info.fX = cursorX + xOffset;
      info.fY = -(cursorY + yOffset);
      out.push_back(info);
      cursorX += xAdvance;
      cursorY += yAdvance;
    }
  }
};

} // namespace ksesh
