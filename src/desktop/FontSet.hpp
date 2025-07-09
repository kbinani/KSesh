#pragma once

namespace ksesh {

struct FontSet {
  std::shared_ptr<hb_font_t> fEgyptianText;
  std::shared_ptr<hb_font_t> fNewGardiner;
  std::shared_ptr<hb_font_t> fNotoSans;

  std::shared_ptr<hb_font_t> select(FontFamily family) const {
    if (family == FontFamily::NewGardiner && fNewGardiner) {
      return fNewGardiner;
    } else if (family == FontFamily::NotoSans && fNotoSans) {
      return fNotoSans;
    } else {
      return fEgyptianText;
    }
  }
};

} // namespace ksesh
