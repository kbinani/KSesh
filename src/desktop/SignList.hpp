#pragma once

namespace ksesh {

class SignList {
  using Entry = std::pair<std::u32string, std::u32string>;

  static std::vector<Entry> const *CreateSigns() {
    using namespace std;
    using namespace std::literals::string_literals;
    auto s = new vector<Entry>();
#define I(a, b) s->push_back(make_pair((a), (b)))
    I(U"A1"s, U"𓀀"s);
    I(U"A2"s, U"𓀁"s);
    I(U"A3"s, U"𓀂"s);
    I(U"A4"s, U"𓀃"s);
    I(U"A5"s, U"𓀄"s);
    I(U"A5A"s, U"𓀅"s);
    I(U"A6"s, U"𓀆"s);
    I(U"A6A"s, U"𓀇"s);
    I(U"A6B"s, U"𓀈"s);
    I(U"A7"s, U"𓀉"s);
    I(U"A8"s, U"𓀊"s);
    I(U"A9"s, U"𓀋"s);
    I(U"A10"s, U"𓀌"s);
    I(U"A11"s, U"𓀍"s);
    I(U"A12"s, U"𓀎"s);
    I(U"A13"s, U"𓀏"s);
    I(U"A14"s, U"𓀐"s);
    I(U"A14A"s, U"𓀑"s);
    I(U"A15"s, U"𓀒"s);
    I(U"A16"s, U"𓀓"s);
    I(U"A17"s, U"𓀔"s);
    I(U"A17A"s, U"𓀕"s);
    I(U"A18"s, U"𓀖"s);
    I(U"A19"s, U"𓀗"s);
    I(U"A20"s, U"𓀘"s);
    I(U"A21"s, U"𓀙"s);
    I(U"A22"s, U"𓀚"s);
    I(U"A23"s, U"𓀛"s);
    I(U"A24"s, U"𓀜"s);
    I(U"A25"s, U"𓀝"s);
    I(U"A26"s, U"𓀞"s);
    I(U"A27"s, U"𓀟"s);
    I(U"A28"s, U"𓀠"s);
    I(U"A29"s, U"𓀡"s);
    I(U"A30"s, U"𓀢"s);
    I(U"A31"s, U"𓀣"s);
    I(U"A32"s, U"𓀤"s);
    I(U"A32A"s, U"𓀥"s);
    I(U"A33"s, U"𓀦"s);
    I(U"A34"s, U"𓀧"s);
    I(U"A35"s, U"𓀨"s);
    I(U"A36"s, U"𓀩"s);
    I(U"A37"s, U"𓀪"s);
    I(U"A38"s, U"𓀫"s);
    I(U"A39"s, U"𓀬"s);
    I(U"A40"s, U"𓀭"s);
    I(U"A40A"s, U"𓀮"s);
    I(U"A41"s, U"𓀯"s);
    I(U"A42"s, U"𓀰"s);
    I(U"A42A"s, U"𓀱"s);
    I(U"A43"s, U"𓀲"s);
    I(U"A43A"s, U"𓀳"s);
    I(U"A44"s, U"𓀴"s);
    I(U"A45"s, U"𓀵"s);
    I(U"A45A"s, U"𓀶"s);
    I(U"A46"s, U"𓀷"s);
    I(U"A47"s, U"𓀸"s);
    I(U"A48"s, U"𓀹"s);
    I(U"A49"s, U"𓀺"s);
    I(U"A50"s, U"𓀻"s);
    I(U"A51"s, U"𓀼"s);
    I(U"A52"s, U"𓀽"s);
    I(U"A53"s, U"𓀾"s);
    I(U"A54"s, U"𓀿"s);
    I(U"A55"s, U"𓁀"s);
    I(U"A56"s, U"𓁁"s);
    I(U"A57"s, U"𓁂"s);
    I(U"A58"s, U"𓁃"s);
    I(U"A59"s, U"𓁄"s);
    I(U"A60"s, U"𓁅"s);
    I(U"A61"s, U"𓁆"s);
    I(U"A62"s, U"𓁇"s);
    I(U"A63"s, U"𓁈"s);
    I(U"A64"s, U"𓁉"s);
    I(U"A65"s, U"𓁊"s);
    I(U"A66"s, U"𓁋"s);
    I(U"A67"s, U"𓁌"s);
    I(U"A68"s, U"𓁍"s);
    I(U"A69"s, U"𓁎"s);
    I(U"A70"s, U"𓁏"s);
    I(U"B1"s, U"𓁐"s);
    I(U"B2"s, U"𓁑"s);
    I(U"B3"s, U"𓁒"s);
    I(U"B4"s, U"𓁓"s);
    I(U"B5"s, U"𓁔"s);
    I(U"B5A"s, U"𓁕"s);
    I(U"B6"s, U"𓁖"s);
    I(U"B7"s, U"𓁗"s);
    I(U"B8"s, U"𓁘"s);
    I(U"B9"s, U"𓁙"s);
    I(U"C1"s, U"𓁚"s);
    I(U"C2"s, U"𓁛"s);
    I(U"C2A"s, U"𓁜"s);
    I(U"C2B"s, U"𓁝"s);
    I(U"C2C"s, U"𓁞"s);
    I(U"C3"s, U"𓁟"s);
    I(U"C4"s, U"𓁠"s);
    I(U"C5"s, U"𓁡"s);
    I(U"C6"s, U"𓁢"s);
    I(U"C7"s, U"𓁣"s);
    I(U"C8"s, U"𓁤"s);
    I(U"C9"s, U"𓁥"s);
    I(U"C10"s, U"𓁦"s);
    I(U"C10A"s, U"𓁧"s);
    I(U"C11"s, U"𓁨"s);
    I(U"C12"s, U"𓁩"s);
    I(U"C13"s, U"𓁪"s);
    I(U"C14"s, U"𓁫"s);
    I(U"C15"s, U"𓁬"s);
    I(U"C16"s, U"𓁭"s);
    I(U"C17"s, U"𓁮"s);
    I(U"C18"s, U"𓁯"s);
    I(U"C19"s, U"𓁰"s);
    I(U"C20"s, U"𓁱"s);
    I(U"C21"s, U"𓁲"s);
    I(U"C22"s, U"𓁳"s);
    I(U"C23"s, U"𓁴"s);
    I(U"C24"s, U"𓁵"s);
    I(U"D1"s, U"𓁶"s);
    I(U"D2"s, U"𓁷"s);
    I(U"D3"s, U"𓁸"s);
    I(U"D4"s, U"𓁹"s);
    I(U"D5"s, U"𓁺"s);
    I(U"D6"s, U"𓁻"s);
    I(U"D7"s, U"𓁼"s);
    I(U"D8"s, U"𓁽"s);
    I(U"D8A"s, U"𓁾"s);
    I(U"D9"s, U"𓁿"s);
    I(U"D10"s, U"𓂀"s);
    I(U"D11"s, U"𓂁"s);
    I(U"D12"s, U"𓂂"s);
    I(U"D13"s, U"𓂃"s);
    I(U"D14"s, U"𓂄"s);
    I(U"D15"s, U"𓂅"s);
    I(U"D16"s, U"𓂆"s);
    I(U"D17"s, U"𓂇"s);
    I(U"D18"s, U"𓂈"s);
    I(U"D19"s, U"𓂉"s);
    I(U"D20"s, U"𓂊"s);
    I(U"D21"s, U"𓂋"s);
    I(U"D22"s, U"𓂌"s);
    I(U"D23"s, U"𓂍"s);
    I(U"D24"s, U"𓂎"s);
    I(U"D25"s, U"𓂏"s);
    I(U"D26"s, U"𓂐"s);
    I(U"D27"s, U"𓂑"s);
    I(U"D27A"s, U"𓂒"s);
    I(U"D28"s, U"𓂓"s);
    I(U"D29"s, U"𓂔"s);
    I(U"D30"s, U"𓂕"s);
    I(U"D31"s, U"𓂖"s);
    I(U"D31A"s, U"𓂗"s);
    I(U"D32"s, U"𓂘"s);
    I(U"D33"s, U"𓂙"s);
    I(U"D34"s, U"𓂚"s);
    I(U"D34A"s, U"𓂛"s);
    I(U"D35"s, U"𓂜"s);
    I(U"D36"s, U"𓂝"s);
    I(U"D37"s, U"𓂞"s);
    I(U"D38"s, U"𓂟"s);
    I(U"D39"s, U"𓂠"s);
    I(U"D40"s, U"𓂡"s);
    I(U"D41"s, U"𓂢"s);
    I(U"D42"s, U"𓂣"s);
    I(U"D43"s, U"𓂤"s);
    I(U"D44"s, U"𓂥"s);
    I(U"D45"s, U"𓂦"s);
    I(U"D46"s, U"𓂧"s);
    I(U"D46A"s, U"𓂨"s);
    I(U"D47"s, U"𓂩"s);
    I(U"D48"s, U"𓂪"s);
    I(U"D48A"s, U"𓂫"s);
    I(U"D49"s, U"𓂬"s);
    I(U"D50"s, U"𓂭"s);
    I(U"D50A"s, U"𓂮"s);
    I(U"D50B"s, U"𓂯"s);
    I(U"D50C"s, U"𓂰"s);
    I(U"D50D"s, U"𓂱"s);
    I(U"D50E"s, U"𓂲"s);
    I(U"D50F"s, U"𓂳"s);
    I(U"D50G"s, U"𓂴"s);
    I(U"D50H"s, U"𓂵"s);
    I(U"D50I"s, U"𓂶"s);
    I(U"D51"s, U"𓂷"s);
    I(U"D52"s, U"𓂸"s);
    I(U"D52A"s, U"𓂹"s);
    I(U"D53"s, U"𓂺"s);
    I(U"D54"s, U"𓂻"s);
    I(U"D54A"s, U"𓂼"s);
    I(U"D55"s, U"𓂽"s);
    I(U"D56"s, U"𓂾"s);
    I(U"D57"s, U"𓂿"s);
    I(U"D58"s, U"𓃀"s);
    I(U"D59"s, U"𓃁"s);
    I(U"D60"s, U"𓃂"s);
    I(U"D61"s, U"𓃃"s);
    I(U"D62"s, U"𓃄"s);
    I(U"D63"s, U"𓃅"s);
    I(U"D64"s, U"𓃆"s);
    I(U"D65"s, U"𓃇"s);
    I(U"D66"s, U"𓃈"s);
    I(U"D67"s, U"𓃉"s);
    I(U"D67A"s, U"𓃊"s);
    I(U"D67B"s, U"𓃋"s);
    I(U"D67C"s, U"𓃌"s);
    I(U"D67D"s, U"𓃍"s);
    I(U"D67E"s, U"𓃎"s);
    I(U"D67F"s, U"𓃏"s);
    I(U"D67G"s, U"𓃐"s);
    I(U"D67H"s, U"𓃑"s);
    I(U"E1"s, U"𓃒"s);
    I(U"E2"s, U"𓃓"s);
    I(U"E3"s, U"𓃔"s);
    I(U"E4"s, U"𓃕"s);
    I(U"E5"s, U"𓃖"s);
    I(U"E6"s, U"𓃗"s);
    I(U"E7"s, U"𓃘"s);
    I(U"E8"s, U"𓃙"s);
    I(U"E8A"s, U"𓃚"s);
    I(U"E9"s, U"𓃛"s);
    I(U"E9A"s, U"𓃜"s);
    I(U"E10"s, U"𓃝"s);
    I(U"E11"s, U"𓃞"s);
    I(U"E12"s, U"𓃟"s);
    I(U"E13"s, U"𓃠"s);
    I(U"E14"s, U"𓃡"s);
    I(U"E15"s, U"𓃢"s);
    I(U"E16"s, U"𓃣"s);
    I(U"E16A"s, U"𓃤"s);
    I(U"E17"s, U"𓃥"s);
    I(U"E17A"s, U"𓃦"s);
    I(U"E18"s, U"𓃧"s);
    I(U"E19"s, U"𓃨"s);
    I(U"E20"s, U"𓃩"s);
    I(U"E20A"s, U"𓃪"s);
    I(U"E21"s, U"𓃫"s);
    I(U"E22"s, U"𓃬"s);
    I(U"E23"s, U"𓃭"s);
    I(U"E24"s, U"𓃮"s);
    I(U"E25"s, U"𓃯"s);
    I(U"E26"s, U"𓃰"s);
    I(U"E27"s, U"𓃱"s);
    I(U"E28"s, U"𓃲"s);
    I(U"E28A"s, U"𓃳"s);
    I(U"E29"s, U"𓃴"s);
    I(U"E30"s, U"𓃵"s);
    I(U"E31"s, U"𓃶"s);
    I(U"E32"s, U"𓃷"s);
    I(U"E33"s, U"𓃸"s);
    I(U"E34"s, U"𓃹"s);
    I(U"E34A"s, U"𓃺"s);
    I(U"E36"s, U"𓃻"s);
    I(U"E37"s, U"𓃼"s);
    I(U"E38"s, U"𓃽"s);
    I(U"F1"s, U"𓃾"s);
    I(U"F1A"s, U"𓃿"s);
    I(U"F2"s, U"𓄀"s);
    I(U"F3"s, U"𓄁"s);
    I(U"F4"s, U"𓄂"s);
    I(U"F5"s, U"𓄃"s);
    I(U"F6"s, U"𓄄"s);
    I(U"F7"s, U"𓄅"s);
    I(U"F8"s, U"𓄆"s);
    I(U"F9"s, U"𓄇"s);
    I(U"F10"s, U"𓄈"s);
    I(U"F11"s, U"𓄉"s);
    I(U"F12"s, U"𓄊"s);
    I(U"F13"s, U"𓄋"s);
    I(U"F13A"s, U"𓄌"s);
    I(U"F14"s, U"𓄍"s);
    I(U"F15"s, U"𓄎"s);
    I(U"F16"s, U"𓄏"s);
    I(U"F17"s, U"𓄐"s);
    I(U"F18"s, U"𓄑"s);
    I(U"F19"s, U"𓄒"s);
    I(U"F20"s, U"𓄓"s);
    I(U"F21"s, U"𓄔"s);
    I(U"F21A"s, U"𓄕"s);
    I(U"F22"s, U"𓄖"s);
    I(U"F23"s, U"𓄗"s);
    I(U"F24"s, U"𓄘"s);
    I(U"F25"s, U"𓄙"s);
    I(U"F26"s, U"𓄚"s);
    I(U"F27"s, U"𓄛"s);
    I(U"F28"s, U"𓄜"s);
    I(U"F29"s, U"𓄝"s);
    I(U"F30"s, U"𓄞"s);
    I(U"F31"s, U"𓄟"s);
    I(U"F31A"s, U"𓄠"s);
    I(U"F32"s, U"𓄡"s);
    I(U"F33"s, U"𓄢"s);
    I(U"F34"s, U"𓄣"s);
    I(U"F35"s, U"𓄤"s);
    I(U"F36"s, U"𓄥"s);
    I(U"F37"s, U"𓄦"s);
    I(U"F37A"s, U"𓄧"s);
    I(U"F38"s, U"𓄨"s);
    I(U"F38A"s, U"𓄩"s);
    I(U"F39"s, U"𓄪"s);
    I(U"F40"s, U"𓄫"s);
    I(U"F41"s, U"𓄬"s);
    I(U"F42"s, U"𓄭"s);
    I(U"F43"s, U"𓄮"s);
    I(U"F44"s, U"𓄯"s);
    I(U"F45"s, U"𓄰"s);
    I(U"F45A"s, U"𓄱"s);
    I(U"F46"s, U"𓄲"s);
    I(U"F46A"s, U"𓄳"s);
    I(U"F47"s, U"𓄴"s);
    I(U"F47A"s, U"𓄵"s);
    I(U"F48"s, U"𓄶"s);
    I(U"F49"s, U"𓄷"s);
    I(U"F50"s, U"𓄸"s);
    I(U"F51"s, U"𓄹"s);
    I(U"F51A"s, U"𓄺"s);
    I(U"F51B"s, U"𓄻"s);
    I(U"F51C"s, U"𓄼"s);
    I(U"F52"s, U"𓄽"s);
    I(U"F53"s, U"𓄾"s);
    I(U"G1"s, U"𓄿"s);
    I(U"G2"s, U"𓅀"s);
    I(U"G3"s, U"𓅁"s);
    I(U"G4"s, U"𓅂"s);
    I(U"G5"s, U"𓅃"s);
    I(U"G6"s, U"𓅄"s);
    I(U"G6A"s, U"𓅅"s);
    I(U"G7"s, U"𓅆"s);
    I(U"G7A"s, U"𓅇"s);
    I(U"G7B"s, U"𓅈"s);
    I(U"G8"s, U"𓅉"s);
    I(U"G9"s, U"𓅊"s);
    I(U"G10"s, U"𓅋"s);
    I(U"G11"s, U"𓅌"s);
    I(U"G11A"s, U"𓅍"s);
    I(U"G12"s, U"𓅎"s);
    I(U"G13"s, U"𓅏"s);
    I(U"G14"s, U"𓅐"s);
    I(U"G15"s, U"𓅑"s);
    I(U"G16"s, U"𓅒"s);
    I(U"G17"s, U"𓅓"s);
    I(U"G18"s, U"𓅔"s);
    I(U"G19"s, U"𓅕"s);
    I(U"G20"s, U"𓅖"s);
    I(U"G20A"s, U"𓅗"s);
    I(U"G21"s, U"𓅘"s);
    I(U"G22"s, U"𓅙"s);
    I(U"G23"s, U"𓅚"s);
    I(U"G24"s, U"𓅛"s);
    I(U"G25"s, U"𓅜"s);
    I(U"G26"s, U"𓅝"s);
    I(U"G26A"s, U"𓅞"s);
    I(U"G27"s, U"𓅟"s);
    I(U"G28"s, U"𓅠"s);
    I(U"G29"s, U"𓅡"s);
    I(U"G30"s, U"𓅢"s);
    I(U"G31"s, U"𓅣"s);
    I(U"G32"s, U"𓅤"s);
    I(U"G33"s, U"𓅥"s);
    I(U"G34"s, U"𓅦"s);
    I(U"G35"s, U"𓅧"s);
    I(U"G36"s, U"𓅨"s);
    I(U"G36A"s, U"𓅩"s);
    I(U"G37"s, U"𓅪"s);
    I(U"G37A"s, U"𓅫"s);
    I(U"G38"s, U"𓅬"s);
    I(U"G39"s, U"𓅭"s);
    I(U"G40"s, U"𓅮"s);
    I(U"G41"s, U"𓅯"s);
    I(U"G42"s, U"𓅰"s);
    I(U"G43"s, U"𓅱"s);
    I(U"G43A"s, U"𓅲"s);
    I(U"G44"s, U"𓅳"s);
    I(U"G45"s, U"𓅴"s);
    I(U"G45A"s, U"𓅵"s);
    I(U"G46"s, U"𓅶"s);
    I(U"G47"s, U"𓅷"s);
    I(U"G48"s, U"𓅸"s);
    I(U"G49"s, U"𓅹"s);
    I(U"G50"s, U"𓅺"s);
    I(U"G51"s, U"𓅻"s);
    I(U"G52"s, U"𓅼"s);
    I(U"G53"s, U"𓅽"s);
    I(U"G54"s, U"𓅾"s);
    I(U"H1"s, U"𓅿"s);
    I(U"H2"s, U"𓆀"s);
    I(U"H3"s, U"𓆁"s);
    I(U"H4"s, U"𓆂"s);
    I(U"H5"s, U"𓆃"s);
    I(U"H6"s, U"𓆄"s);
    I(U"H6A"s, U"𓆅"s);
    I(U"H7"s, U"𓆆"s);
    I(U"H8"s, U"𓆇"s);
    I(U"I1"s, U"𓆈"s);
    I(U"I2"s, U"𓆉"s);
    I(U"I3"s, U"𓆊"s);
    I(U"I4"s, U"𓆋"s);
    I(U"I5"s, U"𓆌"s);
    I(U"I5A"s, U"𓆍"s);
    I(U"I6"s, U"𓆎"s);
    I(U"I7"s, U"𓆏"s);
    I(U"I8"s, U"𓆐"s);
    I(U"I9"s, U"𓆑"s);
    I(U"I9A"s, U"𓆒"s);
    I(U"I10"s, U"𓆓"s);
    I(U"I10A"s, U"𓆔"s);
    I(U"I11"s, U"𓆕"s);
    I(U"I11A"s, U"𓆖"s);
    I(U"I12"s, U"𓆗"s);
    I(U"I13"s, U"𓆘"s);
    I(U"I14"s, U"𓆙"s);
    I(U"I15"s, U"𓆚"s);
    I(U"K1"s, U"𓆛"s);
    I(U"K2"s, U"𓆜"s);
    I(U"K3"s, U"𓆝"s);
    I(U"K4"s, U"𓆞"s);
    I(U"K5"s, U"𓆟"s);
    I(U"K6"s, U"𓆠"s);
    I(U"K7"s, U"𓆡"s);
    I(U"K8"s, U"𓆢"s);
    I(U"L1"s, U"𓆣"s);
    I(U"L2"s, U"𓆤"s);
    I(U"L2A"s, U"𓆥"s);
    I(U"L3"s, U"𓆦"s);
    I(U"L4"s, U"𓆧"s);
    I(U"L5"s, U"𓆨"s);
    I(U"L6"s, U"𓆩"s);
    I(U"L6A"s, U"𓆪"s);
    I(U"L7"s, U"𓆫"s);
    I(U"L8"s, U"𓆬"s);
    I(U"M1"s, U"𓆭"s);
    I(U"M1A"s, U"𓆮"s);
    I(U"M1B"s, U"𓆯"s);
    I(U"M2"s, U"𓆰"s);
    I(U"M3"s, U"𓆱"s);
    I(U"M3A"s, U"𓆲"s);
    I(U"M4"s, U"𓆳"s);
    I(U"M5"s, U"𓆴"s);
    I(U"M6"s, U"𓆵"s);
    I(U"M7"s, U"𓆶"s);
    I(U"M8"s, U"𓆷"s);
    I(U"M9"s, U"𓆸"s);
    I(U"M10"s, U"𓆹"s);
    I(U"M10A"s, U"𓆺"s);
    I(U"M11"s, U"𓆻"s);
    I(U"M12"s, U"𓆼"s);
    I(U"M12A"s, U"𓆽"s);
    I(U"M12B"s, U"𓆾"s);
    I(U"M12C"s, U"𓆿"s);
    I(U"M12D"s, U"𓇀"s);
    I(U"M12E"s, U"𓇁"s);
    I(U"M12F"s, U"𓇂"s);
    I(U"M12G"s, U"𓇃"s);
    I(U"M12H"s, U"𓇄"s);
    I(U"M13"s, U"𓇅"s);
    I(U"M14"s, U"𓇆"s);
    I(U"M15"s, U"𓇇"s);
    I(U"M15A"s, U"𓇈"s);
    I(U"M16"s, U"𓇉"s);
    I(U"M16A"s, U"𓇊"s);
    I(U"M17"s, U"𓇋"s);
    I(U"M17A"s, U"𓇌"s);
    I(U"M18"s, U"𓇍"s);
    I(U"M19"s, U"𓇎"s);
    I(U"M20"s, U"𓇏"s);
    I(U"M21"s, U"𓇐"s);
    I(U"M22"s, U"𓇑"s);
    I(U"M22A"s, U"𓇒"s);
    I(U"M23"s, U"𓇓"s);
    I(U"M24"s, U"𓇔"s);
    I(U"M24A"s, U"𓇕"s);
    I(U"M25"s, U"𓇖"s);
    I(U"M26"s, U"𓇗"s);
    I(U"M27"s, U"𓇘"s);
    I(U"M28"s, U"𓇙"s);
    I(U"M28A"s, U"𓇚"s);
    I(U"M29"s, U"𓇛"s);
    I(U"M30"s, U"𓇜"s);
    I(U"M31"s, U"𓇝"s);
    I(U"M31A"s, U"𓇞"s);
    I(U"M32"s, U"𓇟"s);
    I(U"M33"s, U"𓇠"s);
    I(U"M33A"s, U"𓇡"s);
    I(U"M33B"s, U"𓇢"s);
    I(U"M34"s, U"𓇣"s);
    I(U"M35"s, U"𓇤"s);
    I(U"M36"s, U"𓇥"s);
    I(U"M37"s, U"𓇦"s);
    I(U"M38"s, U"𓇧"s);
    I(U"M39"s, U"𓇨"s);
    I(U"M40"s, U"𓇩"s);
    I(U"M40A"s, U"𓇪"s);
    I(U"M41"s, U"𓇫"s);
    I(U"M42"s, U"𓇬"s);
    I(U"M43"s, U"𓇭"s);
    I(U"M44"s, U"𓇮"s);
    I(U"N1"s, U"𓇯"s);
    I(U"N2"s, U"𓇰"s);
    I(U"N3"s, U"𓇱"s);
    I(U"N4"s, U"𓇲"s);
    I(U"N5"s, U"𓇳"s);
    I(U"N6"s, U"𓇴"s);
    I(U"N7"s, U"𓇵"s);
    I(U"N8"s, U"𓇶"s);
    I(U"N9"s, U"𓇷"s);
    I(U"N10"s, U"𓇸"s);
    I(U"N11"s, U"𓇹"s);
    I(U"N12"s, U"𓇺"s);
    I(U"N13"s, U"𓇻"s);
    I(U"N14"s, U"𓇼"s);
    I(U"N15"s, U"𓇽"s);
    I(U"N16"s, U"𓇾"s);
    I(U"N17"s, U"𓇿"s);
    I(U"N18"s, U"𓈀"s);
    I(U"N18A"s, U"𓈁"s);
    I(U"N18B"s, U"𓈂"s);
    I(U"N19"s, U"𓈃"s);
    I(U"N20"s, U"𓈄"s);
    I(U"N21"s, U"𓈅"s);
    I(U"N22"s, U"𓈆"s);
    I(U"N23"s, U"𓈇"s);
    I(U"N24"s, U"𓈈"s);
    I(U"N25"s, U"𓈉"s);
    I(U"N25A"s, U"𓈊"s);
    I(U"N26"s, U"𓈋"s);
    I(U"N27"s, U"𓈌"s);
    I(U"N28"s, U"𓈍"s);
    I(U"N29"s, U"𓈎"s);
    I(U"N30"s, U"𓈏"s);
    I(U"N31"s, U"𓈐"s);
    I(U"N32"s, U"𓈑"s);
    I(U"N33"s, U"𓈒"s);
    I(U"N33A"s, U"𓈓"s);
    I(U"N34"s, U"𓈔"s);
    I(U"N34A"s, U"𓈕"s);
    I(U"N35"s, U"𓈖"s);
    I(U"N35A"s, U"𓈗"s);
    I(U"N36"s, U"𓈘"s);
    I(U"N37"s, U"𓈙"s);
    I(U"N37A"s, U"𓈚"s);
    I(U"N38"s, U"𓈛"s);
    I(U"N39"s, U"𓈜"s);
    I(U"N40"s, U"𓈝"s);
    I(U"N41"s, U"𓈞"s);
    I(U"N42"s, U"𓈟"s);
    I(U"NL1"s, U"𓈠"s);
    I(U"NL2"s, U"𓈡"s);
    I(U"NL3"s, U"𓈢"s);
    I(U"NL4"s, U"𓈣"s);
    I(U"NL5"s, U"𓈤"s);
    I(U"NL5a"s, U"𓈥"s);
    I(U"NL6"s, U"𓈦"s);
    I(U"NL7"s, U"𓈧"s);
    I(U"NL8"s, U"𓈨"s);
    I(U"NL9"s, U"𓈩"s);
    I(U"NL10"s, U"𓈪"s);
    I(U"NL11"s, U"𓈫"s);
    I(U"NL12"s, U"𓈬"s);
    I(U"NL13"s, U"𓈭"s);
    I(U"NL14"s, U"𓈮"s);
    I(U"NL15"s, U"𓈯"s);
    I(U"NL16"s, U"𓈰"s);
    I(U"NL17"s, U"𓈱"s);
    I(U"NL17a"s, U"𓈲"s);
    I(U"NL18"s, U"𓈳"s);
    I(U"NL19"s, U"𓈴"s);
    I(U"NL20"s, U"𓈵"s);
    I(U"NU1"s, U"𓈶"s);
    I(U"NU2"s, U"𓈷"s);
    I(U"NU3"s, U"𓈸"s);
    I(U"NU4"s, U"𓈹"s);
    I(U"NU5"s, U"𓈺"s);
    I(U"NU6"s, U"𓈻"s);
    I(U"NU7"s, U"𓈼"s);
    I(U"NU8"s, U"𓈽"s);
    I(U"NU9"s, U"𓈾"s);
    I(U"NU10"s, U"𓈿"s);
    I(U"NU10a"s, U"𓉀"s);
    I(U"NU11"s, U"𓉁"s);
    I(U"NU11a"s, U"𓉂"s);
    I(U"NU12"s, U"𓉃"s);
    I(U"NU13"s, U"𓉄"s);
    I(U"NU14"s, U"𓉅"s);
    I(U"NU15"s, U"𓉆"s);
    I(U"NU16"s, U"𓉇"s);
    I(U"NU17"s, U"𓉈"s);
    I(U"NU18"s, U"𓉉"s);
    I(U"NU18a"s, U"𓉊"s);
    I(U"NU19"s, U"𓉋"s);
    I(U"NU20"s, U"𓉌"s);
    I(U"NU21"s, U"𓉍"s);
    I(U"NU22"s, U"𓉎"s);
    I(U"NU22a"s, U"𓉏"s);
    I(U"O1"s, U"𓉐"s);
    I(U"O1A"s, U"𓉑"s);
    I(U"O2"s, U"𓉒"s);
    I(U"O3"s, U"𓉓"s);
    I(U"O4"s, U"𓉔"s);
    I(U"O5"s, U"𓉕"s);
    I(U"O5A"s, U"𓉖"s);
    I(U"O6"s, U"𓉗"s);
    I(U"O6A"s, U"𓉘"s);
    I(U"O6B"s, U"𓉙"s);
    I(U"O6C"s, U"𓉚"s);
    I(U"O6D"s, U"𓉛"s);
    I(U"O6E"s, U"𓉜"s);
    I(U"O6F"s, U"𓉝"s);
    I(U"O7"s, U"𓉞"s);
    I(U"O8"s, U"𓉟"s);
    I(U"O9"s, U"𓉠"s);
    I(U"O10"s, U"𓉡"s);
    I(U"O10A"s, U"𓉢"s);
    I(U"O10B"s, U"𓉣"s);
    I(U"O10C"s, U"𓉤"s);
    I(U"O11"s, U"𓉥"s);
    I(U"O12"s, U"𓉦"s);
    I(U"O13"s, U"𓉧"s);
    I(U"O14"s, U"𓉨"s);
    I(U"O15"s, U"𓉩"s);
    I(U"O16"s, U"𓉪"s);
    I(U"O17"s, U"𓉫"s);
    I(U"O18"s, U"𓉬"s);
    I(U"O19"s, U"𓉭"s);
    I(U"O19A"s, U"𓉮"s);
    I(U"O20"s, U"𓉯"s);
    I(U"O20A"s, U"𓉰"s);
    I(U"O21"s, U"𓉱"s);
    I(U"O22"s, U"𓉲"s);
    I(U"O23"s, U"𓉳"s);
    I(U"O24"s, U"𓉴"s);
    I(U"O24A"s, U"𓉵"s);
    I(U"O25"s, U"𓉶"s);
    I(U"O25A"s, U"𓉷"s);
    I(U"O26"s, U"𓉸"s);
    I(U"O27"s, U"𓉹"s);
    I(U"O28"s, U"𓉺"s);
    I(U"O29A"s, U"𓉼"s);
    I(U"O29"s, U"𓉻"s);
    I(U"O30"s, U"𓉽"s);
    I(U"O30A"s, U"𓉾"s);
    I(U"O31"s, U"𓉿"s);
    I(U"O32"s, U"𓊀"s);
    I(U"O33"s, U"𓊁"s);
    I(U"O33A"s, U"𓊂"s);
    I(U"O34"s, U"𓊃"s);
    I(U"O35"s, U"𓊄"s);
    I(U"O36"s, U"𓊅"s);
    I(U"O36A"s, U"𓊆"s);
    I(U"O36B"s, U"𓊇"s);
    I(U"O36C"s, U"𓊈"s);
    I(U"O36D"s, U"𓊉"s);
    I(U"O37"s, U"𓊊"s);
    I(U"O38"s, U"𓊋"s);
    I(U"O39"s, U"𓊌"s);
    I(U"O40"s, U"𓊍"s);
    I(U"O41"s, U"𓊎"s);
    I(U"O42"s, U"𓊏"s);
    I(U"O43"s, U"𓊐"s);
    I(U"O44"s, U"𓊑"s);
    I(U"O45"s, U"𓊒"s);
    I(U"O46"s, U"𓊓"s);
    I(U"O47"s, U"𓊔"s);
    I(U"O48"s, U"𓊕"s);
    I(U"O49"s, U"𓊖"s);
    I(U"O50"s, U"𓊗"s);
    I(U"O50A"s, U"𓊘"s);
    I(U"O50B"s, U"𓊙"s);
    I(U"O51"s, U"𓊚"s);
    I(U"P1"s, U"𓊛"s);
    I(U"P1A"s, U"𓊜"s);
    I(U"P2"s, U"𓊝"s);
    I(U"P3"s, U"𓊞"s);
    I(U"P3A"s, U"𓊟"s);
    I(U"P4"s, U"𓊠"s);
    I(U"P5"s, U"𓊡"s);
    I(U"P6"s, U"𓊢"s);
    I(U"P7"s, U"𓊣"s);
    I(U"P8"s, U"𓊤"s);
    I(U"P9"s, U"𓊥"s);
    I(U"P10"s, U"𓊦"s);
    I(U"P11"s, U"𓊧"s);
    I(U"Q1"s, U"𓊨"s);
    I(U"Q2"s, U"𓊩"s);
    I(U"Q3"s, U"𓊪"s);
    I(U"Q4"s, U"𓊫"s);
    I(U"Q5"s, U"𓊬"s);
    I(U"Q6"s, U"𓊭"s);
    I(U"Q7"s, U"𓊮"s);
    I(U"R1"s, U"𓊯"s);
    I(U"R2"s, U"𓊰"s);
    I(U"R2A"s, U"𓊱"s);
    I(U"R3"s, U"𓊲"s);
    I(U"R3A"s, U"𓊳"s);
    I(U"R3B"s, U"𓊴"s);
    I(U"R4"s, U"𓊵"s);
    I(U"R5"s, U"𓊶"s);
    I(U"R6"s, U"𓊷"s);
    I(U"R7"s, U"𓊸"s);
    I(U"R8"s, U"𓊹"s);
    I(U"R9"s, U"𓊺"s);
    I(U"R10"s, U"𓊻"s);
    I(U"R10A"s, U"𓊼"s);
    I(U"R11"s, U"𓊽"s);
    I(U"R12"s, U"𓊾"s);
    I(U"R13"s, U"𓊿"s);
    I(U"R14"s, U"𓋀"s);
    I(U"R15"s, U"𓋁"s);
    I(U"R16"s, U"𓋂"s);
    I(U"R16A"s, U"𓋃"s);
    I(U"R17"s, U"𓋄"s);
    I(U"R18"s, U"𓋅"s);
    I(U"R19"s, U"𓋆"s);
    I(U"R20"s, U"𓋇"s);
    I(U"R21"s, U"𓋈"s);
    I(U"R22"s, U"𓋉"s);
    I(U"R23"s, U"𓋊"s);
    I(U"R24"s, U"𓋋"s);
    I(U"R25"s, U"𓋌"s);
    I(U"R26"s, U"𓋍"s);
    I(U"R27"s, U"𓋎"s);
    I(U"R28"s, U"𓋏"s);
    I(U"R29"s, U"𓋐"s);
    I(U"S1"s, U"𓋑"s);
    I(U"S2"s, U"𓋒"s);
    I(U"S2A"s, U"𓋓"s);
    I(U"S3"s, U"𓋔"s);
    I(U"S4"s, U"𓋕"s);
    I(U"S5"s, U"𓋖"s);
    I(U"S6"s, U"𓋗"s);
    I(U"S6A"s, U"𓋘"s);
    I(U"S7"s, U"𓋙"s);
    I(U"S8"s, U"𓋚"s);
    I(U"S9"s, U"𓋛"s);
    I(U"S10"s, U"𓋜"s);
    I(U"S11"s, U"𓋝"s);
    I(U"S12"s, U"𓋞"s);
    I(U"S13"s, U"𓋟"s);
    I(U"S14"s, U"𓋠"s);
    I(U"S14A"s, U"𓋡"s);
    I(U"S14B"s, U"𓋢"s);
    I(U"S15"s, U"𓋣"s);
    I(U"S16"s, U"𓋤"s);
    I(U"S17"s, U"𓋥"s);
    I(U"S17A"s, U"𓋦"s);
    I(U"S18"s, U"𓋧"s);
    I(U"S19"s, U"𓋨"s);
    I(U"S20"s, U"𓋩"s);
    I(U"S21"s, U"𓋪"s);
    I(U"S22"s, U"𓋫"s);
    I(U"S23"s, U"𓋬"s);
    I(U"S24"s, U"𓋭"s);
    I(U"S25"s, U"𓋮"s);
    I(U"S26"s, U"𓋯"s);
    I(U"S26A"s, U"𓋰"s);
    I(U"S26B"s, U"𓋱"s);
    I(U"S27"s, U"𓋲"s);
    I(U"S28"s, U"𓋳"s);
    I(U"S29"s, U"𓋴"s);
    I(U"S30"s, U"𓋵"s);
    I(U"S31"s, U"𓋶"s);
    I(U"S32"s, U"𓋷"s);
    I(U"S33"s, U"𓋸"s);
    I(U"S34"s, U"𓋹"s);
    I(U"S35"s, U"𓋺"s);
    I(U"S35A"s, U"𓋻"s);
    I(U"S36"s, U"𓋼"s);
    I(U"S37"s, U"𓋽"s);
    I(U"S38"s, U"𓋾"s);
    I(U"S39"s, U"𓋿"s);
    I(U"S40"s, U"𓌀"s);
    I(U"S41"s, U"𓌁"s);
    I(U"S42"s, U"𓌂"s);
    I(U"S43"s, U"𓌃"s);
    I(U"S44"s, U"𓌄"s);
    I(U"S45"s, U"𓌅"s);
    I(U"S46"s, U"𓌆"s);
    I(U"T1"s, U"𓌇"s);
    I(U"T2"s, U"𓌈"s);
    I(U"T3"s, U"𓌉"s);
    I(U"T3A"s, U"𓌊"s);
    I(U"T4"s, U"𓌋"s);
    I(U"T5"s, U"𓌌"s);
    I(U"T6"s, U"𓌍"s);
    I(U"T7"s, U"𓌎"s);
    I(U"T7A"s, U"𓌏"s);
    I(U"T8"s, U"𓌐"s);
    I(U"T8A"s, U"𓌑"s);
    I(U"T9"s, U"𓌒"s);
    I(U"T9A"s, U"𓌓"s);
    I(U"T10"s, U"𓌔"s);
    I(U"T11"s, U"𓌕"s);
    I(U"T11A"s, U"𓌖"s);
    I(U"T12"s, U"𓌗"s);
    I(U"T13"s, U"𓌘"s);
    I(U"T14"s, U"𓌙"s);
    I(U"T15"s, U"𓌚"s);
    I(U"T16"s, U"𓌛"s);
    I(U"T16A"s, U"𓌜"s);
    I(U"T17"s, U"𓌝"s);
    I(U"T18"s, U"𓌞"s);
    I(U"T19"s, U"𓌟"s);
    I(U"T20"s, U"𓌠"s);
    I(U"T21"s, U"𓌡"s);
    I(U"T22"s, U"𓌢"s);
    I(U"T23"s, U"𓌣"s);
    I(U"T24"s, U"𓌤"s);
    I(U"T25"s, U"𓌥"s);
    I(U"T26"s, U"𓌦"s);
    I(U"T27"s, U"𓌧"s);
    I(U"T28"s, U"𓌨"s);
    I(U"T29"s, U"𓌩"s);
    I(U"T30"s, U"𓌪"s);
    I(U"T31"s, U"𓌫"s);
    I(U"T32"s, U"𓌬"s);
    I(U"T32A"s, U"𓌭"s);
    I(U"T33"s, U"𓌮"s);
    I(U"T33A"s, U"𓌯"s);
    I(U"T34"s, U"𓌰"s);
    I(U"T35"s, U"𓌱"s);
    I(U"T36"s, U"𓌲"s);
    I(U"U1"s, U"𓌳"s);
    I(U"U2"s, U"𓌴"s);
    I(U"U3"s, U"𓌵"s);
    I(U"U4"s, U"𓌶"s);
    I(U"U5"s, U"𓌷"s);
    I(U"U6"s, U"𓌸"s);
    I(U"U6A"s, U"𓌹"s);
    I(U"U6B"s, U"𓌺"s);
    I(U"U7"s, U"𓌻"s);
    I(U"U8"s, U"𓌼"s);
    I(U"U9"s, U"𓌽"s);
    I(U"U10"s, U"𓌾"s);
    I(U"U11"s, U"𓌿"s);
    I(U"U12"s, U"𓍀"s);
    I(U"U13"s, U"𓍁"s);
    I(U"U14"s, U"𓍂"s);
    I(U"U15"s, U"𓍃"s);
    I(U"U16"s, U"𓍄"s);
    I(U"U17"s, U"𓍅"s);
    I(U"U18"s, U"𓍆"s);
    I(U"U19"s, U"𓍇"s);
    I(U"U20"s, U"𓍈"s);
    I(U"U21"s, U"𓍉"s);
    I(U"U22"s, U"𓍊"s);
    I(U"U23"s, U"𓍋"s);
    I(U"U23A"s, U"𓍌"s);
    I(U"U24"s, U"𓍍"s);
    I(U"U25"s, U"𓍎"s);
    I(U"U26"s, U"𓍏"s);
    I(U"U27"s, U"𓍐"s);
    I(U"U28"s, U"𓍑"s);
    I(U"U29"s, U"𓍒"s);
    I(U"U29A"s, U"𓍓"s);
    I(U"U30"s, U"𓍔"s);
    I(U"U31"s, U"𓍕"s);
    I(U"U32"s, U"𓍖"s);
    I(U"U32A"s, U"𓍗"s);
    I(U"U33"s, U"𓍘"s);
    I(U"U34"s, U"𓍙"s);
    I(U"U35"s, U"𓍚"s);
    I(U"U36"s, U"𓍛"s);
    I(U"U37"s, U"𓍜"s);
    I(U"U38"s, U"𓍝"s);
    I(U"U39"s, U"𓍞"s);
    I(U"U40"s, U"𓍟"s);
    I(U"U41"s, U"𓍠"s);
    I(U"U42"s, U"𓍡"s);
    I(U"V1"s, U"𓍢"s);
    I(U"V1A"s, U"𓍣"s);
    I(U"V1B"s, U"𓍤"s);
    I(U"V1C"s, U"𓍥"s);
    I(U"V1D"s, U"𓍦"s);
    I(U"V1E"s, U"𓍧"s);
    I(U"V1F"s, U"𓍨"s);
    I(U"V1G"s, U"𓍩"s);
    I(U"V1H"s, U"𓍪"s);
    I(U"V1I"s, U"𓍫"s);
    I(U"V2"s, U"𓍬"s);
    I(U"V2A"s, U"𓍭"s);
    I(U"V3"s, U"𓍮"s);
    I(U"V4"s, U"𓍯"s);
    I(U"V5"s, U"𓍰"s);
    I(U"V6"s, U"𓍱"s);
    I(U"V7"s, U"𓍲"s);
    I(U"V7A"s, U"𓍳"s);
    I(U"V7B"s, U"𓍴"s);
    I(U"V8"s, U"𓍵"s);
    I(U"V9"s, U"𓍶"s);
    I(U"V10"s, U"𓍷"s);
    I(U"V11"s, U"𓍸"s);
    I(U"V11A"s, U"𓍹"s);
    I(U"V11B"s, U"𓍺"s);
    I(U"V11C"s, U"𓍻"s);
    I(U"V12"s, U"𓍼"s);
    I(U"V12A"s, U"𓍽"s);
    I(U"V12B"s, U"𓍾"s);
    I(U"V13"s, U"𓍿"s);
    I(U"V14"s, U"𓎀"s);
    I(U"V15"s, U"𓎁"s);
    I(U"V16"s, U"𓎂"s);
    I(U"V17"s, U"𓎃"s);
    I(U"V18"s, U"𓎄"s);
    I(U"V19"s, U"𓎅"s);
    I(U"V20"s, U"𓎆"s);
    I(U"V20A"s, U"𓎇"s);
    I(U"V20B"s, U"𓎈"s);
    I(U"V20C"s, U"𓎉"s);
    I(U"V20D"s, U"𓎊"s);
    I(U"V20E"s, U"𓎋"s);
    I(U"V20F"s, U"𓎌"s);
    I(U"V20G"s, U"𓎍"s);
    I(U"V20H"s, U"𓎎"s);
    I(U"V20I"s, U"𓎏"s);
    I(U"V20J"s, U"𓎐"s);
    I(U"V20K"s, U"𓎑"s);
    I(U"V20L"s, U"𓎒"s);
    I(U"V21"s, U"𓎓"s);
    I(U"V22"s, U"𓎔"s);
    I(U"V23"s, U"𓎕"s);
    I(U"V23A"s, U"𓎖"s);
    I(U"V24"s, U"𓎗"s);
    I(U"V25"s, U"𓎘"s);
    I(U"V26"s, U"𓎙"s);
    I(U"V27"s, U"𓎚"s);
    I(U"V28"s, U"𓎛"s);
    I(U"V28A"s, U"𓎜"s);
    I(U"V29"s, U"𓎝"s);
    I(U"V29A"s, U"𓎞"s);
    I(U"V30"s, U"𓎟"s);
    I(U"V30A"s, U"𓎠"s);
    I(U"V31"s, U"𓎡"s);
    I(U"V31A"s, U"𓎢"s);
    I(U"V32"s, U"𓎣"s);
    I(U"V33"s, U"𓎤"s);
    I(U"V33A"s, U"𓎥"s);
    I(U"V34"s, U"𓎦"s);
    I(U"V35"s, U"𓎧"s);
    I(U"V36"s, U"𓎨"s);
    I(U"V37"s, U"𓎩"s);
    I(U"V37A"s, U"𓎪"s);
    I(U"V38"s, U"𓎫"s);
    I(U"V39"s, U"𓎬"s);
    I(U"V40"s, U"𓎭"s);
    I(U"V40A"s, U"𓎮"s);
    I(U"W1"s, U"𓎯"s);
    I(U"W2"s, U"𓎰"s);
    I(U"W3"s, U"𓎱"s);
    I(U"W3A"s, U"𓎲"s);
    I(U"W4"s, U"𓎳"s);
    I(U"W5"s, U"𓎴"s);
    I(U"W6"s, U"𓎵"s);
    I(U"W7"s, U"𓎶"s);
    I(U"W8"s, U"𓎷"s);
    I(U"W9"s, U"𓎸"s);
    I(U"W9A"s, U"𓎹"s);
    I(U"W10"s, U"𓎺"s);
    I(U"W10A"s, U"𓎻"s);
    I(U"W11"s, U"𓎼"s);
    I(U"W12"s, U"𓎽"s);
    I(U"W13"s, U"𓎾"s);
    I(U"W14"s, U"𓎿"s);
    I(U"W14A"s, U"𓏀"s);
    I(U"W15"s, U"𓏁"s);
    I(U"W16"s, U"𓏂"s);
    I(U"W17"s, U"𓏃"s);
    I(U"W17A"s, U"𓏄"s);
    I(U"W18"s, U"𓏅"s);
    I(U"W18A"s, U"𓏆"s);
    I(U"W19"s, U"𓏇"s);
    I(U"W20"s, U"𓏈"s);
    I(U"W21"s, U"𓏉"s);
    I(U"W22"s, U"𓏊"s);
    I(U"W23"s, U"𓏋"s);
    I(U"W24"s, U"𓏌"s);
    I(U"W24A"s, U"𓏍"s);
    I(U"W25"s, U"𓏎"s);
    I(U"X1"s, U"𓏏"s);
    I(U"X2"s, U"𓏐"s);
    I(U"X3"s, U"𓏑"s);
    I(U"X4"s, U"𓏒"s);
    I(U"X4A"s, U"𓏓"s);
    I(U"X4B"s, U"𓏔"s);
    I(U"X5"s, U"𓏕"s);
    I(U"X6"s, U"𓏖"s);
    I(U"X6A"s, U"𓏗"s);
    I(U"X7"s, U"𓏘"s);
    I(U"X8"s, U"𓏙"s);
    I(U"X8A"s, U"𓏚"s);
    I(U"Y1"s, U"𓏛"s);
    I(U"Y1A"s, U"𓏜"s);
    I(U"Y2"s, U"𓏝"s);
    I(U"Y3"s, U"𓏞"s);
    I(U"Y4"s, U"𓏟"s);
    I(U"Y5"s, U"𓏠"s);
    I(U"Y6"s, U"𓏡"s);
    I(U"Y7"s, U"𓏢"s);
    I(U"Y8"s, U"𓏣"s);
    I(U"Z1"s, U"𓏤"s);
    I(U"Z2"s, U"𓏥"s);
    I(U"Z2A"s, U"𓏦"s);
    I(U"Z2B"s, U"𓏧"s);
    I(U"Z2C"s, U"𓏨"s);
    I(U"Z2D"s, U"𓏩"s);
    I(U"Z3"s, U"𓏪"s);
    I(U"Z3A"s, U"𓏫"s);
    I(U"Z3B"s, U"𓏬"s);
    I(U"Z4"s, U"𓏭"s);
    I(U"Z4A"s, U"𓏮"s);
    I(U"Z5"s, U"𓏯"s);
    I(U"Z5A"s, U"𓏰"s);
    I(U"Z6"s, U"𓏱"s);
    I(U"Z7"s, U"𓏲"s);
    I(U"Z8"s, U"𓏳"s);
    I(U"Z9"s, U"𓏴"s);
    I(U"Z10"s, U"𓏵"s);
    I(U"Z11"s, U"𓏶"s);
    I(U"Z12"s, U"𓏷"s);
    I(U"Z13"s, U"𓏸"s);
    I(U"Z14"s, U"𓏹"s);
    I(U"Z15"s, U"𓏺"s);
    I(U"Z15A"s, U"𓏻"s);
    I(U"Z15B"s, U"𓏼"s);
    I(U"Z15C"s, U"𓏽"s);
    I(U"Z15D"s, U"𓏾"s);
    I(U"Z15E"s, U"𓏿"s);
    I(U"Z15F"s, U"𓐀"s);
    I(U"Z15G"s, U"𓐁"s);
    I(U"Z15H"s, U"𓐂"s);
    I(U"Z15I"s, U"𓐃"s);
    I(U"Z16"s, U"𓐄"s);
    I(U"Z16A"s, U"𓐅"s);
    I(U"Z16B"s, U"𓐆"s);
    I(U"Z16C"s, U"𓐇"s);
    I(U"Z16D"s, U"𓐈"s);
    I(U"Z16E"s, U"𓐉"s);
    I(U"Z16F"s, U"𓐊"s);
    I(U"Z16G"s, U"𓐋"s);
    I(U"Z16H"s, U"𓐌"s);
    I(U"Aa1"s, U"𓐍"s);
    I(U"Aa2"s, U"𓐎"s);
    I(U"Aa3"s, U"𓐏"s);
    I(U"Aa4"s, U"𓐐"s);
    I(U"Aa5"s, U"𓐑"s);
    I(U"Aa6"s, U"𓐒"s);
    I(U"Aa7"s, U"𓐓"s);
    I(U"Aa7A"s, U"𓐔"s);
    I(U"Aa7B"s, U"𓐕"s);
    I(U"Aa8"s, U"𓐖"s);
    I(U"Aa9"s, U"𓐗"s);
    I(U"Aa10"s, U"𓐘"s);
    I(U"Aa11"s, U"𓐙"s);
    I(U"Aa12"s, U"𓐚"s);
    I(U"Aa13"s, U"𓐛"s);
    I(U"Aa14"s, U"𓐜"s);
    I(U"Aa15"s, U"𓐝"s);
    I(U"Aa16"s, U"𓐞"s);
    I(U"Aa17"s, U"𓐟"s);
    I(U"Aa18"s, U"𓐠"s);
    I(U"Aa19"s, U"𓐡"s);
    I(U"Aa20"s, U"𓐢"s);
    I(U"Aa21"s, U"𓐣"s);
    I(U"Aa22"s, U"𓐤"s);
    I(U"Aa23"s, U"𓐥"s);
    I(U"Aa24"s, U"𓐦"s);
    I(U"Aa25"s, U"𓐧"s);
    I(U"Aa26"s, U"𓐨"s);
    I(U"Aa27"s, U"𓐩"s);
    I(U"Aa28"s, U"𓐪"s);
    I(U"Aa29"s, U"𓐫"s);
    I(U"Aa30"s, U"𓐬"s);
    I(U"Aa31"s, U"𓐭"s);
    I(U"Aa32"s, U"𓐮"s);
    return s;
  }

public:
  static std::vector<Entry> const &Signs() {
    using namespace std;
    static unique_ptr<vector<Entry> const> const s(CreateSigns());
    return *s;
  }

