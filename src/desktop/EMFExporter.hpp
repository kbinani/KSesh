#pragma once

namespace ksesh {

#if defined(JUCE_WINDOWS)
class EMFExporter {
  EMFExporter() = delete;

public:
  static std::string ToEMF(Document const &doc, PresentationSetting const &setting) {
    using namespace std;
    string out;
    auto font = doc.fFont.lock();
    if (!font) {
      return out;
    }
    auto [widthf, heightf] = doc.getSize(setting);
    LONG width = (LONG)ceil(widthf);
    LONG height = (LONG)ceil(heightf);
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = width;
    rc.bottom = height;
    HDC hdc = ::CreateEnhMetaFile(nullptr, nullptr, nullptr, nullptr);
    if (INVALID_HANDLE_VALUE == hdc) {
      return out;
    }
    ::SetGraphicsMode(hdc, GM_ADVANCED);
    ::SetWindowOrgEx(hdc, 0, 0, nullptr);
    ::SetWindowExtEx(hdc, width, height, nullptr);

    struct Data {
      HDC fHdc;
      int fDx = 0;
      int fDy = 0;
      float fTy = 0;
      float fTx = 0;
      float fCurrentX = 0;
      float fCurrentY = 0;
      int x(float v) const {
        return (int)round(v + fTx) + fDx;
      }
      int y(float v) const {
        return (int)round(-v + fTy) + fDy;
      }
      void current(float x, float y) {
        fCurrentX = x;
        fCurrentY = y;
      }
      void begin() {
        if (fBegan) {
          return;
        }
        ::BeginPath(fHdc);
        fBegan = true;
      }
      bool end() {
        if (!fBegan) {
          return false;
        }
        ::EndPath(fHdc);
        return true;
      }
      bool fBegan = false;
    };
    HbDrawFuncsUniquePtr funcs(hb_draw_funcs_create());
    hb_draw_funcs_set_move_to_func(
        funcs.get(),
        [](auto *, void *data, auto *, float x, float y, auto *) {
          auto &d = *static_cast<Data *>(data);
          d.begin();
          ::MoveToEx(d.fHdc, d.x(x), d.y(y), nullptr);
          d.current(x, y);
        },
        nullptr, nullptr);
    hb_draw_funcs_set_line_to_func(
        funcs.get(),
        [](auto *, void *data, auto *, float x, float y, auto *) {
          auto &d = *static_cast<Data *>(data);
          d.begin();
          ::LineTo(d.fHdc, d.x(x), d.y(y));
          d.current(x, y);
        },
        nullptr, nullptr);
    hb_draw_funcs_set_quadratic_to_func(
        funcs.get(),
        [](auto *, void *data, auto *, float ctlX, float ctlY, float toX, float toY, auto *) {
          auto &d = *static_cast<Data *>(data);
          float xc1 = d.fCurrentX + (ctlX - d.fCurrentX) * (2.0f / 3.0f);
          float yc1 = d.fCurrentY + (ctlY - d.fCurrentY) * (2.0f / 3.0f);
          float xc2 = toX + (ctlX - toX) * (2.0f / 3.0f);
          float yc2 = toY + (ctlY - toY) * (2.0f / 3.0f);
          POINT pt[3] = {
              {d.x(xc1), d.y(yc1)},
              {d.x(xc2), d.y(yc2)},
              {d.x(toX), d.y(toY)},
          };
          d.begin();
          ::PolyBezierTo(d.fHdc, pt, 3);
          d.current(toX, toY);
        },
        nullptr, nullptr);
    hb_draw_funcs_set_cubic_to_func(
        funcs.get(),
        [](auto *, void *data, auto *, float ctlX1, float ctlY1, float ctlX2, float ctlY2, float toX, float toY, auto *) {
          auto &d = *static_cast<Data *>(data);
          POINT pt[3] = {
              {d.x(ctlX1), d.y(ctlY1)},
              {d.x(ctlX2), d.y(ctlY2)},
              {d.x(toX), d.y(toY)},
          };
          d.begin();
          ::PolyBezierTo(d.fHdc, pt, 3);
          d.current(toX, toY);
        },
        nullptr, nullptr);
    hb_draw_funcs_set_close_path_func(
        funcs.get(),
        [](auto *, void *data, auto *, auto *) {
          auto &d = *static_cast<Data *>(data);
          d.begin();
          ::CloseFigure(d.fHdc);
        },
        nullptr, nullptr);
    {
      ScopedHANDLE<HBRUSH, ::DeleteObject> brush(::CreateSolidBrush(RGB(0, 0, 0)));
      ::SelectObject(hdc, brush);

      float const fontSize = setting.fFontSize;
      float const padding = setting.fPadding;
      float dy = 0;

      ::ModifyWorldTransform(hdc, nullptr, MWT_IDENTITY);
      XFORM mtx;
      mtx.eM11 = font->fScale * fontSize;
      mtx.eM12 = 0;
      mtx.eM21 = 0;
      mtx.eM22 = font->fScale * fontSize;
      mtx.eDx = padding;
      mtx.eDy = padding;
      ::SetWorldTransform(hdc, &mtx);

      for (int lineIndex = 0; lineIndex < (int)doc.fLines.size(); lineIndex++) {
        auto const &line = doc.fLines[lineIndex];
        unsigned int numGlyphs = hb_buffer_get_length(line->fBuffer.get());
        hb_glyph_info_t *glyphInfo = hb_buffer_get_glyph_infos(line->fBuffer.get(), nullptr);
        hb_glyph_position_t *glyphPos = hb_buffer_get_glyph_positions(line->fBuffer.get(), nullptr);
        float cursorX = 0;
        float cursorY = 0;
        for (unsigned int i = 0; i < numGlyphs; i++) {
          auto glyphId = glyphInfo[i].codepoint;
          auto xOffset = glyphPos[i].x_offset;
          auto yOffset = glyphPos[i].y_offset;
          auto xAdvance = glyphPos[i].x_advance;
          auto yAdvance = glyphPos[i].y_advance;
          float x = cursorX + xOffset;
          float y = -(cursorY + yOffset - font->fY);

          Data data;
          data.fHdc = hdc;
          data.fTx = x;
          data.fTy = y;
          data.fDx = 0;
          data.fDy = dy;
          hb_font_draw_glyph(font->fFont.get(), glyphId, funcs.get(), &data);
          if (data.end()) {
            ::FillPath(hdc);
          }
          cursorX += xAdvance;
          cursorY += yAdvance;
        }
        dy += (setting.lineSpacing() + fontSize) / (font->fScale * fontSize);
      }
    }

    ScopedHANDLE<HENHMETAFILE, ::DeleteEnhMetaFile> file(::CloseEnhMetaFile(hdc));
    if (!file) {
      return out;
    }
    auto size = ::GetEnhMetaFileBits(file, 0, nullptr);
    if (size == 0) {
      return out;
    }
    string s;
    s.resize(size);
    if (::GetEnhMetaFileBits(file, s.size(), (LPBYTE)s.data()) != size) {
      return out;
    }
    s.swap(out);
    return out;
  }
};
#endif

} // namespace ksesh
