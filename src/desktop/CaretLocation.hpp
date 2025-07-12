#pragma once

namespace ksesh {

struct CaretLocation {
  int location;
  Direction direction;
  CaretLocation(int location, Direction direction) : location(location), direction(direction) {}
};

} // namespace ksesh
