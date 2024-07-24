import { bufferFromText, EdgeInset, enumerateGlyphs } from "./export";
import { FontData } from "./font-data";
import { Rect } from "./rect";
import { BoundingBox } from "./bounding-box";
import { HarfBuzzBuffer } from "./harfbuzz";
import { SignList } from "./sign-list";
import { Direction } from "./component/app";

export type Cursor = { rect: Rect | undefined; selectionRects: Rect[] };
type CursorLocation = {
  lineIndex: number;
  clusterIndex: number;
  type: "right" | "left";
  block: boolean;
};

export class Content {
  readonly lines: ReadonlyArray<Line>;

  constructor(
    readonly raw: string,
    font: FontData,
  ) {
    const lines: Line[] = [];
    let offset = 0;
    for (const line of raw.split("\n")) {
      const l = new Line(offset, line, font);
      lines.push(l);
      offset += 1 + line.length;
    }
    this.lines = lines;
  }

  destroy() {
    this.lines.forEach((line) => line.destroy());
  }

  private cursorLocation({
    location,
    font,
    fontSize,
    lineSpacing,
    edgeInset,
    direction,
  }: {
    location: number;
    font: FontData;
    fontSize: number;
    lineSpacing: number;
    edgeInset: EdgeInset;
    direction: Direction;
  }): CursorLocation | undefined {
    let lineIndex: number | undefined;
    let type: "left" | "right" | undefined;
    let clusterIndex: number | undefined;
    let block = false;
    for (let j = 0; j < this.lines.length; j++) {
      const line = this.lines[j];
      if (location < line.unicodeOffset) {
        continue;
      } else if (line.unicodeOffset + line.raw.length < location) {
        continue;
      }
      if (location === line.unicodeOffset) {
        lineIndex = j;
        clusterIndex = 0;
        type = "left";
        break;
      }
      let nearest = -1;
      for (let i = 0; i < line.chars.length; i++) {
        const char = line.chars[i];
        const from = line.unicodeOffset + char.rawOffset;
        const to =
          line.unicodeOffset +
          (i + 1 < line.chars.length
            ? line.chars[i + 1].rawOffset
            : line.raw.length);
        if (location < from && nearest < 0) {
          nearest = i;
        }
        if (from === location && !char.ctrl) {
          lineIndex = j;
          type = "left";
          clusterIndex = char.cluster;
          break;
        } else if (to === location && !char.ctrl) {
          lineIndex = j;
          clusterIndex = char.cluster;
          type = "right";
          block = true;
          break;
        } else if (from < location && location < to) {
          lineIndex = j;
          if (location === from) {
            type = "left";
          } else {
            type = "right";
          }
          clusterIndex = char.cluster;
          block = true;
          break;
        }
      }
      if (type === undefined) {
        const char = line.chars[nearest];
        if (direction === "forward") {
          lineIndex = j;
          clusterIndex = char.cluster;
          type = "left";
        } else {
          lineIndex = j;
          clusterIndex = Math.max(0, char.cluster - 1);
          type = "right";
        }
      }
    }
    if (
      lineIndex === undefined ||
      clusterIndex === undefined ||
      type === undefined
    ) {
      return undefined;
    } else {
      return { lineIndex, clusterIndex, type, block };
    }
  }

