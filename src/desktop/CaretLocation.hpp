#pragma once

namespace ksesh {

struct CaretLocation {
  int fLocation;
  Direction fDirection;
  CaretLocation(int location, Direction direction) : fLocation(location), fDirection(direction) {}
};

} // namespace ksesh
