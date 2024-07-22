import { bufferFromText, enumerateGlyphs } from "./export";
import { FontData } from "./font-data";
import { Rect } from "./rect";
import { BoundingBox } from "./bounding-box";
import { rangeIntersectsWith } from "./range";
import { HarfBuzzBuffer } from "./harfbuzz";
import { SignList } from "./sign-list";

type Char = { char: string; index: number };

type Cluster = {
  index: number;
  bounds?: Rect;
};

export class Content {
  readonly raw: string;
  readonly result: string;
  readonly clusters: ReadonlyArray<Readonly<Cluster>>;
  readonly boundingBox: Rect | undefined;
  readonly unitsPerEm: number;
  readonly chars: ReadonlyArray<Readonly<Char>>;
  readonly buffer: HarfBuzzBuffer;

  constructor(raw: string, font: FontData) {
    this.raw = raw;
    let chars: Char[] = [];
    for (let offset = 0; offset < raw.length; ) {
      const map = SignList.map(raw, offset);
      if (map === undefined) {
        chars.push({ char: raw.substring(offset, offset + 1), index: offset });
        offset += 1;
      } else {
        if (map[1].length > 0) {
          chars.push({ char: map[1], index: offset });
        }
        offset = offset + map[0].length;
      }
    }
    this.chars = chars;
    this.result = chars.map((c) => c.char).join("");
    const e = new TextEncoder();
    const utf8 = e.encode(this.result);
    const buffer = bufferFromText(this.result, font);
    const clusters: Cluster[] = [];
    let lastCluster = 0;
    let offset = 0;
    let bb = new BoundingBox();
    const d = new TextDecoder();
    let maxX = 0;
    for (const { x, y, info } of enumerateGlyphs(buffer, font)) {
      const glyph = font.ot.glyphs.get(info.GlyphId);
      const p = glyph.getPath(x, y, font.ot.unitsPerEm);
      const bounds = p.getBoundingBox();
      maxX = Math.max(maxX, bounds.x2);
      if (info.Cluster !== lastCluster) {
        const u16 = d.decode(utf8.slice(lastCluster, info.Cluster));
        clusters.push({ index: offset, bounds: bb.rect });
        offset += u16.length;
        lastCluster = info.Cluster;
        bb = new BoundingBox();
      }
      if (p.commands.length > 0) {
        bb.add(
          bounds.x1,
          bounds.y1,
          bounds.x2 - bounds.x1,
          bounds.y2 - bounds.y1,
        );
      }
    }
    this.buffer = buffer;
    if (lastCluster < utf8.length) {
      clusters.push({ index: offset, bounds: bb.rect });
    }
    this.clusters = clusters;

    const { ascender, descender, unitsPerEm } = font.ot;
    this.boundingBox = new Rect(
      0,
      0,
      maxX / unitsPerEm,
      (ascender - descender) / unitsPerEm,
    );
    this.unitsPerEm = font.ot.unitsPerEm;
  }

  destroy() {
    this.buffer.destroy();
  }

  cursor({
    fontSize,
    selectionStart,
    selectionEnd,
  }: {
    fontSize: number;
    selectionStart: number;
    selectionEnd: number;
  }): Rect | undefined {
    let start: number | undefined;
    let end: number | undefined;
    let offset = 0;
    if (selectionStart === selectionEnd) {
      start = 0;
      for (let i = 0; i < this.chars.length; i++) {
        const char = this.chars[i];
        if (char.index < selectionStart) {
          start = offset;
        } else {
          break;
        }
        offset += char.char.length;
      }
      end = start;
    } else {
      for (let i = 0; i < this.chars.length; i++) {
        const char = this.chars[i];
        const from = char.index;
        const to =
          i + 1 < this.chars.length ? this.chars[i + 1].index : this.raw.length;
        if (
          start === undefined &&
          from <= selectionStart &&
          selectionStart < to
        ) {
          start = offset;
        }
        offset += char.char.length;
        if (
          start !== undefined &&
          end === undefined &&
          from <= selectionEnd &&
          selectionEnd <= to
        ) {
          end = offset;
          break;
        }
      }
    }
    if (start === undefined || end === undefined) {
      return undefined;
    }
    const bounds = new BoundingBox();
    for (let i = 0; i < this.clusters.length; i++) {
      const cluster = this.clusters[i];
      if (!cluster.bounds) {
        continue;
      }
      const lowerBound = cluster.index;
      const upperBound =
        i + 1 < this.clusters.length
          ? this.clusters[i + 1].index
          : this.result.length;
      if (
        rangeIntersectsWith(
          { lowerBound: start, upperBound: end },
          { lowerBound, upperBound },
        )
      ) {
        bounds.add(cluster.bounds);
      }
    }
    const rect = bounds.rect;
    if (!rect) {
      return undefined;
    }
    const scale = fontSize / this.unitsPerEm;
    return new Rect(
      rect.x * scale,
      rect.y * scale,
      rect.width * scale,
      rect.height * scale,
    );
  }

  get plainText(): string {
    let ret = this.result;
    for (let code = 0x13430; code <= 0x13455; code++) {
      ret = ret.replaceAll(String.fromCodePoint(code), "");
    }
    return ret;
  }
}
