#pragma once

#pragma GCC diagnostic ignored "-Wunused-private-field"

namespace ksesh {

class MenuBarModel : public juce::MenuBarModel {
public:
  explicit MenuBarModel(juce::ApplicationCommandManager *manager) : fManager(manager) {}

  juce::StringArray getMenuBarNames() override {
    juce::StringArray names;
    names.add(TRANS(u8"File"));
    return names;
  }

  juce::PopupMenu getMenuForIndex(int topLevelMenuIndex, juce::String const &menuName) override {
    juce::PopupMenu menu;
    if (topLevelMenuIndex == 0) {
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
      menu.addCommandItem(fManager, commandFileExportAsPdf);
#if !JUCE_MAC
      menu.addSeparator();
      menu.addCommandItem(fManager, commandFileExit);
#endif
    }
    return menu;
  }

  void menuItemSelected(int menuItemID, int topLevelMenuIndex) override {
  }

private:
  juce::ApplicationCommandManager *const fManager;
};

} // namespace ksesh
