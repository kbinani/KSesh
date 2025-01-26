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
      juce::PopupMenu category;
      category.addCommandItem(fManager, commandEditSignListCategoryNext);
      category.addCommandItem(fManager, commandEditSignListCategoryPrev);
      category.addSeparator();
      category.addCommandItem(fManager, commandEditSignListCategoryA);
      category.addCommandItem(fManager, commandEditSignListCategoryB);
      category.addCommandItem(fManager, commandEditSignListCategoryC);
      category.addCommandItem(fManager, commandEditSignListCategoryD);
      category.addCommandItem(fManager, commandEditSignListCategoryE);
      category.addCommandItem(fManager, commandEditSignListCategoryF);
      category.addCommandItem(fManager, commandEditSignListCategoryG);
      category.addCommandItem(fManager, commandEditSignListCategoryH);
      category.addCommandItem(fManager, commandEditSignListCategoryI);
      category.addCommandItem(fManager, commandEditSignListCategoryK);
      category.addCommandItem(fManager, commandEditSignListCategoryL);
      category.addCommandItem(fManager, commandEditSignListCategoryM);
      category.addCommandItem(fManager, commandEditSignListCategoryN);
      category.addCommandItem(fManager, commandEditSignListCategoryO);
      category.addCommandItem(fManager, commandEditSignListCategoryP);
      category.addCommandItem(fManager, commandEditSignListCategoryQ);
      category.addCommandItem(fManager, commandEditSignListCategoryR);
      category.addCommandItem(fManager, commandEditSignListCategoryS);
      category.addCommandItem(fManager, commandEditSignListCategoryT);
      category.addCommandItem(fManager, commandEditSignListCategoryU);
      category.addCommandItem(fManager, commandEditSignListCategoryV);
      category.addCommandItem(fManager, commandEditSignListCategoryW);
      category.addCommandItem(fManager, commandEditSignListCategoryX);
      category.addCommandItem(fManager, commandEditSignListCategoryY);
      category.addCommandItem(fManager, commandEditSignListCategoryZ);
      category.addCommandItem(fManager, commandEditSignListCategoryAa);
      category.addCommandItem(fManager, commandEditSignListCategoryTall);
      category.addCommandItem(fManager, commandEditSignListCategoryWide);
      category.addCommandItem(fManager, commandEditSignListCategorySmall);
      menu.addSubMenu(TRANS("Sign list category"), category, true);
      juce::PopupMenu sign;
      sign.addCommandItem(fManager, commandEditSignListUseSign);
      sign.addSeparator();
      sign.addCommandItem(fManager, commandEditSignListMoveUp);
      sign.addCommandItem(fManager, commandEditSignListMoveRight);
      sign.addCommandItem(fManager, commandEditSignListMoveDown);
      sign.addCommandItem(fManager, commandEditSignListMoveLeft);
      menu.addSubMenu(TRANS("Sign"), sign, true);
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
