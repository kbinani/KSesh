#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_opengl/juce_opengl.h>

#include <juce_graphics/fonts/harfbuzz/hb.hh>

#include "MainWindow.hpp"

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
    return false;
  }

  void initialise(juce::String const &) override {
    hb_buffer_t *buf = hb_buffer_create();
    hb_buffer_destroy(buf);

    mainWindow.reset(new MainWindow(getApplicationName()));
  }

  void shutdown() override {
    mainWindow = nullptr;
  }

  void systemRequestedQuit() override {
    quit();
  }

  void anotherInstanceStarted(juce::String const &) override {
  }

private:
  std::unique_ptr<MainWindow> mainWindow;
};

} // namespace ksesh

START_JUCE_APPLICATION(ksesh::Application)
