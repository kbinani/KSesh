import { insertions } from "../src/web/insertions";

console.log(`#pragma once

namespace ksesh {

// clang-format off
class Insertions {
public:
  enum Type : uint8_t {
    BottomStart = 0b1,
    TopStart = 0b10,
    BottomEnd = 0b100,
    TopEnd = 0b1000,
  };
  static uint8_t InsertionType(std::u32string const &sign) {
    using namespace std::literals::string_literals;`);

for (const key of Object.keys(insertions)) {
  let v = 0;
  const item = insertions[key];
  if (item.bs !== undefined) {
    v |= 1;
  }
  if (item.ts !== undefined) {
    v |= 2;
  }
  if (item.be !== undefined) {
    v |= 4;
  }
  if (item.te !== undefined) {
    v |= 8;
  }
  if (v === 0) {
    continue;
  }
  console.log(`    if (sign == U"${key.toUpperCase()}"s) { return ${v}; }`);
}
console.log(`    return 0;
  }
};

} // namespace ksesh`);
