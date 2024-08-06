#pragma once

namespace ksesh {

class AppSetting {
public:
  enum ColorScheme {
    Auto = -1,
    Dark = 0,
    Midnight = 1,
    Gray = 2,
    Light = 3,
  };

  AppSetting() {
    fRecentFiles.setMaxNumberOfItems(10);
  }

  ColorScheme getColorScheme() const {
    return fColorScheme;
  }

  void setColorScheme(ColorScheme scheme) {
    fColorScheme = scheme;
  }

  std::vector<juce::File> createRecentFilesMenu(juce::PopupMenu &menu, int baseId) {
    fRecentFiles.removeNonExistentFiles();
    std::vector<juce::File> files;
    for (int i = 0; i < fRecentFiles.getNumFiles(); i++) {
      files.push_back(fRecentFiles.getFile(i));
    }
    fRecentFiles.createPopupMenuItems(menu, baseId, true, false);
    return files;
  }

  void addToRecentFile(juce::File const &file) {
    fRecentFiles.addFile(file);
  }

  juce::LookAndFeel_V4::ColourScheme getColorScheme(bool darkModeActive) {
    switch (fColorScheme) {
    case Dark:
      return juce::LookAndFeel_V4::getDarkColourScheme();
    case Midnight:
      return juce::LookAndFeel_V4::getMidnightColourScheme();
    case Gray:
      return juce::LookAndFeel_V4::getGreyColourScheme();
    case Light:
      return juce::LookAndFeel_V4::getLightColourScheme();
    case Auto:
    default:
      if (darkModeActive) {
        return juce::LookAndFeel_V4::getDarkColourScheme();
      } else {
        return juce::LookAndFeel_V4::getLightColourScheme();
      }
    }
  }

private:
  ColorScheme fColorScheme = Light;
  juce::RecentlyOpenedFilesList fRecentFiles;
};

} // namespace ksesh
