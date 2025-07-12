#pragma once

namespace ksesh {

struct Cluster {
  int resultOffset;
  int cluster;
  std::optional<juce::Rectangle<float>> bounds;
  Cluster(int resultOffset, std::optional<juce::Rectangle<float>> bounds, int cluster) : resultOffset(resultOffset), cluster(cluster), bounds(bounds) {}
};

} // namespace ksesh