  cursor({
    selectionStart,
    selectionEnd,
    font,
    fontSize,
    lineSpacing,
    edgeInset,
    direction,
  }: {
    selectionStart: number;
    selectionEnd: number;
    font: FontData;
    fontSize: number;
    lineSpacing: number;
    edgeInset: EdgeInset;
    direction: Direction;
  }): Cursor {
    const scale = fontSize / font.ot.unitsPerEm;
    if (selectionStart === selectionEnd) {
      const location = this.cursorLocation({
        location: selectionStart,
        font,
        fontSize,
        lineSpacing,
        edgeInset,
        direction,
      });
      if (location === undefined) {
        return { rect: undefined, selectionRects: [] };
      } else {
        const { lineIndex, clusterIndex, type, block } = location;
        const dx = edgeInset.left;
        const dy = edgeInset.top + (fontSize + lineSpacing) * lineIndex;
        const line = this.lines[lineIndex];
        const cluster = line.clusters[clusterIndex];
        const bounds = cluster.bounds?.scaled(scale);
        if (bounds === undefined) {
          return { rect: undefined, selectionRects: [] };
        }
        if (type === "left") {
          return {
            rect: new Rect(dx + bounds.x, dy + bounds.y, 0, bounds.height),
            selectionRects: [],
          };
        } else {
          return {
            rect: new Rect(dx + bounds.maxX, dy + bounds.y, 0, bounds.height),
            selectionRects: block
              ? [
                  new Rect(
                    dx + bounds.x,
                    dy + bounds.y,
                    bounds.width,
                    bounds.height,
                  ),
                ]
              : [],
          };
        }
      }
    } else {
      return { rect: undefined, selectionRects: [] };
    }
  }

  get plainText(): string {
    return this.lines.map((line) => line.plainText).join("\n");
  }
}

type CharBase = {
  char: string;
  rawOffset: number;
  resultOffset: number;
  ctrl: boolean;
};
type Char = CharBase & { cluster: number };

type Cluster = {
  resultOffset: number;
  bounds?: Rect;
};

export class Line {
  readonly result: string;
  readonly clusters: ReadonlyArray<Readonly<Cluster>>;
  readonly boundingBox: Rect | undefined;
  readonly unitsPerEm: number;
  readonly chars: ReadonlyArray<Readonly<Char>>;
  readonly buffer: HarfBuzzBuffer;

  constructor(
    readonly unicodeOffset: number,
    readonly raw: string,
    font: FontData,
  ) {
    let chars: CharBase[] = [];
    let offset = 0;
    for (let index = 0; index < raw.length; ) {
      const map = SignList.map(raw, index);
      if (map === undefined) {
        chars.push({
          char: raw.substring(index, index + 1),
          rawOffset: index,
          ctrl: false,
          resultOffset: offset,
        });
        index += 1;
      } else {
        const ctrl = map[1].length === 0 || SignList.isFormatControl(map[1]);
        chars.push({
          char: map[1],
          rawOffset: index,
          resultOffset: offset,
          ctrl,
        });
        index = index + map[0].length;
        offset += map[1].length;
      }
    }
    this.result = chars.map((c) => c.char).join("");
    const e = new TextEncoder();
    const utf8 = e.encode(this.result);
    const buffer = bufferFromText(this.result, font);
    const clusters: Cluster[] = [];
    let lastCluster = 0;
    let index = 0;
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
        clusters.push({ resultOffset: index, bounds: bb.rect });
        index += u16.length;
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
      clusters.push({ resultOffset: index, bounds: bb.rect });
    }
    this.clusters = clusters;
    const chs: Char[] = [];
    for (let i = 0; i < clusters.length; i++) {
      const from = clusters[i].resultOffset;
      const to =
        i + 1 < clusters.length ? clusters[i + 1].resultOffset : raw.length;
      for (const char of chars) {
        if (from <= char.resultOffset && char.resultOffset < to) {
          chs.push({ ...char, cluster: i });
        }
      }
    }
    this.chars = chs;

    const { ascender, descender, unitsPerEm } = font.ot;
    this.boundingBox = new Rect(0, 0, maxX, ascender - descender);
    this.unitsPerEm = font.ot.unitsPerEm;
  }

  destroy() {
    this.buffer.destroy();
  }

  get plainText(): string {
    let ret = this.result;
    for (let code = 0x13430; code <= 0x13455; code++) {
      ret = ret.replaceAll(String.fromCodePoint(code), "");
    }
    return ret;
  }
}
