#pragma once

namespace ksesh {

struct AppSetting {
public:
  enum ColorScheme {
    Auto = -1,
    Dark = 0,
    Midnight = 1,
    Gray = 2,
    Light = 3,
  };
  ColorScheme colorScheme = Light;

  juce::LookAndFeel_V4::ColourScheme getColorScheme(bool darkModeActive) {
    switch (colorScheme) {
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
};

} // namespace ksesh