  static std::vector<Entry> const &Special() {
    using namespace std;
    static unique_ptr<vector<Entry> const> const s(CreateSpecial());
    return *s;
  }

private:
  static std::vector<Entry> const *CreateSpecial() {
    using namespace std;
    auto s = new vector<Entry>();
    // clang-format off
    I(U":"s, U"\U00013430"s); // EGYPTIAN HIEROGLYPH VERTICAL JOINER
    I(U"*"s, U"\U00013431"s); // EGYPTIAN HIEROGLYPH HORIZONTAL JOINER
    I(U".."s, U"\U00013441"s); // EGYPTIAN HIEROGLYPH FULL BLANK
    I(U"."s, U" "s);
    I(U"//"s, U"\U00013441\U00013455"s);
    I(U"v/"s, U"\U00013445\U0000fe00"s);
    I(U"#1"s, U"\U00013447"s);
    I(U"#2"s, U"\U0001344a"s);
    I(U"#3"s, U"\U00013448"s);
    I(U"#4"s, U"\U0001344e"s);
    I(U"#12"s, U"\U0001344b"s); I(U"#21"s, U"\U0001344b"s);
    I(U"#13"s, U"\U00013449"s); I(U"#31"s, U"\U00013449"s);
    I(U"#14"s, U"\U0001344f"s); I(U"#41"s, U"\U0001344f"s);
    I(U"#23"s, U"\U0001344c"s); I(U"#32"s, U"\U0001344c"s);
    I(U"#24"s, U"\U00013452"s); I(U"#42"s, U"\U00013452"s);
    I(U"#34"s, U"\U00013450"s); I(U"#43"s, U"\U00013450"s);
    I(U"#123"s, U"\U0001344d"s); I(U"#132"s, U"\U0001344d"s); I(U"#213"s, U"\U0001344d"s); I(U"#231"s, U"\U0001344d"s); I(U"#312"s, U"\U0001344d"s); I(U"#321"s, U"\U0001344d"s);
    I(U"#124"s, U"\U00013453"s); I(U"#142"s, U"\U00013453"s); I(U"#214"s, U"\U00013453"s); I(U"#241"s, U"\U00013453"s); I(U"#412"s, U"\U00013453"s); I(U"#421"s, U"\U00013453"s);
    I(U"#134"s, U"\U00013451"s); I(U"#143"s, U"\U00013451"s); I(U"#314"s, U"\U00013451"s); I(U"#341"s, U"\U00013451"s); I(U"#413"s, U"\U00013451"s); I(U"#431"s, U"\U00013451"s);
    I(U"#234"s, U"\U00013454"s); I(U"#243"s, U"\U00013454"s); I(U"#324"s, U"\U00013454"s); I(U"#342"s, U"\U00013454"s); I(U"#423"s, U"\U00013454"s); I(U"#432"s, U"\U00013454"s);
    I(U"#1234"s, U"\U00013455"s); I(U"#1243"s, U"\U00013455"s); I(U"#1324"s, U"\U00013455"s); I(U"#1342"s, U"\U00013455"s); I(U"#1423"s, U"\U00013455"s); I(U"#1432"s, U"\U00013455"s); I(U"#2134"s, U"\U00013455"s); I(U"#2143"s, U"\U00013455"s); I(U"#2314"s, U"\U00013455"s); I(U"#2341"s, U"\U00013455"s); I(U"#2413"s, U"\U00013455"s); I(U"#2431"s, U"\U00013455"s); I(U"#3124"s, U"\U00013455"s); I(U"#3142"s, U"\U00013455"s); I(U"#3214"s, U"\U00013455"s); I(U"#3241"s, U"\U00013455"s); I(U"#3412"s, U"\U00013455"s); I(U"#3421"s, U"\U00013455"s); I(U"#4123"s, U"\U00013455"s); I(U"#4132"s, U"\U00013455"s); I(U"#4213"s, U"\U00013455"s); I(U"#4231"s, U"\U00013455"s); I(U"#4312"s, U"\U00013455"s); I(U"#4321"s, U"\U00013455"s);
    I(U"\\shading1"s, U"\U00013447"s);
    I(U"\\shading2"s, U"\U0001344a"s);
    I(U"\\shading3"s, U"\U00013448"s);
    I(U"\\shading4"s, U"\U0001344e"s);
    I(U"\\shading12"s, U"\U0001344b"s); I(U"\\shading21"s, U"\U0001344b"s);
    I(U"\\shading13"s, U"\U00013449"s); I(U"\\shading31"s, U"\U00013449"s);
    I(U"\\shading14"s, U"\U0001344f"s); I(U"\\shading41"s, U"\U0001344f"s);
    I(U"\\shading23"s, U"\U0001344c"s); I(U"\\shading32"s, U"\U0001344c"s);
    I(U"\\shading24"s, U"\U00013452"s); I(U"\\shading42"s, U"\U00013452"s);
    I(U"\\shading34"s, U"\U00013450"s); I(U"\\shading43"s, U"\U00013450"s);
    I(U"\\shading123"s, U"\U0001344d"s); I(U"\\shading132"s, U"\U0001344d"s); I(U"\\shading213"s, U"\U0001344d"s); I(U"\\shading231"s, U"\U0001344d"s); I(U"\\shading312"s, U"\U0001344d"s); I(U"\\shading321"s, U"\U0001344d"s);
    I(U"\\shading124"s, U"\U00013453"s); I(U"\\shading142"s, U"\U00013453"s); I(U"\\shading214"s, U"\U00013453"s); I(U"\\shading241"s, U"\U00013453"s); I(U"\\shading412"s, U"\U00013453"s); I(U"\\shading421"s, U"\U00013453"s);
    I(U"\\shading134"s, U"\U00013451"s); I(U"\\shading143"s, U"\U00013451"s); I(U"\\shading314"s, U"\U00013451"s); I(U"\\shading341"s, U"\U00013451"s); I(U"\\shading413"s, U"\U00013451"s); I(U"\\shading431"s, U"\U00013451"s);
    I(U"\\shading234"s, U"\U00013454"s); I(U"\\shading243"s, U"\U00013454"s); I(U"\\shading324"s, U"\U00013454"s); I(U"\\shading342"s, U"\U00013454"s); I(U"\\shading423"s, U"\U00013454"s); I(U"\\shading432"s, U"\U00013454"s);
    I(U"\\shading1234"s, U"\U00013455"s); I(U"\\shading1243"s, U"\U00013455"s); I(U"\\shading1324"s, U"\U00013455"s); I(U"\\shading1342"s, U"\U00013455"s); I(U"\\shading1423"s, U"\U00013455"s); I(U"\\shading1432"s, U"\U00013455"s); I(U"\\shading2134"s, U"\U00013455"s); I(U"\\shading2143"s, U"\U00013455"s); I(U"\\shading2314"s, U"\U00013455"s); I(U"\\shading2341"s, U"\U00013455"s); I(U"\\shading2413"s, U"\U00013455"s); I(U"\\shading2431"s, U"\U00013455"s); I(U"\\shading3124"s, U"\U00013455"s); I(U"\\shading3142"s, U"\U00013455"s); I(U"\\shading3214"s, U"\U00013455"s); I(U"\\shading3241"s, U"\U00013455"s); I(U"\\shading3412"s, U"\U00013455"s); I(U"\\shading3421"s, U"\U00013455"s); I(U"\\shading4123"s, U"\U00013455"s); I(U"\\shading4132"s, U"\U00013455"s); I(U"\\shading4213"s, U"\U00013455"s); I(U"\\shading4231"s, U"\U00013455"s); I(U"\\shading4312"s, U"\U00013455"s); I(U"\\shading4321"s, U"\U00013455"s);
    I(U"\\"s, U"\U00013440"s); // EGYPTIAN HIEROGLYPH MIRROR HORIZONTALLY
    I(U"<"s, U"𓍹\U0001343c"s); I(U"<1"s, U"𓍹\U0001343c"s); I(U"<2"s, U"𓍹\U0001343c"s); // EGYPTIAN HIEROGLYPH BEGIN ENCLOSURE
    I(U">"s, U"\U0001343d𓍺"s); I(U"1>"s, U"\U0001343d𓍺"s); I(U"2>"s, U"\U0001343d𓍺"s); // EGYPTIAN HIEROGLYPH END ENCLOSURE
    I(U"<h1"s, U"𓉘\U0001343c"s); I(U"<H1"s, U"𓉘\U0001343c"s);
    I(U"h1>"s, U"\U0001343d𓉝"s); I(U"H1>"s, U"\U0001343d𓉝"s);
    I(U"<h2"s, U"𓉙\U0001343c"s); I(U"<H2"s, U"𓉙\U0001343c"s);
    I(U"h2>"s, U"\U0001343d𓉜"s); I(U"H2>"s, U"\U0001343d𓉜"s);
    I(U"<h3"s, U"𓉚\U0001343c"s); I(U"<H3"s, U"𓉚\U0001343c"s);
    I(U"h3>"s, U"\U0001343d𓉛"s); I(U"H3>"s, U"\U0001343d𓉛"s);
    I(U"<f"s, U"𓊈\U0001343e"s); I(U"<F"s, U"𓊈\U0001343e"s);
    I(U"("s, U"\U00013437"s); // EGYPTIAN HIEROGLYPH BEGIN SEGMENT
    I(U")"s, U"\U00013438"s); // EGYPTIAN HIEROGLYPH END SEGMENT
    I(U"##"s, U"\U00013436"s); // EGYPTIAN HIEROGLYPH OVERLAY MIDDLE
    I(U" "s, U""s);
    I(U"-"s, U""s);
    I(U"A"s, U"𓄿"s); // G1
    I(U"i"s, U"𓇋"s); // M17
    I(U"y"s, U"𓏭"s); // Z4
    I(U"a"s, U"𓂝"s); // D36
    I(U"w"s, U"𓅱"s); // G43
    I(U"W"s, U"𓏲"s); // Z7
    I(U"b"s, U"𓃀"s); // D58
    I(U"p"s, U"𓊪"s); // Q3
    I(U"f"s, U"𓆑"s); // I9
    I(U"m"s, U"𓅓"s); // G17
    I(U"n"s, U"𓈖"s); // N35
    I(U"r"s, U"𓂋"s); // D21
    I(U"h"s, U"𓉔"s); // O4
    I(U"H"s, U"𓎛"s); // V28
    I(U"x"s, U"𓐍"s); // Aa1
    I(U"X"s, U"𓄡"s); // F32
    I(U"z"s, U"𓊃"s); // O34
    I(U"s"s, U"𓋴"s); // S29
    I(U"S"s, U"𓈙"s); // N37
    I(U"q"s, U"𓈎"s); // N29
    I(U"k"s, U"𓎡"s); // V31
    I(U"g"s, U"𓎼"s); // W11
    I(U"t"s, U"𓏏"s); // X1
    I(U"T"s, U"𓍿"s); // V13
    I(U"d"s, U"𓂧"s); // D46
    I(U"D"s, U"𓆓"s); // I10
    I(U"1"s, U"𓏤"s); // Z1

    I(U"qiz"s, U"𓀫"s);
    I(U"Xrd"s, U"𓀔"s);
    I(U"iry"s, U"𓀸"s);
    I(U"Sps"s, U"𓀻"s);
    I(U"Spsi"s, U"𓀼"s);
    I(U"msi"s, U"𓁒"s);
    I(U"DHwty"s, U"𓁟"s);
    I(U"Xnmw"s, U"𓁠"s);
    I(U"inpw"s, U"𓁢"s);
    I(U"stX"s, U"𓁣"s);
    I(U"mnw"s, U"𓁤"s);
    I(U"mAat"s, U"𓁦"s);
    I(U"HH"s, U"𓁨"s);
    I(U"tp"s, U"𓁶"s);
    I(U"Hr"s, U"𓁷"s);
    I(U"Sny"s, U"𓁸"s);
    I(U"ir"s, U"𓁹"s);
    I(U"rmi"s, U"𓁿"s);
    I(U"wDAt"s, U"𓂀"s);
    I(U"fnD"s, U"𓂉"s);
    I(U"rA"s, U"𓂋"s);
    I(U"spt"s, U"𓂎"s);
    I(U"spty"s, U"𓂏"s);
    I(U"mnD"s, U"𓂑"s);
    I(U"kA"s, U"𓂓"s);
    I(U"aHA"s, U"𓂚"s);
    I(U"Dsr"s, U"𓂦"s);
    I(U"mt"s, U"𓂸"s);
    I(U"rd"s, U"𓂾"s);
    I(U"sbq"s, U"𓂾"s);
    I(U"gH"s, U"𓂾"s);
    I(U"gHs"s, U"𓂾"s);
    I(U"ab"s, U"𓃁"s);
    I(U"wab"s, U"𓃂"s);
    I(U"sAH"s, U"𓃃"s);
    I(U"zzmt"s, U"𓃗"s);
    I(U"zAb"s, U"𓃥"s);
    I(U"mAi"s, U"𓃬"s);
    I(U"rw"s, U"𓃭"s);
    I(U"l"s, U"𓃭"s);
    I(U"Aby"s, U"𓃮"s);
    I(U"wn"s, U"𓃺"s);
    I(U"HAt"s, U"𓄂"s);
    I(U"SsA"s, U"𓄃"s);
    I(U"wsr"s, U"𓄊"s);
    I(U"wp"s, U"𓄋"s);
    I(U"db"s, U"𓄏"s);
    I(U"Hw"s, U"𓄑"s);
    I(U"bH"s, U"𓄑"s);
    I(U"ns"s, U"𓄓"s);
    I(U"idn"s, U"𓄔"s);
    I(U"msDr"s, U"𓄔"s);
    I(U"sDm"s, U"𓄔"s);
    I(U"DrD"s, U"𓄔"s);
    I(U"pH"s, U"𓄖"s);
    I(U"kfA"s, U"𓄖"s);
    I(U"xpS"s, U"𓄗"s);
    I(U"wHm"s, U"𓄙"s);
    I(U"Xn"s, U"𓄚"s);
    I(U"sti"s, U"𓄝"s);
    I(U"Sd"s, U"𓄞"s);
    I(U"ms"s, U"𓄟"s);
    I(U"sd"s, U"𓄢"s);
    I(U"ib"s, U"𓄣"s);
    I(U"nfr"s, U"𓄤"s);
    I(U"zmA"s, U"𓄥"s);
    I(U"imAx"s, U"𓄪"s);
    I(U"Aw"s, U"𓄫"s);
    I(U"spr"s, U"𓄭"s);
    I(U"iwa"s, U"𓄯"s);
    I(U"isw"s, U"𓄯"s);
    I(U"pXr"s, U"𓄲"s);
    I(U"qAb"s, U"𓄲"s);
    I(U"tyw"s, U"𓅂"s);
    I(U"mwt"s, U"𓅐"s);
    I(U"nbty"s, U"𓅒"s);
    I(U"mm"s, U"𓅔"s);
    I(U"nH"s, U"𓅘"s);
    I(U"Db"s, U"𓅙"s);
    I(U"rxyt"s, U"𓅚"s);
    I(U"Ax"s, U"𓅜"s);
    I(U"dSr"s, U"𓅟"s);
    I(U"gm"s, U"𓅠"s);
    I(U"bA"s, U"𓅡"s);
    I(U"baHi"s, U"𓅤"s);
    I(U"aq"s, U"𓅧"s);
    I(U"wr"s, U"𓅩"s);
    I(U"gb"s, U"𓅬"s);
    I(U"zA"s, U"𓅭"s);
    I(U"pA"s, U"𓅮"s);
    I(U"xn"s, U"𓅯"s);
    I(U"wSA"s, U"𓅰"s);
    I(U"ww"s, U"𓅳"s);
    I(U"mAw"s, U"𓅶"s);
    I(U"TA"s, U"𓅷"s);
    I(U"snD"s, U"𓅾"s);
    I(U"wSm"s, U"𓆀"s);
    I(U"pAq"s, U"𓆁"s);
    I(U"Sw"s, U"𓆄"s);
    I(U"aSA"s, U"𓆈"s);
    I(U"Styw"s, U"𓆉"s);
    I(U"mzH"s, U"𓆊"s);
    I(U"sbk"s, U"𓆋"s);
    I(U"sAq"s, U"𓆌"s);
    I(U"km"s, U"𓆎"s);
    I(U"Hfn"s, U"𓆐"s);
    I(U"DD"s, U"𓆕"s);
    I(U"in"s, U"𓆛"s);
    I(U"ad"s, U"𓆝"s);
    I(U"XA"s, U"𓆞"s);
    I(U"bz"s, U"𓆟"s);
    I(U"nSmt"s, U"𓆠"s);
    I(U"xpr"s, U"𓆣"s);
    I(U"bit"s, U"𓆤"s);
    I(U"srqt"s, U"𓆫"s);
    I(U"iAm"s, U"𓆭"s);
    I(U"Hn"s, U"𓆰"s);
    I(U"xt"s, U"𓆱"s);
    I(U"rnp"s, U"𓆳"s);
    I(U"tr"s, U"𓆵"s);
    I(U"SA"s, U"𓆷"s);
    I(U"zSn"s, U"𓆸"s);
    I(U"wdn"s, U"𓆻"s);
    I(U"xA"s, U"𓆼"s);
    I(U"wAD"s, U"𓇅"s);
    I(U"HA"s, U"𓇉"s);
    I(U"ii"s, U"𓇍"s);
    I(U"sxt"s, U"𓇏"s);
    I(U"sm"s, U"𓇐"s);
    I(U"sw"s, U"𓇓"s);
    I(U"rsw"s, U"𓇔"s);
    I(U"Sma"s, U"𓇗"s);
    I(U"nDm"s, U"𓇛"s);
    I(U"bnr"s, U"𓇜"s);
    I(U"bdt"s, U"𓇣"s);
    I(U"Dr"s, U"𓇥"s);
    I(U"iz"s, U"𓇩"s);
    I(U"pt"s, U"𓇯"s);
    I(U"iAdt"s, U"𓇲"s);
    I(U"idt"s, U"𓇲"s);
    I(U"ra"s, U"𓇳"s);
    I(U"zw"s, U"𓇳"s);
    I(U"hrw"s, U"𓇳"s);
    I(U"Hnmmt"s, U"𓇶"s);
    I(U"pzD"s, U"𓇷"s);
    I(U"Abd"s, U"𓇹"s);
    I(U"iaH"s, U"𓇹"s);
    I(U"dwA"s, U"𓇼"s);
    I(U"sbA"s, U"𓇼"s);
    I(U"dwAt"s, U"𓇽"s);
    I(U"tA"s, U"𓇾"s);
    I(U"iw"s, U"𓏔"s);
    I(U"wDb"s, U"𓈄"s);
    I(U"spAt"s, U"𓈈"s);
    I(U"xAst"s, U"𓈉"s);
    I(U"Dw"s, U"𓈋"s);
    I(U"Axt"s, U"𓈌"s);
    I(U"xa"s, U"𓈍"s);
    I(U"iAt"s, U"𓈏"s);
    I(U"mw"s, U"𓈗"s);
    I(U"Sm"s, U"𓈝"s);
    I(U"id"s, U"𓈟"s);
    I(U"pr"s, U"𓉐"s);
    I(U"Hwt"s, U"𓉗"s);
    I(U"aH"s, U"𓉥"s);
    I(U"wsxt"s, U"𓉩"s);
    I(U"kAr"s, U"𓉬"s);
    I(U"zH"s, U"𓉲"s);
    I(U"txn"s, U"𓉶"s);
    I(U"iwn"s, U"𓉺"s);
    I(U"aAv"s, U"𓉼"s);
    I(U"O29v"s, U"𓉼"s);
    I(U"aA"s, U"𓉻"s);
    I(U"zxnt"s, U"𓉽"s);
    I(U"zb"s, U"𓊄"s);
    I(U"inb"s, U"𓊅"s);
    I(U"Szp"s, U"𓊏"s);
    I(U"ipt"s, U"𓊒"s);
    I(U"nxn"s, U"𓊔"s);
    I(U"niwt"s, U"𓊖"s);
    I(U"zp"s, U"𓊗"s);
    I(U"Snwt"s, U"𓊚"s);
    I(U"wHa"s, U"𓊠"s);
    I(U"TAw"s, U"𓊡"s);
    I(U"nfw"s, U"𓊡"s);
    I(U"aHa"s, U"𓊢"s);
    I(U"xrw"s, U"𓊤"s);
    I(U"st"s, U"𓊨"s);
    I(U"wz"s, U"𓊩"s);
    I(U"qrsw"s, U"𓊭"s);
    I(U"qrs"s, U"𓊭"s);
    I(U"xAwt"s, U"𓊯"s);
    I(U"xAt"s, U"𓊯"s);
    I(U"Htp"s, U"𓊵"s);
    I(U"kAp"s, U"𓊶"s);
    I(U"kp"s, U"𓊶"s);
    I(U"snTr"s, U"𓊸"s);
    I(U"nTr"s, U"𓊹"s);
    I(U"bd"s, U"𓊺"s);
    I(U"dd"s, U"𓊽"s);
    I(U"Dd"s, U"𓊽"s);
    I(U"imnt"s, U"𓋀"s);
    I(U"iAb"s, U"𓋁"s);
    I(U"wx"s, U"𓋂"s);
    I(U"xm"s, U"𓋉"s);
    I(U"HDt"s, U"𓋑"s);
    I(U"dSrt"s, U"𓋔"s);
    I(U"sxmty"s, U"𓋗"s);
    I(U"xprS"s, U"𓋙"s);
    I(U"Atf"s, U"𓋚"s);
    I(U"Swty"s, U"𓋛"s);
    I(U"mDH"s, U"𓋜"s);
    I(U"wsx"s, U"𓋝"s);
    I(U"nbw"s, U"𓋞"s);
    I(U"tHn"s, U"𓋣"s);
    I(U"THn"s, U"𓋣"s);
    I(U"mnit"s, U"𓋧"s);
    I(U"sDAw"s, U"𓋨"s);
    I(U"xtm"s, U"𓋩"s);
    I(U"sT"s, U"𓋫"s);
    I(U"dmD"s, U"𓋬"s);
    I(U"Tz"s, U"𓋭"s);
    I(U"Sndyt"s, U"𓋯"s);
    I(U"mnxt"s, U"𓋲"s);
    I(U"sf"s, U"𓋵"s);
    I(U"siA"s, U"𓋷"s);
    I(U"Tb"s, U"𓋸"s);
    I(U"anx"s, U"𓋹"s);
    I(U"Swt"s, U"𓋺"s);
    I(U"xw"s, U"𓋽"s);
    I(U"HqA"s, U"𓋾"s);
    I(U"awt"s, U"𓋿"s);
    I(U"wAs"s, U"𓌀"s);
    I(U"Dam"s, U"𓌁"s);
    I(U"abA"s, U"𓌂"s);
    I(U"sxm"s, U"𓌂"s);
    I(U"xrp"s, U"𓌂"s);
    I(U"md"s, U"𓌃"s);
    I(U"Ams"s, U"𓌄"s);
    I(U"nxxw"s, U"𓌅"s);
    I(U"HD"s, U"𓌉"s);
    I(U"HDD"s, U"𓌍"s);
    I(U"pd"s, U"𓌒"s);
    I(U"pD"s, U"𓌔"s);
    I(U"zin"s, U"𓌕"s);
    I(U"zwn"s, U"𓌕"s);
    I(U"sXr"s, U"𓌕"s);
    I(U"Ai"s, U"𓌗"s);
    I(U"Ar"s, U"𓌗"s);
    I(U"rwd"s, U"𓌗"s);
    I(U"rwD"s, U"𓌗"s);
    I(U"rs"s, U"𓌘"s);
    I(U"qmA"s, U"𓌙"s);
    I(U"wrrt"s, U"𓌝"s);
    I(U"Sms"s, U"𓌞"s);
    I(U"qs"s, U"𓌟"s);
    I(U"wa"s, U"𓌡"s);
    I(U"sn"s, U"𓌢"s);
    I(U"iH"s, U"𓌤"s);
    I(U"DbA"s, U"𓌥"s);
    I(U"Xr"s, U"𓌨"s);
    I(U"nmt"s, U"𓌩"s);
    I(U"sSm"s, U"𓌫"s);
    I(U"nm"s, U"𓌰"s);
    I(U"mA"s, U"𓌳"s);
    I(U"mr"s, U"𓌸"s);
    I(U"it"s, U"𓌾"s);
    I(U"HqAt"s, U"𓌿"s);
    I(U"hb"s, U"𓍁"s);
    I(U"Sna"s, U"𓍁"s);
    I(U"tm"s, U"𓍃"s);
    I(U"biA"s, U"𓍄"s);
    I(U"grg"s, U"𓍅"s);
    I(U"stp"s, U"𓍉"s);
    I(U"mnx"s, U"𓍊"s);
    I(U"Ab"s, U"𓍋"s);
    I(U"Hmt"s, U"𓍍"s);
    I(U"wbA"s, U"𓍏"s);
    I(U"DA"s, U"𓍑"s);
    I(U"rtH"s, U"𓍕"s);
    I(U"zmn"s, U"𓍖"s);
    I(U"ti"s, U"𓍘"s);
    I(U"xsf"s, U"𓍙"s);
    I(U"Hm"s, U"𓍛"s);
    I(U"mxAt"s, U"𓍝"s);
    I(U"St"s, U"𓍢"s);
    I(U"Snt"s, U"𓍢"s);
    I(U"100"s, U"𓍢"s);
    I(U"sTA"s, U"𓍬"s);
    I(U"sTAw"s, U"𓍮"s);
    I(U"wA"s, U"𓍯"s);
    I(U"snT"s, U"𓍰"s);
    I(U"Sn"s, U"𓍲"s);
    I(U"arq"s, U"𓍼"s);
    I(U"iTi"s, U"𓎁"s);
    I(U"mDt"s, U"𓎅"s);
    I(U"XAr"s, U"𓎅"s);
    I(U"TmA"s, U"𓎅"s);
    I(U"10"s, U"𓎆"s);
    I(U"mD"s, U"𓎆"s);
    I(U"mH"s, U"𓎔"s);
    I(U"wD"s, U"𓎗"s);
    I(U"aD"s, U"𓎙"s);
    I(U"wAH"s, U"𓎝"s);
    I(U"sk"s, U"𓎝"s);
    I(U"nb"s, U"𓎟"s);
    I(U"msn"s, U"𓎣"s);
    I(U"sSr"s, U"𓎤"s);
    I(U"idr"s, U"𓎩"s);
    I(U"bAs"s, U"𓎰"s);
    I(U"Hb"s, U"𓎲"s);
    I(U"Xnm"s, U"𓎸"s);
    I(U"iab"s, U"𓎺"s);
    I(U"nst"s, U"𓎼"s);
    I(U"Hz"s, U"𓎿"s);
    I(U"xnt"s, U"𓏃"s);
    I(U"mi"s, U"𓏇"s);
    I(U"Hnqt"s, U"𓏊"s);
    I(U"nw"s, U"𓏌"s);
    I(U"ini"s, U"𓏎"s);
    I(U"rdi"s, U"𓏙"s);
    I(U"di"s, U"𓏙"s);
    I(U"Y1v"s, U"𓏜"s);
    I(U"mDAt"s, U"𓏛"s);
    I(U"mnhd"s, U"𓏞"s);
    I(U"mn"s, U"𓏠"s);
    I(U"ibA"s, U"𓏡"s);
    I(U"zSSt"s, U"𓏣"s);
    I(U"imi"s, U"𓏶"s);
    I(U"wnm"s, U"𓏶"s);
    I(U"`"s, U"𓏹"s);
    I(U"Hp"s, U"𓐑"s);
    I(U"qn"s, U"𓐖"s);
    I(U"mAa"s, U"𓐙"s);
    I(U"im"s, U"𓐛"s);
    I(U"gs"s, U"𓐛"s);
    I(U"sA"s, U"𓐟"s);
    I(U"apr"s, U"𓐢"s);
    I(U"wDa"s, U"𓐣"s);
    I(U"nD"s, U"𓐩"s);
    I(U"qd"s, U"𓐪"s);
    I(U"Xkr"s, U"𓐬"s);
    I(U"2"s, U"𓏻"s);
    I(U"3"s, U"𓏼"s);
    I(U"4"s, U"𓏽"s);
    I(U"5"s, U"𓏾"s);
    I(U"6"s, U"𓏿"s);
    I(U"7"s, U"𓐀"s);
    I(U"8"s, U"𓐁"s);
    I(U"9"s, U"𓐂"s);
    I(U"nn"s, U"𓇒"s);
    // clang-format on
#undef I
    return s;
  }

