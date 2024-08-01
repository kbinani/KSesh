#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_opengl/juce_opengl.h>

#include <iostream>

#pragma GCC diagnostic ignored "-Wunused-variable"

// clang-format off
#include "String.hpp"
#include "Harfbuzz.hpp"
#include "BinaryData.hpp"
#include "Insertions.hpp"
#include "SignList.hpp"
#include "Content.hpp"
#include "AppSetting.hpp"
#include "PresentationSetting.hpp"
#include "HieroglyphComponent.hpp"
#include "TextEditorComponent.hpp"
#include "Sign.hpp"
#include "SignListButtonContainer.hpp"
#include "SignListComponent.hpp"
#include "SplitterComponent.hpp"
#include "MainComponent.hpp"
#include "MainWindow.hpp"
#include "LookAndFeel.hpp"
// clang-format on

using namespace std::literals::string_literals;

namespace ksesh {

class Application : public juce::JUCEApplication, public juce::DarkModeSettingListener {
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
    fLaf = std::make_unique<LookAndFeel>();
    fLaf->setColourScheme(fSetting.getColorScheme(juce::Desktop::getInstance().isDarkModeActive()));
    juce::LookAndFeel::setDefaultLookAndFeel(fLaf.get());
    juce::Desktop::getInstance().addDarkModeSettingListener(this);
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

  void darkModeSettingChanged() override {
    if (fSetting.colorScheme != AppSetting::ColorScheme::Auto) {
      return;
    }
    auto laf = std::make_unique<LookAndFeel>();
    laf->setColourScheme(fSetting.getColorScheme(juce::Desktop::getInstance().isDarkModeActive()));
    juce::LookAndFeel::setDefaultLookAndFeel(laf.get());
    fLaf.swap(laf);
  }

private:
  std::unique_ptr<MainWindow> fMainWindow;
  std::unique_ptr<LookAndFeel> fLaf;
  HbFontUniquePtr fFont;
  AppSetting fSetting;
};

std::vector<std::u32string> const SignList::enclosureBeginning = {
    U"𓉘"s,
    U"𓉙"s,
    U"𓉚"s,
    U"𓊆"s,
    U"𓊈"s,
    U"𓍹"s,
};
std::vector<std::u32string> const SignList::enclosureTerminal = {
    U"𓉛"s,
    U"𓉜"s,
    U"𓉝"s,
    U"𓊇"s,
    U"𓊉"s,
    U"𓍺"s,
    U"𓍻"s,
};

std::u32string const SignList::topStartInsertion = U"\U00013432";
std::u32string const SignList::bottomStartInsertion = U"\U00013433";
std::u32string const SignList::topEndInsertion = U"\U00013434";
std::u32string const SignList::bottomEndInsertion = U"\U00013435";

} // namespace ksesh

START_JUCE_APPLICATION(ksesh::Application)
