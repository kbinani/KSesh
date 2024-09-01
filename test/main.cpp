#include <juce_core/juce_core.h>

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

#include "ContentTests.hpp"

int main(int argc, char **argv) {
  doctest::Context context;
  context.applyCommandLine(argc, argv);
  return context.run();
}
