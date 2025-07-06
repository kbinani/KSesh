// clang-format off
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

#include <deque>
#include <optional>
#include <variant>

#include "defer.hpp"

#include "String.hpp"
#include "Harfbuzz.hpp"
#include "BinaryData.hpp"
#include "Insertions.hpp"
#include "SignList.hpp"
// clang-format on

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

int main(int argc, char **argv) {
  doctest::Context context;
  context.applyCommandLine(argc, argv);
  return context.run();
}
