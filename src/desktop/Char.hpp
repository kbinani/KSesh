#pragma once

namespace ksesh {

struct Char : public CharBase {
  int fClusterIndex;
  int fCluster;
  Char(CharBase const &base, int clusterIndex, int cluster) : CharBase(base.fCh, base.fRaw, base.fRawOffset, base.fResultOffset, base.fCtrl, base.fSign), fClusterIndex(clusterIndex), fCluster(cluster) {}
};

} // namespace ksesh
