#pragma once

namespace ksesh {

inline juce::String JuceStringFromU32String(std::u32string const &s) {
  return juce::String(juce::CharPointer_UTF32((juce::juce_wchar *)s.c_str()), juce::CharPointer_UTF32((juce::juce_wchar *)(s.c_str() + s.size())));
}

inline juce::String JuceStringFromU8String(std::u8string const &s) {
  return juce::String::fromUTF8((char const *)s.c_str(), s.size());
}

inline std::u8string U8StringFromJuceString(juce::String const &s) {
  return std::u8string((char8_t const *)s.toRawUTF8());
}

inline std::u8string U8StringFromU32String(std::u32string const &s) {
  return U8StringFromJuceString(JuceStringFromU32String(s));
}

inline std::u32string U32StringFromJuceString(juce::String const &s) {
  return std::u32string((char32_t const *)s.toUTF32().getAddress());
}

inline std::u32string U32StringFromU8string(std::u8string const &s) {
  return U32StringFromJuceString(juce::String::fromUTF8(s.c_str()));
}

} // namespace ksesh
