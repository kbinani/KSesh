#pragma once

namespace ksesh {

struct Sign {
  juce::String name;
  std::vector<juce::String> mcd;
  std::shared_ptr<juce::Path> path;
};

} // namespace ksesh
