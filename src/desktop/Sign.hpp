#pragma once

namespace ksesh {

struct Sign {
  juce::String name;
  juce::String mdcFirst;
  juce::String mdcTrailing;
  std::shared_ptr<juce::Path> path;
};

} // namespace ksesh
