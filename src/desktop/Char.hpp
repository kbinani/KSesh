#pragma once

namespace ksesh {

struct Char : public CharBase {
  int clusterIndex;
  int cluster;
  Char(CharBase const &base, int clusterIndex, int cluster) : CharBase(base.ch, base.raw, base.rawOffset, base.resultOffset, base.ctrl, base.sign), clusterIndex(clusterIndex), cluster(cluster) {}
};

} // namespace ksesh
