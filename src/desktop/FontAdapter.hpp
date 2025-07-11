#pragma once

namespace ksesh {

class FontAdapter {
  struct Data {
    juce::Path path;
    int dx;
    int dy;
  };

public:
  explicit FontAdapter(std::shared_ptr<hb_font_t> const &font) : fFont(font) {
    juce::Path path = Harfbuzz::CreatePath(U"𓄿𓇋𓇌𓂝𓅱𓃀𓊪𓆑𓅓𓈖𓂋𓉔𓎛𓐍𓄡𓋴𓊃𓈙𓈎𓎡𓎼𓏏𓍿𓂧𓆓", font.get());
    auto bounds = path.getBounds();
    dy = bounds.getY();
    scale = 1.0f / bounds.getHeight();
  }

  juce::Path path(int glyphId, float height) const {
    auto p = Harfbuzz::CreatePath(glyphId, fFont.get());
    auto at = juce::AffineTransform().translated(0, dy).scaled(scale * height);
    p.applyTransform(at);
    return p;
  }

  juce::Path path(std::u32string const &text, float height) const {
    juce::Path ret;
    HbBufferUniquePtr buffer(Harfbuzz::CreateBuffer(text, fFont.get()));
    std::vector<GlyphInformation> glyphs;
    Harfbuzz::CreateGlyphInformations(buffer, fFont.get(), glyphs);
    auto at = juce::AffineTransform().translated(0, dy).scaled(scale * height);
    for (auto const &glyph : glyphs) {
      auto p = Harfbuzz::CreatePath(glyph.glyphId, fFont.get(), glyph.x, glyph.y);
      if (!p.isEmpty()) {
        p.applyTransform(at);
        ret.addPath(p);
      }
    }
    return ret;
  }

public:
  std::shared_ptr<hb_font_t> fFont;

private:
  float dy = 0;
  float scale = 1;
};

} // namespace ksesh
