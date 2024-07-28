#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "SignList.hpp"

using namespace std::literals::string_literals;

namespace ksesh {
std::vector<std::u32string> SignList::enclosureBeginning = {
    U"𓉘"s,
    U"𓉙"s,
    U"𓉚"s,
    U"𓊆"s,
    U"𓊈"s,
    U"𓍹"s,
};
std::vector<std::u32string> SignList::enclosureTerminal = {
    U"𓉛"s,
    U"𓉜"s,
    U"𓉝"s,
    U"𓊇"s,
    U"𓊉"s,
    U"𓍺"s,
    U"𓍻"s,
};
} // namespace ksesh
