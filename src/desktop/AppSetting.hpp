#pragma once

namespace ksesh {

class AppSetting : public juce::ChangeBroadcaster {
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
    load();
  }

  ColorScheme getColorScheme() const {
    return fColorScheme;
  }

  void setColorScheme(ColorScheme scheme) {
    fColorScheme = scheme;
    sendSynchronousChangeMessage();
    save();
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
    sendSynchronousChangeMessage();
    save();
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

  float getEditorFontSize() const {
    return fEditorFontSize;
  }

  void setEditorFontSize(float s) {
    if (fEditorFontSize != s) {
      fEditorFontSize = s;
      sendSynchronousChangeMessage();
    }
  }

  PresentationSetting getPresentationSetting() const {
    return fPresentation;
  }

  void setPresentationSetting(PresentationSetting s) {
    fPresentation = s;
    sendSynchronousChangeMessage();
  }

private:
  void save() {
    auto file = ConfigFilePath();
    if (file == juce::File()) {
      return;
    }
    if (!file.getParentDirectory().exists()) {
      file.getParentDirectory().createDirectory();
    }
    auto stream = file.createOutputStream();
    if (!stream || stream->failedToOpen()) {
      return;
    }
    if (!stream->setPosition(0)) {
      return;
    }
    if (stream->truncate().failed()) {
      return;
    }
    juce::String colorScheme = "auto";
    switch (fColorScheme) {
    case ColorScheme::Auto:
      colorScheme = "auto";
      break;
    case ColorScheme::Dark:
      colorScheme = "dark";
      break;
    case ColorScheme::Gray:
      colorScheme = "gray";
      break;
    case ColorScheme::Light:
      colorScheme = "light";
      break;
    case ColorScheme::Midnight:
      colorScheme = "midnight";
      break;
    }
    juce::var obj = juce::JSON::fromString("{}");
    auto o = obj.getDynamicObject();
    if (!o) {
      return;
    }
    o->setProperty(juce::Identifier("color_scheme"), colorScheme);
    juce::StringArray items;
    for (int i = 0; i < fRecentFiles.getNumFiles(); i++) {
      auto file = fRecentFiles.getFile(i);
#if defined(JUCE_MAC)
      if (auto item = Bookmark(file); item.isNotEmpty()) {
        items.add(item);
      }
#else
      items.add(file.getFullPathName());
#endif
    }
    o->setProperty(juce::Identifier("recent_files"), items.joinIntoString("\n"));
    juce::JSON::writeToStream(*stream, obj, {});
  }

  void load() {
    auto file = ConfigFilePath();
    if (file == juce::File()) {
      return;
    }
    auto stream = file.createInputStream();
    if (!stream || stream->failedToOpen()) {
      return;
    }
    auto obj = juce::JSON::parse(*stream);
    auto o = obj.getDynamicObject();
    if (!o) {
      return;
    }
    juce::var colorScheme = o->getProperty(juce::Identifier("color_scheme"));
    if (colorScheme.isString()) {
      juce::String cs = colorScheme;
      if (cs == "auto") {
        fColorScheme = ColorScheme::Auto;
      } else if (cs == "dark") {
        fColorScheme = ColorScheme::Dark;
      } else if (cs == "gray") {
        fColorScheme = ColorScheme::Gray;
      } else if (cs == "light") {
        fColorScheme = ColorScheme::Light;
      } else if (cs == "midnight") {
        fColorScheme = ColorScheme::Midnight;
      }
    }
    juce::var recentFiles = o->getProperty(juce::Identifier("recent_files"));
    if (recentFiles.isString()) {
      juce::String rf = recentFiles;
      auto items = juce::StringArray::fromLines(rf);
      for (int i = items.size() - 1; i >= 0; i--) {
        auto const &item = items[i];
#if defined(JUCE_MAC)
        if (auto file = FromBookmark(item); file != juce::File()) {
          fRecentFiles.addFile(file);
        }
#else
        fRecentFiles.addFile(juce::File(item));
#endif
      }
    }
  }

#if defined(JUCE_MAC)
  static juce::String Bookmark(juce::File const &f) {
    juce::URL u(f);
    juce::String s = u.toString(false);
    NSString *ns = [[NSString alloc] initWithUTF8String:s.toRawUTF8()];
    NSURL *nsU = [[NSURL alloc] initWithString:ns];
    NSError *e = nullptr;
    NSData *data = [nsU bookmarkDataWithOptions:NSURLBookmarkCreationWithSecurityScope includingResourceValuesForKeys:nullptr relativeToURL:nullptr error:&e];
    if (e != nullptr) {
      return juce::String();
    }
    NSString *b64 = [data base64EncodedStringWithOptions:0];
    juce::String b64s = juce::String::fromUTF8([b64 UTF8String]);
    return b64s;
  }

  static juce::File FromBookmark(juce::String const &s) {
    NSString *b64 = [[NSString alloc] initWithUTF8String:s.toRawUTF8()];
    NSData *data = [[NSData alloc] initWithBase64EncodedString:b64 options:0];
    NSError *e = nullptr;
    NSURL *nsU = [NSURL URLByResolvingBookmarkData:data options:NSURLBookmarkResolutionWithSecurityScope relativeToURL:nullptr bookmarkDataIsStale:nullptr error:&e];
    if (e != nullptr) {
      return juce::File();
    }
    [nsU startAccessingSecurityScopedResource];
    NSString *ns = [nsU absoluteString];
    juce::String ss = juce::String::fromUTF8([ns UTF8String]);
    juce::URL u(ss);
    return u.getLocalFile();
  }
#endif

  juce::File ConfigFilePath() {
    auto dir = ConfigFileDirectory();
    if (dir == juce::File()) {
      return juce::File();
    }
    return dir.getChildFile("config.json");
  }

  juce::File ConfigFileDirectory() {
#if defined(JUCE_WINDOWS)
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile(".ksesh");
#elif defined(JUCE_MAC)
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("Application Support").getChildFile(".ksesh");
#endif
    return juce::File();
  }

private:
  ColorScheme fColorScheme = Light;
  juce::RecentlyOpenedFilesList fRecentFiles;
  PresentationSetting fPresentation;
  float fEditorFontSize = 24;
};

} // namespace ksesh
