#pragma once

namespace ksesh {

struct CursorLocationLeft : public LineAndCluster {
  CursorLocationLeft(int lineIndex, int clusterIndex) : LineAndCluster(lineIndex, clusterIndex) {}
};

struct CursorLocationRight : public LineAndCluster {
  bool block;
  CursorLocationRight(int lineIndex, int clusterIndex, bool block) : LineAndCluster(lineIndex, clusterIndex), block(block) {}
};

struct CursorLocationEnd {
  int lineIndex;
  explicit CursorLocationEnd(int lineIndex) : lineIndex(lineIndex) {}
};

using CursorLocation = std::variant<CursorLocationLeft, CursorLocationRight, CursorLocationEnd>;

} // namespace ksesh
