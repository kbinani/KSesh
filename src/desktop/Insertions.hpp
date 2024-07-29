#pragma once

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
    using namespace std::literals::string_literals;
    if (sign == U"A16"s) { return 8; }
    if (sign == U"A18"s) { return 4; }
    if (sign == U"A26"s) { return 1; }
    if (sign == U"A28"s) { return 5; }
    if (sign == U"A30"s) { return 1; }
    if (sign == U"A40"s) { return 2; }
    if (sign == U"A41"s) { return 2; }
    if (sign == U"A43"s) { return 2; }
    if (sign == U"A45"s) { return 2; }
    if (sign == U"A50"s) { return 2; }
    if (sign == U"B1"s) { return 2; }
    if (sign == U"C1"s) { return 2; }
    if (sign == U"C2A"s) { return 2; }
    if (sign == U"C9"s) { return 2; }
    if (sign == U"C10"s) { return 2; }
    if (sign == U"D3"s) { return 1; }
    if (sign == U"D17"s) { return 8; }
    if (sign == U"D36"s) { return 2; }
    if (sign == U"D41"s) { return 2; }
    if (sign == U"D42"s) { return 2; }
    if (sign == U"D52"s) { return 1; }
    if (sign == U"D53"s) { return 1; }
    if (sign == U"D54"s) { return 2; }
    if (sign == U"D55"s) { return 8; }
    if (sign == U"D56"s) { return 2; }
    if (sign == U"D58"s) { return 2; }
    if (sign == U"D66"s) { return 2; }
    if (sign == U"E1"s) { return 8; }
    if (sign == U"E3"s) { return 8; }
    if (sign == U"E6"s) { return 8; }
    if (sign == U"E7"s) { return 8; }
    if (sign == U"E8"s) { return 8; }
    if (sign == U"E8A"s) { return 9; }
    if (sign == U"E9"s) { return 9; }
    if (sign == U"E10"s) { return 8; }
    if (sign == U"E11"s) { return 8; }
    if (sign == U"E15"s) { return 9; }
    if (sign == U"E16"s) { return 8; }
    if (sign == U"E16A"s) { return 8; }
    if (sign == U"E17"s) { return 8; }
    if (sign == U"E17A"s) { return 8; }
    if (sign == U"E18"s) { return 13; }
    if (sign == U"E19"s) { return 8; }
    if (sign == U"E20"s) { return 8; }
    if (sign == U"E20A"s) { return 8; }
    if (sign == U"E22"s) { return 8; }
    if (sign == U"E23"s) { return 8; }
    if (sign == U"E27"s) { return 8; }
    if (sign == U"E29"s) { return 8; }
    if (sign == U"E31"s) { return 8; }
    if (sign == U"E32"s) { return 8; }
    if (sign == U"E34"s) { return 8; }
    if (sign == U"E38"s) { return 8; }
    if (sign == U"E100"s) { return 8; }
    if (sign == U"F4"s) { return 2; }
    if (sign == U"F6"s) { return 8; }
    if (sign == U"F16"s) { return 8; }
    if (sign == U"F18"s) { return 8; }
    if (sign == U"F19"s) { return 2; }
    if (sign == U"F20"s) { return 1; }
    if (sign == U"F29"s) { return 5; }
    if (sign == U"F30"s) { return 1; }
    if (sign == U"F39"s) { return 1; }
    if (sign == U"F45"s) { return 5; }
    if (sign == U"F45A"s) { return 5; }
    if (sign == U"G1"s) { return 1; }
    if (sign == U"G2"s) { return 1; }
    if (sign == U"G3"s) { return 1; }
    if (sign == U"G4"s) { return 1; }
    if (sign == U"G5"s) { return 1; }
    if (sign == U"G6"s) { return 1; }
    if (sign == U"G6A"s) { return 8; }
    if (sign == U"G7"s) { return 13; }
    if (sign == U"G8"s) { return 8; }
    if (sign == U"G9"s) { return 9; }
    if (sign == U"G10"s) { return 12; }
    if (sign == U"G11A"s) { return 9; }
    if (sign == U"G13"s) { return 8; }
    if (sign == U"G14"s) { return 1; }
    if (sign == U"G15"s) { return 1; }
    if (sign == U"G17"s) { return 9; }
    if (sign == U"G18"s) { return 1; }
    if (sign == U"G20"s) { return 1; }
    if (sign == U"G21"s) { return 9; }
    if (sign == U"G22"s) { return 9; }
    if (sign == U"G23"s) { return 13; }
    if (sign == U"G25"s) { return 13; }
    if (sign == U"G26"s) { return 13; }
    if (sign == U"G26A"s) { return 1; }
    if (sign == U"G27"s) { return 9; }
    if (sign == U"G28"s) { return 9; }
    if (sign == U"G31"s) { return 9; }
    if (sign == U"G32"s) { return 8; }
    if (sign == U"G33"s) { return 1; }
    if (sign == U"G34"s) { return 1; }
    if (sign == U"G35"s) { return 9; }
    if (sign == U"G36"s) { return 9; }
    if (sign == U"G37"s) { return 9; }
    if (sign == U"G38"s) { return 9; }
    if (sign == U"G39"s) { return 13; }
    if (sign == U"G41"s) { return 1; }
    if (sign == U"G42"s) { return 9; }
    if (sign == U"G43"s) { return 5; }
    if (sign == U"G44"s) { return 9; }
    if (sign == U"G45"s) { return 1; }
    if (sign == U"G47"s) { return 9; }
    if (sign == U"G50"s) { return 1; }
    if (sign == U"G53"s) { return 8; }
    if (sign == U"I1"s) { return 1; }
    if (sign == U"I3"s) { return 8; }
    if (sign == U"I5"s) { return 8; }
    if (sign == U"I7"s) { return 8; }
    if (sign == U"I8"s) { return 9; }
    if (sign == U"I9"s) { return 8; }
    if (sign == U"I10"s) { return 9; }
    if (sign == U"I10A"s) { return 1; }
    if (sign == U"I11"s) { return 9; }
    if (sign == U"M9"s) { return 1; }
    if (sign == U"M10"s) { return 1; }
    if (sign == U"M26"s) { return 5; }
    if (sign == U"M27"s) { return 5; }
    if (sign == U"N2"s) { return 5; }
    if (sign == U"N3"s) { return 5; }
    if (sign == U"O14"s) { return 4; }
    if (sign == U"Q2"s) { return 2; }
    if (sign == U"R8"s) { return 1; }
    if (sign == U"R12"s) { return 5; }
    if (sign == U"R13"s) { return 12; }
    if (sign == U"S1"s) { return 2; }
    if (sign == U"S2"s) { return 2; }
    if (sign == U"S28"s) { return 1; }
    if (sign == U"T6"s) { return 1; }
    if (sign == U"T7A"s) { return 1; }
    if (sign == U"T14"s) { return 1; }
    if (sign == U"T32"s) { return 1; }
    if (sign == U"U1"s) { return 2; }
    if (sign == U"U2"s) { return 10; }
    if (sign == U"U15"s) { return 8; }
    if (sign == U"U19"s) { return 8; }
    if (sign == U"U21"s) { return 8; }
    if (sign == U"V12"s) { return 1; }
    if (sign == U"V15"s) { return 1; }
    if (sign == U"V22"s) { return 1; }
    if (sign == U"V23"s) { return 1; }
    if (sign == U"V23A"s) { return 1; }
    if (sign == U"Z6"s) { return 1; }
    if (sign == U"Z11"s) { return 5; }
    if (sign == U"J7"s) { return 2; }
    return 0;
  }
};

} // namespace ksesh
