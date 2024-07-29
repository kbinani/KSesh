#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_opengl/juce_opengl.h>

#include <iostream>

#pragma GCC diagnostic ignored "-Wunused-variable"

// clang-format off
#include "String.hpp"
#include "Harfbuzz.hpp"
#include "BinaryData.hpp"
#include "SignList.hpp"
#include "Content.hpp"
#include "HieroglyphComponent.hpp"
#include "TextEditorComponent.hpp"
#include "SignListComponent.hpp"
#include "SplitterComponent.hpp"
#include "MainComponent.hpp"
#include "MainWindow.hpp"
// clang-format on

using namespace std::literals::string_literals;

namespace ksesh {

class Application : public juce::JUCEApplication {
public:
  Application() {}

  juce::String const getApplicationName() override {
    return JUCE_APPLICATION_NAME_STRING;
  }

  juce::String const getApplicationVersion() override {
    return JUCE_APPLICATION_VERSION_STRING;
  }

  bool moreThanOneInstanceAllowed() override {
    return true;
  }

  void initialise(juce::String const &) override {
    HbBlobUniquePtr blob(hb_blob_create(BinaryData::eot_ttf,
                                        BinaryData::eot_ttfSize,
                                        HB_MEMORY_MODE_READONLY,
                                        nullptr,
                                        nullptr));
    HbFaceUniquePtr face(hb_face_create(blob.get(), 0));
    fFont.reset(hb_font_create(face.get()));
    fMainWindow = std::make_unique<MainWindow>(getApplicationName(), fFont);
  }

  void shutdown() override {
    fMainWindow = nullptr;
  }

  void systemRequestedQuit() override {
    quit();
  }

  void anotherInstanceStarted(juce::String const &) override {
  }

private:
  std::unique_ptr<MainWindow> fMainWindow;
  HbFontUniquePtr fFont;
};

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

START_JUCE_APPLICATION(ksesh::Application)
