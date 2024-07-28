#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_opengl/juce_opengl.h>

#include <juce_graphics/fonts/harfbuzz/hb.hh>

// clang-format off
#include "BinaryData.hpp"
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
    fTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::eot_ttf, BinaryData::eot_ttfSize);
    fMainWindow = std::make_unique<MainWindow>(getApplicationName(), fTypeface);
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
  juce::Typeface::Ptr fTypeface;
};

} // namespace ksesh

START_JUCE_APPLICATION(ksesh::Application)
