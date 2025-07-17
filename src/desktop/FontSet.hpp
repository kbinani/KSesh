#pragma once

namespace ksesh {

struct FontSet {
  std::shared_ptr<FontAdapter> fEgyptianText;
  std::shared_ptr<FontAdapter> fNewGardiner;
  std::shared_ptr<FontAdapter> fNotoSans;

  std::shared_ptr<FontAdapter> select(FontFamily family) const {
    if (family == FontFamily::NewGardiner && fNewGardiner) {
      return fNewGardiner;
    } else if (family == FontFamily::NotoSans && fNotoSans) {
      return fNotoSans;
    } else {
      return fEgyptianText;
    }
  }

  bool ok() const {
    return fEgyptianText && fNewGardiner && fNotoSans;
  }
};

} // namespace ksesh
