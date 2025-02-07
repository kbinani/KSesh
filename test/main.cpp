// clang-format off
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

#include <deque>
#include <optional>
#include <variant>

#include "otf/Type.hpp"
#include "otf/io/InputStream.hpp"
#include "otf/io/OutputStream.hpp"
#include "otf/io/FileInputStream.hpp"
#include "otf/io/FileOutputStream.hpp"
#include "otf/io/ByteInputStream.hpp"
#include "otf/io/ByteOutputStream.hpp"
#include "otf/Tag.hpp"
#include "otf/TableRecord.hpp"
#include "otf/TableDirectory.hpp"
#include "otf/table/Table.hpp"
#include "otf/table/ReadonlyTable.hpp"
#include "otf/table/FontHeaderTable.hpp"
#include "otf/table/MaximumProfileTable.hpp"
#include "otf/FontFile.hpp"

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
