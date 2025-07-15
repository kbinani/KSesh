// clang-format off
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_cryptography/juce_cryptography.h>
#include "juce_TextEditor.h"
#include "PDFGen.hpp"
#include <iostream>
#include "defer.hpp"
#include <eglyf.hpp>

#include "String.hpp"
#include "Harfbuzz.hpp"
#include "FontAdapter.hpp"
#include "BinaryData.hpp"
#include "Insertions.hpp"
#include "SignList.hpp"
#include "PresentationSetting.hpp"
#include "CharBase.hpp"
#include "Char.hpp"
#include "Cluster.hpp"
#include "Line.hpp"
#include "Direction.hpp"
#include "LineAndCluster.hpp"
#include "CursorLocation.hpp"
#include "Cursor.hpp"
#include "CaretLocation.hpp"
#include "Content.hpp"
#include "FontFamily.hpp"
#include "FilePath.hpp"
#include "AppSetting.hpp"
#include "Clipboard.hpp"
#include "TextEditorComponent.hpp"
#include "HieroglyphComponent.hpp"
#include "Sign.hpp"
#include "SignListButtonContainer.hpp"
#include "BottomToolBar.hpp"
#include "SignListComponent.hpp"
#include "SplitterComponent.hpp"
#include "CommandID.hpp"
#include "MenuBarModel.hpp"
#include "AboutComponent.hpp"
#include "ExampleComponent.hpp"
#include "FontSet.hpp"
#include "MainComponent.hpp"
#include "MainWindow.hpp"
#include "LookAndFeel.hpp"
#include "FontLoaderComponent.hpp"
// clang-format on

using namespace std::literals::string_literals;

namespace ksesh {

class Application : public juce::JUCEApplication, public juce::DarkModeSettingListener, public FontLoaderComponent::Delegate {
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
    FilePath::PrepareApplicationDataDirectory();

    fSetting = std::make_shared<AppSetting>();

    juce::LocalisedStrings::setCurrentMappings(CurrentLocalisedStrings());

    fLaf = std::make_unique<LookAndFeel>();
    fLaf->setColourScheme(fSetting->getColorScheme(juce::Desktop::getInstance().isDarkModeActive()));
    juce::LookAndFeel::setDefaultLookAndFeel(fLaf.get());
    juce::Desktop::getInstance().addDarkModeSettingListener(this);

    fFontLoader = std::make_unique<FontLoaderComponent>(this);

    fCommandManager = std::make_unique<juce::ApplicationCommandManager>();
    fCommandManager->registerAllCommandsForTarget(this);
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
    case commandHelpAboutClose:
      info.setInfo("Close About dialog", {}, {}, juce::ApplicationCommandInfo::hiddenFromKeyEditor);
      return;
    case commandHelpExampleClose:
      info.setInfo("Close Example dialog", {}, {}, juce::ApplicationCommandInfo::hiddenFromKeyEditor);
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
    case commandHelpAboutClose:
      fMainWindow->closeAbout();
      return true;
    case commandHelpExampleClose:
      fMainWindow->closeExample();
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

  void fontLoaderComponentDidFinishLoadingFont(FontSet const &fontSet) override {
    auto loader = fFontLoader.release();
    if (loader) {
      loader->deleteAfterDelay(juce::RelativeTime::seconds(1), false);
    }

    fMainWindow = std::make_unique<MainWindow>(getApplicationName(), fCommandManager, fSetting, fontSet);
    fMainWindow->setVisible(true);
    fMainWindow->updateWindowTitle({}, false);
  }

  ApplicationCommandTarget *getNextCommandTarget() override {
    return fMainWindow.get();
  }

private:
  static juce::LocalisedStrings *CurrentLocalisedStrings() {
#if defined(JUCE_WINDOWS)
    LANGID lang = GetSystemDefaultUILanguage();
    if (lang == 0x0411) {
      return Japanese();
    }
    return nullptr;
#elif defined(JUCE_MAC)
    NSString *language = [[NSLocale preferredLanguages] firstObject];
    if ([language isEqualToString:@"ja-JP"]) {
      return Japanese();
    }
    return nullptr;
#else
    return nullptr;
#endif
  }

  static juce::LocalisedStrings *Japanese() {
    return new juce::LocalisedStrings(juce::String::fromUTF8(BinaryData::japanese_txt, BinaryData::japanese_txtSize), false);
  }

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
  std::shared_ptr<AppSetting> fSetting;
  std::unique_ptr<juce::ApplicationCommandManager> fCommandManager;
  std::unique_ptr<FontLoaderComponent> fFontLoader;
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
