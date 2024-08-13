//@ts-ignore
import { type opentype, parse } from "opentype.js/dist/opentype.module.js";
import { HarfBuzzBlob, HarfBuzzFace, HarfBuzzFont } from "./harfbuzz";

export class FontData {
  readonly ot: opentype.Font;
  readonly hb: HarfBuzzFont;
  private readonly face: HarfBuzzFace;
  private readonly blob: HarfBuzzBlob;

  constructor(buffer: Uint8Array) {
    this.ot = parse(buffer.buffer);
    this.blob = new HarfBuzzBlob(buffer);
    this.face = new HarfBuzzFace(this.blob, 0);
    this.hb = new HarfBuzzFont(this.face);
  }

  destroy() {
    this.hb.destroy();
    this.face.destroy();
    this.blob.destroy();
  }
}
