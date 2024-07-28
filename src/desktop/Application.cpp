#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_opengl/juce_opengl.h>

#include <juce_graphics/fonts/harfbuzz/hb.hh>

#include <iostream>

#pragma GCC diagnostic ignored "-Wunused-variable"

// clang-format off
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
    auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::eot_ttf, BinaryData::eot_ttfSize);
    auto blob = hb_blob_create(BinaryData::eot_ttf,
                               BinaryData::eot_ttfSize,
                               HB_MEMORY_MODE_READONLY,
                               nullptr,
                               nullptr);
    hb_face_t *face = hb_face_create(blob, 0);
    hb_font_t *font = hb_font_create(face);
    fFont = std::make_unique<FontData>(font, juce::Font(juce::FontOptions(typeface)));
    hb_face_destroy(face);
    hb_blob_destroy(blob);
    fMainWindow = std::make_unique<MainWindow>(getApplicationName(), *fFont);
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
  std::unique_ptr<FontData> fFont;
};

} // namespace ksesh

START_JUCE_APPLICATION(ksesh::Application)
