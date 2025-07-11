#pragma once

namespace ksesh {

struct FontSet {
  std::shared_ptr<Font> fEgyptianText;
  std::shared_ptr<Font> fNewGardiner;
  std::shared_ptr<Font> fNotoSans;

  std::shared_ptr<Font> select(FontFamily family) const {
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
