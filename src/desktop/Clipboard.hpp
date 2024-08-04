#pragma once

namespace ksesh {

class Clipboard {
public:
  enum class Type {
#if JUCE_WINDOWS
    Emf,
#endif
    Png,
  };

  static bool Store(std::string_view data, Type type);
};

} // namespace ksesh
