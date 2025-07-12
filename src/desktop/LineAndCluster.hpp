#pragma once

namespace ksesh {

struct LineAndCluster {
  int lineIndex;
  int clusterIndex;
  LineAndCluster() : lineIndex(0), clusterIndex(0) {}
  LineAndCluster(int lineIndex, int clusterIndex) : lineIndex(lineIndex), clusterIndex(clusterIndex) {}
  LineAndCluster(LineAndCluster const &) = default;
  LineAndCluster &operator=(LineAndCluster const &) = default;
};

inline int compareLineAndCluster(LineAndCluster left, LineAndCluster right) {
  if (left.lineIndex == right.lineIndex) {
    return left.clusterIndex - right.clusterIndex;
  } else {
    return left.lineIndex - right.lineIndex;
  }
}

} // namespace ksesh
