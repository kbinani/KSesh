// clang-format off
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

#include <deque>
#include <optional>
#include <variant>

#include "String.hpp"
#include "Harfbuzz.hpp"
#include "BinaryData.hpp"
#include "Insertions.hpp"
#include "SignList.hpp"
//#include "PresentationSetting.hpp"
//#include "Content.hpp"
//#include "AppSetting.hpp"
//#include "Clipboard.hpp"
//#include "TextEditorComponent.hpp"
//#include "HieroglyphComponent.hpp"
//#include "Sign.hpp"
//#include "SignListButtonContainer.hpp"
//#include "BottomToolBar.hpp"
//#include "SignListComponent.hpp"
//#include "SplitterComponent.hpp"
//#include "CommandID.hpp"
//#include "MenuBarModel.hpp"
//#include "AboutComponent.hpp"
//#include "ExampleComponent.hpp"
//#include "MainComponent.hpp"
//#include "MainWindow.hpp"
//#include "LookAndFeel.hpp"
// clang-format on

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

#include "ContentTests.hpp"

int main(int argc, char **argv) {
  doctest::Context context;
  context.applyCommandLine(argc, argv);
  return context.run();
}