  static bool CompareSigns(Entry const &left, Entry const &right) {
    auto l = left.first;
    auto r = right.first;
    if (l.size() == r.size()) {
      return l.compare(r) < 0;
    } else {
      return r.size() < l.size();
    }
  }

  static std::vector<Entry> *CreateMapping() {
    using namespace std;
    auto s = new vector<Entry>();
    for (auto const &it : Signs()) {
      s->push_back(it);
    }
    for (auto const &it : Special()) {
      s->push_back(it);
    }
    std::sort(s->begin(), s->end(), CompareSigns);
    return s;
  }

  static std::vector<Entry> const &Mapping() {
    using namespace std;
    static unique_ptr<vector<Entry> const> const mapping(CreateMapping());
    return *mapping;
  }

public:
  static std::optional<Entry> Map(std::u32string const &s, std::optional<int> offset) {
    using namespace std;
    int o = offset ? *offset : 0;
    u32string_view v(s.begin() + o, s.end());
    auto const &mapping = Mapping();
    auto found = find_if(mapping.begin(), mapping.end(), [&](Entry const &m) {
      return v.starts_with(m.first);
    });
    if (found == mapping.end()) {
      return nullopt;
    } else {
      return *found;
    }
  }

  static std::vector<std::u32string> const &Categories() {
    using namespace std;
    using namespace std::literals::string_literals;
    static vector<u32string> categories;
    if (categories.empty()) {
      unordered_set<u32string> index;
      for (auto [s, _] : Signs()) {
        auto found = find_if(s.begin(), s.end(), [](char32_t c) {
          return U'0' <= c && c <= U'9';
        });
        if (found == s.end()) {
          continue;
        }
        auto category = s.substr(0, *found);
        if (index.count(category) == 0) {
          index.insert(category);
          categories.push_back(category);
        }
      }
    }
    return categories;
  }

private:
  static std::unordered_map<std::u32string, std::vector<std::u32string>> *CreateReverseMapping() {
    using namespace std;
    auto ret = new unordered_map<u32string, vector<u32string>>();
    for (auto const &[write, sign] : Special()) {
      if (sign.empty()) {
        continue;
      }
      (*ret)[sign].push_back(write);
    }
    return ret;
  }

public:
  static std::vector<std::u32string> const &MapReverse(std::u32string const &sign) {
    using namespace std;
    static unique_ptr<unordered_map<u32string, vector<u32string>> const> const sTable(CreateReverseMapping());
    if (auto found = sTable->find(sign); found == sTable->end()) {
      static std::vector<std::u32string> st;
      return st;
    } else {
      return found->second;
    }
  }

private:
  static std::unordered_map<std::u32string, std::u32string> *CreateCodeFromSignTable() {
    using namespace std;
    auto ret = new unordered_map<u32string, u32string>();
    for (auto const &[write, sign] : Signs()) {
      (*ret)[sign] = write;
    }
    return ret;
  }

public:
  static std::optional<std::u32string> CodeFromSign(std::u32string const &sign) {
    using namespace std;
    static unique_ptr<unordered_map<u32string, u32string> const> const sTable(CreateCodeFromSignTable());
    if (auto found = sTable->find(sign); found != sTable->end()) {
      return found->second;
    } else {
      return nullopt;
    }
  }

