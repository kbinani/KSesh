#include <juce_gui_extra/juce_gui_extra.h>

#include "PDFGen.hpp"

#include <iostream>

#pragma GCC diagnostic ignored "-Wunused-variable"

// clang-format off
#include "String.hpp"
#include "Harfbuzz.hpp"
#include "BinaryData.hpp"
#include "Insertions.hpp"
#include "SignList.hpp"
#include "PresentationSetting.hpp"
#include "Content.hpp"
#include "AppSetting.hpp"
#include "Clipboard.hpp"
#include "HieroglyphComponent.hpp"
#include "TextEditorComponent.hpp"
#include "Sign.hpp"
#include "SignListButtonContainer.hpp"
#include "BottomToolBar.hpp"
#include "SignListComponent.hpp"
#include "SplitterComponent.hpp"
#include "CommandID.hpp"
#include "MenuBarModel.hpp"
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
    fSetting = std::make_shared<AppSetting>();
    HbBlobUniquePtr blob(hb_blob_create(BinaryData::eot_ttf,
                                        BinaryData::eot_ttfSize,
                                        HB_MEMORY_MODE_READONLY,
                                        nullptr,
                                        nullptr));
    HbFaceUniquePtr face(hb_face_create(blob.get(), 0));
    fFont = HbMakeSharedFontPtr(hb_font_create(face.get()));
    fLaf = std::make_unique<LookAndFeel>();
    fLaf->setColourScheme(fSetting->getColorScheme(juce::Desktop::getInstance().isDarkModeActive()));
    juce::LookAndFeel::setDefaultLookAndFeel(fLaf.get());
    juce::Desktop::getInstance().addDarkModeSettingListener(this);

    fCommandManager = std::make_unique<juce::ApplicationCommandManager>();
    fCommandManager->registerAllCommandsForTarget(this);
    fMainWindow = std::make_unique<MainWindow>(getApplicationName(), fFont, fCommandManager, fSetting);
  }

  void getAllCommands(juce::Array<juce::CommandID> &commands) override {
    for (int id = applicationCommandIDBegin; id < applicationCommandIDEnd; id++) {
      commands.add(id);
    }
  }

  void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &info) override {
    switch (commandID) {
    case commandViewThemeAuto:
      info.setInfo(TRANS("Auto"), {}, {}, 0);
      info.setTicked(fSetting->getColorScheme() == AppSetting::ColorScheme::Auto);
      return;
    case commandViewThemeDark:
      info.setInfo(TRANS("Dark"), {}, {}, 0);
      info.setTicked(fSetting->getColorScheme() == AppSetting::ColorScheme::Dark);
      return;
    case commandViewThemeMidnight:
      info.setInfo(TRANS("Midnight"), {}, {}, 0);
      info.setTicked(fSetting->getColorScheme() == AppSetting::ColorScheme::Midnight);
      return;
    case commandViewThemeGray:
      info.setInfo(TRANS("Gray"), {}, {}, 0);
      info.setTicked(fSetting->getColorScheme() == AppSetting::ColorScheme::Gray);
      return;
    case commandViewThemeLight:
      info.setInfo(TRANS("Light"), {}, {}, 0);
      info.setTicked(fSetting->getColorScheme() == AppSetting::ColorScheme::Light);
      return;
    default:
      break;
    }
  }

  bool perform(juce::ApplicationCommandTarget::InvocationInfo const &info) override {
    switch (info.commandID) {
    case commandViewThemeAuto:
      setColorScheme(AppSetting::ColorScheme::Auto);
      return true;
    case commandViewThemeDark:
      setColorScheme(AppSetting::ColorScheme::Dark);
      return true;
    case commandViewThemeMidnight:
      setColorScheme(AppSetting::ColorScheme::Midnight);
      return true;
    case commandViewThemeGray:
      setColorScheme(AppSetting::ColorScheme::Gray);
      return true;
    case commandViewThemeLight:
      setColorScheme(AppSetting::ColorScheme::Light);
      return true;
    }
    return false;
  }

  void shutdown() override {
    fMainWindow = nullptr;
  }

  void systemRequestedQuit() override {
    juce::ApplicationCommandTarget::InvocationInfo info(commandFileExit);
    fCommandManager->invoke(info, false);
  }

  void anotherInstanceStarted(juce::String const &) override {
  }

  void darkModeSettingChanged() override {
    if (fSetting->getColorScheme() != AppSetting::ColorScheme::Auto) {
      return;
    }
    auto laf = std::make_unique<LookAndFeel>();
    laf->setColourScheme(fSetting->getColorScheme(juce::Desktop::getInstance().isDarkModeActive()));
    juce::LookAndFeel::setDefaultLookAndFeel(laf.get());
    fLaf.swap(laf);
  }

private:
  void setColorScheme(AppSetting::ColorScheme scheme) {
    fSetting->setColorScheme(scheme);
    auto laf = std::make_unique<LookAndFeel>();
    laf->setColourScheme(fSetting->getColorScheme(juce::Desktop::getInstance().isDarkModeActive()));
    juce::LookAndFeel::setDefaultLookAndFeel(laf.get());
    fLaf.swap(laf);
    fCommandManager->invoke(juce::ApplicationCommandTarget::InvocationInfo(commandUpdateMenuModel), false);
  }

private:
  std::unique_ptr<MainWindow> fMainWindow;
  std::unique_ptr<LookAndFeel> fLaf;
  std::shared_ptr<hb_font_t> fFont;
  std::shared_ptr<AppSetting> fSetting;
  std::unique_ptr<juce::ApplicationCommandManager> fCommandManager;
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
