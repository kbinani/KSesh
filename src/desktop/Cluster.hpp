#pragma once

namespace ksesh {

struct Cluster {
  int fResultOffset;
  int fCluster;
  std::optional<juce::Rectangle<float>> fBounds;
  Cluster(int resultOffset, std::optional<juce::Rectangle<float>> bounds, int cluster) : fResultOffset(resultOffset), fCluster(cluster), fBounds(bounds) {}
};

} // namespace ksesh