  static bool IsFormatControl(std::u32string const &ch) {
    char32_t cp = ch[0];
    return 0x13430 <= cp && cp <= 0x13455;
  }
#if 0

  private static _whitespaces: Set<string> | undefined;
  static isWhitespace(char: string): boolean {
    let list = this._whitespaces;
    if (list === undefined) {
      list = new Set<string>();
      for (const [write, sign] of this.special) {
        if (sign === "") {
          list.add(write);
        }
      }
      this._whitespaces = list;
    }
    return list.has(char);
  }
#endif

  static bool IsSign(std::u32string const &ch) {
    using namespace std;
    static unordered_set<u32string> all;
    if (all.empty()) {
      for (auto [write, sign] : Signs()) {
        all.insert(sign);
      }
    }
    return all.find(ch) != all.end();
  }

  static std::vector<std::u32string> const enclosureBeginning;
  static std::vector<std::u32string> const enclosureTerminal;

  static std::u32string const topStartInsertion;
  static std::u32string const bottomStartInsertion;
  static std::u32string const topEndInsertion;
  static std::u32string const bottomEndInsertion;

private:
  static std::unordered_map<std::u32string, uint8_t> const *CreateInsertions() {
    using namespace std;
    auto s = new unordered_map<u32string, uint8_t>();
    for (auto const &[key, sign] : Signs()) {
      auto type = Insertions::InsertionType(key);
      if (sign == U"𓅱") {
        type = (type & (~Insertions::Type::BottomEnd)) | Insertions::Type::TopEnd;
      } else if (sign == U"𓅡") {
        type = type | Insertions::Type::BottomStart;
      }
      if (type == 0) {
        continue;
      }
      (*s)[sign] = type;
    }
    return s;
  }

