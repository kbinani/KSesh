#pragma once

namespace ksesh {

struct Sign {
  juce::String fName;
  juce::String fMdcFirst;
  juce::String fMdcTrailing;
  std::shared_ptr<juce::Path> fPath;
};

} // namespace ksesh
