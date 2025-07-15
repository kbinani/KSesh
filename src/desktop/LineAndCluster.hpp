#pragma once

namespace ksesh {

struct LineAndCluster {
  int fLineIndex;
  int fClusterIndex;
  LineAndCluster() : fLineIndex(0), fClusterIndex(0) {}
  LineAndCluster(int lineIndex, int clusterIndex) : fLineIndex(lineIndex), fClusterIndex(clusterIndex) {}
  LineAndCluster(LineAndCluster const &) = default;
  LineAndCluster &operator=(LineAndCluster const &) = default;
};

inline int compareLineAndCluster(LineAndCluster left, LineAndCluster right) {
  if (left.fLineIndex == right.fLineIndex) {
    return left.fClusterIndex - right.fClusterIndex;
  } else {
    return left.fLineIndex - right.fLineIndex;
  }
}

} // namespace ksesh
