#pragma once

namespace ksesh {

class PDFExporter {
  PDFExporter() = delete;

public:
  static std::string ToPDF(Document const &document, PresentationSetting const &setting) {
    using namespace std;
    float const fontSize = setting.fFontSize;
    float const padding = setting.fPadding;
    auto font = document.fFont.lock();
    if (!font) {
      return {};
    }
    auto [width, height] = document.getSize(setting);
    struct Data {
      float fScale;
      float fDx;
      float fDy;
      float fTx;
      float fTy;
      vector<pdf_path_operation> fBuffer;
      float x(float v) const {
        return (v + fTx) * fScale + fDx;
      }
      float y(float v) const {
        return (v + fTy) * fScale + fDy;
      }
      float fCurrentX = 0;
      float fCurrentY = 0;
    };
    HbDrawFuncsUniquePtr funcs(hb_draw_funcs_create());
    hb_draw_funcs_set_move_to_func(
        funcs.get(),
        [](auto *, void *data, auto *, float x, float y, auto *) {
          auto &d = *static_cast<Data *>(data);
          pdf_path_operation op;
          op.op = 'm';
          op.x1 = d.x(x);
          op.y1 = d.y(y);
          d.fBuffer.push_back(op);
          d.fCurrentX = x;
          d.fCurrentY = y;
        },
        nullptr, nullptr);
    hb_draw_funcs_set_line_to_func(
        funcs.get(),
        [](auto *, void *data, auto *, float x, float y, auto *) {
          auto &d = *static_cast<Data *>(data);
          pdf_path_operation op;
          op.op = 'l';
          op.x1 = d.x(x);
          op.y1 = d.y(y);
          d.fBuffer.push_back(op);
          d.fCurrentX = x;
          d.fCurrentY = y;
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
          pdf_path_operation op;
          op.op = 'c';
          op.x3 = d.x(toX);
          op.y3 = d.y(toY);
          op.x1 = d.x(xc1);
          op.y1 = d.y(yc1);
          op.x2 = d.x(xc2);
          op.y2 = d.y(yc2);
          d.fBuffer.push_back(op);
          d.fCurrentX = toX;
          d.fCurrentY = toY;
        },
        nullptr, nullptr);
    hb_draw_funcs_set_cubic_to_func(
        funcs.get(),
        [](auto *, void *data, auto *, float ctlX1, float ctlY1, float ctlX2, float ctlY2, float toX, float toY, auto *) {
          auto &d = *static_cast<Data *>(data);
          pdf_path_operation op;
          op.op = 'c';
          op.x3 = d.x(toX);
          op.y3 = d.y(toY);
          op.x1 = d.x(ctlX1);
          op.y1 = d.y(ctlY1);
          op.x2 = d.x(ctlX2);
          op.y2 = d.y(ctlY2);
          d.fBuffer.push_back(op);
          d.fCurrentX = toX;
          d.fCurrentY = toY;
        },
        nullptr, nullptr);
    hb_draw_funcs_set_close_path_func(
        funcs.get(),
        [](auto *, void *data, auto *, auto *) {
          auto &d = *static_cast<Data *>(data);
          pdf_path_operation op;
          op.op = 'h';
          d.fBuffer.push_back(op);
        },
        nullptr, nullptr);

    unique_ptr<pdf_doc, juce::FunctionPointerDestructor<pdf_destroy>> doc(pdf_create(width, height, nullptr));
    pdf_object *page = pdf_append_page(doc.get());

    int lineIndex = 0;
    for (auto const &line : document.fLines) {
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
        float y = cursorY + yOffset;

        Data data;
        data.fScale = fontSize * font->fScale;
        data.fTx = x;
        // https://gyazo.com/1aae4ad48ead791b3daeaf1e4c7146d1
        data.fTy = y + 1 / font->fScale + font->fY;
        data.fDx = padding;
        data.fDy = height - lineIndex * (fontSize + setting.lineSpacing()) - padding - fontSize;
        data.fBuffer.push_back({.op = 'm', .x1 = 0, .y1 = 0});
        hb_font_draw_glyph(font->fFont.get(), glyphId, funcs.get(), &data);
        if (data.fBuffer.size() > 1) {
          pdf_add_custom_path(doc.get(), page, data.fBuffer.data(), data.fBuffer.size(), 0, 0, PDF_BLACK);
        }
        cursorX += xAdvance;
        cursorY += yAdvance;
      }
      lineIndex++;
    }
    string out;
    pdf_save_stream(doc.get(), out);
    return out;
  }
};

} // namespace ksesh
