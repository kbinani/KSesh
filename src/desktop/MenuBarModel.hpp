#pragma once

#pragma GCC diagnostic ignored "-Wunused-private-field"

namespace ksesh {

class MenuBarModel : public juce::MenuBarModel {
  enum : int {
    recentFilesMenuIdOffset = 1000,
  };

public:
  MenuBarModel(juce::ApplicationCommandManager *manager, std::shared_ptr<AppSetting> setting) : fManager(manager), fSetting(setting) {}

  juce::StringArray getMenuBarNames() override {
    juce::StringArray names;
    names.add(TRANS("File"));
    names.add(TRANS("Edit"));
    names.add(TRANS("View"));
    names.add(TRANS("Help"));
    return names;
  }

  juce::PopupMenu getMenuForIndex(int topLevelMenuIndex, juce::String const &menuName) override {
    juce::PopupMenu menu;
    if (topLevelMenuIndex == 0) {
      // File
      menu.addCommandItem(fManager, commandFileNew);
      menu.addCommandItem(fManager, commandFileOpen);
      juce::PopupMenu recent;
      fRecentFiles = fSetting->createRecentFilesMenu(recent, recentFilesMenuIdOffset);
      menu.addSubMenu(TRANS("Open Recent"), recent, recent.getNumItems() > 0);
      menu.addSeparator();
      menu.addCommandItem(fManager, commandFileSave);
      menu.addCommandItem(fManager, commandFileSaveAs);
      menu.addSeparator();
      juce::PopupMenu png;
      png.addCommandItem(fManager, commandFileExportAsPng1x);
      png.addCommandItem(fManager, commandFileExportAsPng2x);
      png.addCommandItem(fManager, commandFileExportAsPng4x);
      png.addCommandItem(fManager, commandFileExportAsPng8x);
      menu.addSubMenu(TRANS("Export as PNG"), png, true);
#if defined(JUCE_WINDOWS)
      menu.addCommandItem(fManager, commandFileExportAsEmf);
#endif
      menu.addCommandItem(fManager, commandFileExportAsPdf);
#if !defined(JUCE_MAC)
      menu.addSeparator();
      menu.addCommandItem(fManager, commandFileExit);
#endif
    } else if (topLevelMenuIndex == 1) {
      // Edit
      menu.addCommandItem(fManager, commandEditCopyAsUnicodeWithoutFormatControl);
      menu.addCommandItem(fManager, commandEditCopyAsUnicodeWithFormatControl);
#if defined(JUCE_WINDOWS)
      menu.addCommandItem(fManager, commandEditCopyAsEmf);
#endif
#if defined(JUCE_MAC)
      menu.addCommandItem(fManager, commandEditCopyAsPdf);
#endif
      juce::PopupMenu png;
      png.addCommandItem(fManager, commandEditCopyAsImage1x);
      png.addCommandItem(fManager, commandEditCopyAsImage2x);
      png.addCommandItem(fManager, commandEditCopyAsImage4x);
      png.addCommandItem(fManager, commandEditCopyAsImage8x);
      menu.addSubMenu(TRANS("Copy as Image"), png, true);
      menu.addSeparator();
      menu.addCommandItem(fManager, commandEditSwitchFocus);
      juce::PopupMenu tab;
      tab.addCommandItem(fManager, commandEditSignListCategoryNext);
      tab.addCommandItem(fManager, commandEditSignListCategoryPrev);
      tab.addSeparator();
      tab.addCommandItem(fManager, commandEditSignListCategoryA);
      tab.addCommandItem(fManager, commandEditSignListCategoryB);
      tab.addCommandItem(fManager, commandEditSignListCategoryC);
      tab.addCommandItem(fManager, commandEditSignListCategoryD);
      tab.addCommandItem(fManager, commandEditSignListCategoryE);
      tab.addCommandItem(fManager, commandEditSignListCategoryF);
      tab.addCommandItem(fManager, commandEditSignListCategoryG);
      tab.addCommandItem(fManager, commandEditSignListCategoryH);
      tab.addCommandItem(fManager, commandEditSignListCategoryI);
      tab.addCommandItem(fManager, commandEditSignListCategoryK);
      tab.addCommandItem(fManager, commandEditSignListCategoryL);
      tab.addCommandItem(fManager, commandEditSignListCategoryM);
      tab.addCommandItem(fManager, commandEditSignListCategoryN);
      tab.addCommandItem(fManager, commandEditSignListCategoryO);
      tab.addCommandItem(fManager, commandEditSignListCategoryP);
      tab.addCommandItem(fManager, commandEditSignListCategoryQ);
      tab.addCommandItem(fManager, commandEditSignListCategoryR);
      tab.addCommandItem(fManager, commandEditSignListCategoryS);
      tab.addCommandItem(fManager, commandEditSignListCategoryT);
      tab.addCommandItem(fManager, commandEditSignListCategoryU);
      tab.addCommandItem(fManager, commandEditSignListCategoryV);
      tab.addCommandItem(fManager, commandEditSignListCategoryW);
      tab.addCommandItem(fManager, commandEditSignListCategoryX);
      tab.addCommandItem(fManager, commandEditSignListCategoryY);
      tab.addCommandItem(fManager, commandEditSignListCategoryZ);
      tab.addCommandItem(fManager, commandEditSignListCategoryAa);
      tab.addCommandItem(fManager, commandEditSignListCategoryTall);
      tab.addCommandItem(fManager, commandEditSignListCategoryWide);
      tab.addCommandItem(fManager, commandEditSignListCategorySmall);
      menu.addSubMenu(TRANS("Switch sign list category"), tab, true);
    } else if (topLevelMenuIndex == 2) {
      // View
      menu.addCommandItem(fManager, commandViewTogglePreviewVisibility);
      juce::PopupMenu theme;
      theme.addCommandItem(fManager, commandViewThemeAuto);
      theme.addCommandItem(fManager, commandViewThemeDark);
      theme.addCommandItem(fManager, commandViewThemeMidnight);
      theme.addCommandItem(fManager, commandViewThemeGray);
      theme.addCommandItem(fManager, commandViewThemeLight);
      menu.addSubMenu(TRANS("Theme"), theme, true);
    } else if (topLevelMenuIndex == 3) {
      // Help
      menu.addCommandItem(fManager, commandHelpExample);
      menu.addSeparator();
      menu.addCommandItem(fManager, commandHelpAbout);
    }
    return menu;
  }

  void menuItemSelected(int menuItemID, int topLevelMenuIndex) override {
    if (topLevelMenuIndex == 0) {
      int index = menuItemID - recentFilesMenuIdOffset;
      if (0 <= index && index < (int)fRecentFiles.size()) {
        if (onRecentFileClicked) {
          auto file = fRecentFiles[index];
          fSetting->addToRecentFile(file);
          onRecentFileClicked(file);
        }
      }
    }
  }

public:
  std::function<void(juce::File)> onRecentFileClicked;

private:
  juce::ApplicationCommandManager *const fManager;
  std::shared_ptr<AppSetting> fSetting;
  std::vector<juce::File> fRecentFiles;
};

} // namespace ksesh
