#pragma once

namespace ksesh {

class Clipboard {
public:
  enum class Type {
#if JUCE_WINDOWS
    Emf,
#endif
    Png,
#if JUCE_MAC
    Pdf,
#endif
  };

  static bool Store(std::string_view data, Type type);
};

} // namespace ksesh
