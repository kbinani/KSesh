#pragma once

namespace ksesh {

class FilePath {
  FilePath() = delete;

  static juce::File ApplicationDataDirectory() {
#if JUCE_WINDOWS
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile(".ksesh");
#elif JUCE_MAC || JUCE_IOS
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("Application Support").getChildFile(".ksesh");
#endif
    return juce::File();
  }

  static juce::File CacheDirectory() {
    auto dir = ApplicationDataDirectory();
    return dir.getChildFile("cache");
  }

  static juce::File FontCacheDirectory() {
    auto cacheDir = CacheDirectory();
    return cacheDir.getChildFile("font").getChildFile("v1");
  }

public:
  static void PrepareApplicationDataDirectory() {
    auto dir = FontCacheDirectory();
#if JUCE_DEBUG
    juce::Logger::outputDebugString("application data directory: " + ApplicationDataDirectory().getFullPathName());
#endif
    dir.createDirectory();
  }

  static juce::File ConfigFilePath() {
    auto dir = FilePath::ApplicationDataDirectory();
    if (dir == juce::File()) {
      return juce::File();
    }
    return dir.getChildFile("config.json");
  }

  static juce::File FontCacheFilePath(void const *data, size_t size) {
    auto fontDir = FontCacheDirectory();
    juce::SHA256 hash(data, size);
    juce::String name = hash.toHexString() + "_" + juce::String(size) + ".gzip";
    return fontDir.getChildFile(name);
  }
};

} // namespace ksesh
