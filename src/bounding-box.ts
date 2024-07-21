import { Rect } from "./rect";

export class BoundingBox {
  private _rect: Rect | undefined = undefined;

  get rect(): Rect | undefined {
    if (this._rect) {
      return new Rect(
        this._rect.x,
        this._rect.y,
        this._rect.width,
        this._rect.height,
      );
    } else {
      return undefined;
    }
  }

  add(rect: Rect): void;
  add(x: number, y: number, width: number, height: number): void;
  add(
    xOrRect: Rect | number,
    y?: number,
    width?: number,
    height?: number,
  ): void {
    let input: Rect;
    if (typeof xOrRect === "number") {
      input = new Rect(xOrRect, y!, width!, height!);
    } else {
      input = xOrRect;
    }
    if (this._rect) {
      const minX = Math.min(input.x, this._rect.x);
      const minY = Math.min(input.y, this._rect.y);
      const maxX = Math.max(
        input.x + input.width,
        this._rect.x + this._rect.width,
      );
      const maxY = Math.max(
        input.y + input.height,
        this._rect.y + this._rect.height,
      );
      this._rect = new Rect(minX, minY, maxX - minX, maxY - minY);
    } else {
      this._rect = new Rect(input.x, input.y, input.width, input.height);
    }
  }
}
