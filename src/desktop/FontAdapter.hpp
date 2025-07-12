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
    juce::Path path = Harfbuzz::CreatePath(U"𓍹𓐼𓀀𓍹𓐼𓀀𓐽𓍺𓐽𓍺", font.get());
    auto bounds = path.getBounds();
    fDy = bounds.getY();
    fScale = 1.0f / bounds.getHeight();
  }

  juce::Path path(int glyphId, float height) const {
    auto p = Harfbuzz::CreatePath(glyphId, fFont.get());
    auto at = juce::AffineTransform().translated(0, fDy).scaled(fScale * height);
    p.applyTransform(at);
    return p;
  }

  juce::Path path(std::u32string const &text, float height) const {
    juce::Path ret;
    HbBufferUniquePtr buffer(Harfbuzz::CreateBuffer(text, fFont.get()));
    std::vector<GlyphInformation> glyphs;
    Harfbuzz::CreateGlyphInformations(buffer, fFont.get(), glyphs);
    auto at = juce::AffineTransform().translated(0, fDy).scaled(fScale * height);
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
  float fDy;
  float fScale;
};

} // namespace ksesh
