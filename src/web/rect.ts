import { Point } from "./content";

export class Rect {
  constructor(
    readonly x: number,
    readonly y: number,
    readonly width: number,
    readonly height: number,
  ) {}

  scaled(scale: number): Rect {
    return new Rect(
      this.x * scale,
      this.y * scale,
      this.width * scale,
      this.height * scale,
    );
  }

  get maxX(): number {
    return this.x + this.width;
  }

  get maxY(): number {
    return this.y + this.height;
  }

  inflated(size: number): Rect {
    return new Rect(
      this.x - size,
      this.y - size,
      this.width + 2 * size,
      this.height + 2 * size,
    );
  }

  get center(): Point {
    return { x: this.x + this.width * 0.5, y: this.y + this.height * 0.5 };
  }
}
