#pragma once

#pragma GCC diagnostic ignored "-Wunused-private-field"

namespace ksesh {

class MenuBarModel : public juce::MenuBarModel {
public:
  explicit MenuBarModel(juce::ApplicationCommandManager *manager) : fManager(manager) {}

  juce::StringArray getMenuBarNames() override {
    juce::StringArray names;
    names.add(TRANS("File"));
    names.add(TRANS("Edit"));
    names.add(TRANS("View"));
    return names;
  }

  juce::PopupMenu getMenuForIndex(int topLevelMenuIndex, juce::String const &menuName) override {
    juce::PopupMenu menu;
    if (topLevelMenuIndex == 0) {
      menu.addCommandItem(fManager, commandFileNew);
      menu.addCommandItem(fManager, commandFileOpen);
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
#if JUCE_WINDOWS
      menu.addCommandItem(fManager, commandFileExportAsEmf);
#endif
      menu.addCommandItem(fManager, commandFileExportAsPdf);
#if !JUCE_MAC
      menu.addSeparator();
      menu.addCommandItem(fManager, commandFileExit);
#endif
    } else if (topLevelMenuIndex == 1) {
#if JUCE_WINDOWS
      menu.addCommandItem(fManager, commandEditCopyAsEmf);
#endif
      juce::PopupMenu png;
      png.addCommandItem(fManager, commandEditCopyAsImage1x);
      png.addCommandItem(fManager, commandEditCopyAsImage2x);
      png.addCommandItem(fManager, commandEditCopyAsImage4x);
      png.addCommandItem(fManager, commandEditCopyAsImage8x);
      menu.addSubMenu(TRANS("Copy as Image"), png, true);
    } else if (topLevelMenuIndex == 2) {
      juce::PopupMenu theme;
      theme.addCommandItem(fManager, commandViewThemeAuto);
      theme.addCommandItem(fManager, commandViewThemeDark);
      theme.addCommandItem(fManager, commandViewThemeMidnight);
      theme.addCommandItem(fManager, commandViewThemeGray);
      theme.addCommandItem(fManager, commandViewThemeLight);
      menu.addSubMenu(TRANS("Theme"), theme, true);
    }
    return menu;
  }

  void menuItemSelected(int menuItemID, int topLevelMenuIndex) override {
  }

private:
  juce::ApplicationCommandManager *const fManager;
};

} // namespace ksesh
