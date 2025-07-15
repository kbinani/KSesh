#pragma once

namespace ksesh {

struct CursorLocationLeft : public LineAndCluster {
  CursorLocationLeft(int lineIndex, int clusterIndex) : LineAndCluster(lineIndex, clusterIndex) {}
};

struct CursorLocationRight : public LineAndCluster {
  bool fBlock;
  CursorLocationRight(int lineIndex, int clusterIndex, bool block) : LineAndCluster(lineIndex, clusterIndex), fBlock(block) {}
};

struct CursorLocationEnd {
  int fLineIndex;
  explicit CursorLocationEnd(int lineIndex) : fLineIndex(lineIndex) {}
};

using CursorLocation = std::variant<CursorLocationLeft, CursorLocationRight, CursorLocationEnd>;

} // namespace ksesh
