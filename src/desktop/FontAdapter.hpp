#pragma once

namespace ksesh {

class FontAdapter {
public:
  explicit FontAdapter(std::shared_ptr<hb_font_t> const &font) : fFont(font) {
    juce::Path path = Harfbuzz::CreatePath(U"𓍹𓐼𓀀𓍹𓐼𓀀𓐽𓍺𓐽𓍺", font.get());
    auto bounds = path.getBounds();
    fY = bounds.getY();
    fScale = 1.0f / bounds.getHeight();
  }

public:
  std::shared_ptr<hb_font_t> fFont;
  float fY;
  float fScale;
};

} // namespace ksesh