  static std::unordered_map<std::u32string, uint8_t> const &Insertions() {
    using namespace std;
    static unique_ptr<unordered_map<u32string, uint8_t> const> const sTable(CreateInsertions());
    return *sTable;
  }

public:
  static std::optional<Insertions::Type> InsertionType(std::u32string const &left, std::u32string const &right) {
    auto const &table = Insertions();
    auto l = table.find(left);
    if (l != table.end()) {
      if ((l->second & Insertions::Type::BottomEnd) == Insertions::Type::BottomEnd) {
        return Insertions::Type::BottomEnd;
      }
      if ((l->second & Insertions::Type::TopEnd) == Insertions::Type::TopEnd) {
        return Insertions::Type::TopEnd;
      }
    }
    auto r = table.find(right);
    if (r != table.end()) {
      if ((r->second & Insertions::Type::BottomStart) == Insertions::Type::BottomStart) {
        return Insertions::Type::BottomStart;
      }
      if ((r->second & Insertions::Type::TopStart) == Insertions::Type::TopStart) {
        return Insertions::Type::TopStart;
      }
    }
    return std::nullopt;
  }

  static bool HasInsertion(std::u32string const &center, Insertions::Type type) {
    auto const &table = Insertions();
    switch (type) {
    case Insertions::Type::BottomStart:
    case Insertions::Type::TopStart: {
      auto r = table.find(center);
      if (r == table.end()) {
        return false;
      } else {
        return (r->second & type) == type;
      }
    }
    case Insertions::Type::BottomEnd:
    case Insertions::Type::TopEnd: {
      auto l = table.find(center);
      if (l == table.end()) {
        return false;
      } else {
        return (l->second & type) == type;
      }
    }
    }
    return false;
  }
};

} // namespace ksesh
