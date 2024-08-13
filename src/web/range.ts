export type Range = { lowerBound: number; upperBound: number };

export function rangeIntersectsWith(x: Range, y: Range): boolean {
  if (y.lowerBound <= x.lowerBound && x.lowerBound < y.upperBound) {
    return true;
  } else {
    return x.lowerBound <= y.lowerBound && y.lowerBound < x.upperBound;
  }
}
