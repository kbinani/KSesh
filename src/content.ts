import { bufferFromText, EdgeInset, enumerateGlyphs } from "./export";
import { FontData } from "./font-data";
import { Rect } from "./rect";
import { BoundingBox } from "./bounding-box";
import { HarfBuzzBuffer } from "./harfbuzz";
import { SignList } from "./sign-list";
import { Direction } from "./component/app";

export type Cursor = { rect: Rect | undefined; selectionRects: Rect[] };
type CursorLocation =
  | { type: "left"; lineIndex: number; clusterIndex: number }
  | { type: "right"; lineIndex: number; clusterIndex: number; block: boolean }
  | { type: "end"; lineIndex: number };
export type Point = { x: number; y: number };

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

  closestPosition({
    point,
    font,
    fontSize,
    lineSpacing,
    edgeInset,
  }: {
    point: Point;
    font: FontData;
    fontSize: number;
    lineSpacing: number;
    edgeInset: EdgeInset;
  }): { location: number; direction: Direction } {
    if (this.lines.length === 0) {
      return { location: 0, direction: "forward" };
    }
    let lineIndex: number;
    if (point.y <= edgeInset.top + fontSize + lineSpacing / 2) {
      lineIndex = 0;
    } else {
      const dy = point.y - (edgeInset.top + fontSize + lineSpacing / 2);
      lineIndex = Math.min(
        1 + Math.floor(dy / (fontSize + lineSpacing)),
        this.lines.length - 1,
      );
    }
    const line = this.lines[lineIndex];
    let minDistance = Number.MAX_VALUE;
    let nearest: { location: number; direction: Direction } = {
      location: line.rawOffset,
      direction: "forward",
    };
    for (let i = 0; i < line.chars.length; i++) {
      const ch = line.chars[i];
      if (!ch.sign) {
        continue;
      }
      const start = line.rawOffset + ch.rawOffset;
      const end = line.rawOffset + ch.rawOffset + ch.raw.length;
      const test: { location: number; direction: Direction }[] = [
        { location: start, direction: "forward" },
        { location: end, direction: "backward" },
      ];
      for (const { location, direction } of test) {
        const { rect } = this.cursor({
          selectionStart: location,
          selectionEnd: location,
          direction,
          font,
          fontSize,
          lineSpacing,
          edgeInset,
        });
        if (!rect) {
          continue;
        }
        const center = rect.center;
        const distance = Math.hypot(point.x - center.x, point.y - center.y);
        if (distance <= minDistance) {
          minDistance = distance;
          nearest = { location, direction };
        }
      }
    }
    return nearest;
  }

  private cursorLocation({
    location,
    direction,
  }: {
    location: number;
    direction: Direction;
  }): CursorLocation | undefined {
    const lineIndex = this.lines.findIndex(
      (line) =>
        line.rawOffset <= location &&
        location <= line.rawOffset + line.raw.length,
    );
    if (lineIndex < 0) {
      return undefined;
    }
    const line = this.lines[lineIndex];
    if (location === line.rawOffset) {
      return { lineIndex, clusterIndex: 0, type: "left" };
    }
    let charIndex = -1;
    for (let i = 0; i < line.chars.length; i++) {
      const char = line.chars[i];
      const from = line.rawOffset + char.rawOffset;
      let to: number;
      if (i + 1 < line.chars.length) {
        to = line.chars[i + 1].rawOffset;
      } else {
        to = line.raw.length;
      }
      to += line.rawOffset;
      if (from <= location && location < to) {
        charIndex = i;
        break;
      }
    }
    const char = line.chars[charIndex];
    if (char === undefined) {
      return { lineIndex, type: "end" };
    }
    let firstCharIndex = charIndex;
    for (let i = charIndex; i >= 0; i--) {
      const ch = line.chars[i];
      if (ch.cluster === char.cluster) {
        firstCharIndex = i;
      } else if (ch.cluster < char.cluster) {
        break;
      }
    }
    let lastCharIndex = charIndex;
    for (let i = charIndex; i < line.chars.length; i++) {
      const ch = line.chars[i];
      if (ch.cluster === char.cluster) {
        lastCharIndex = i;
      } else if (ch.cluster > char.cluster) {
        break;
      }
    }
    let startCharIndex = -1;
    for (let i = firstCharIndex; i <= lastCharIndex; i++) {
      const ch = line.chars[i];
      if (ch.sign) {
        startCharIndex = i;
        break;
      }
    }
    if (startCharIndex < 0) {
      return;
    }
    if (char.cluster > 0) {
      for (let i = firstCharIndex - 1; i >= 0; i--) {
        const ch = line.chars[i];
        if (ch.sign && !ch.ctrl) {
          if (location === line.rawOffset + ch.rawOffset + ch.raw.length) {
            return {
              lineIndex,
              clusterIndex: ch.cluster,
              type: "right",
              block: true,
            };
          }
        }
      }
    }
    const start = line.chars[startCharIndex];
    if (
      line.rawOffset + start.rawOffset < location &&
      location <= line.rawOffset + start.rawOffset + start.raw.length
    ) {
      return {
        lineIndex,
        clusterIndex: start.cluster,
        type: "right",
        block: true,
      };
    }
    if (line.rawOffset + start.rawOffset + start.raw.length < location) {
      if (direction === "backward") {
        return {
          lineIndex,
          clusterIndex: char.cluster,
          type: "right",
          block: false,
        };
      } else if (line.clusters.length - 1 === char.cluster) {
        return { lineIndex, type: "end" };
      } else {
        for (let i = lastCharIndex + 1; i < line.chars.length; i++) {
          const ch = line.chars[i];
          if (ch.sign) {
            return { lineIndex, clusterIndex: ch.cluster, type: "left" };
          }
        }
      }
    }
    if (charIndex <= startCharIndex) {
      if (direction === "forward" || char.cluster === 0) {
        return { lineIndex, clusterIndex: char.cluster, type: "left" };
      }
    }
    if (direction === "forward") {
      for (let i = charIndex + 1; i < line.chars.length; i++) {
        const ch = line.chars[i];
        if (ch.sign) {
          return { lineIndex, clusterIndex: ch.cluster, type: "left" };
        }
      }
    } else {
      for (let i = charIndex - 1; i >= 0; i--) {
        const ch = line.chars[i];
        if (ch.sign) {
          return {
            lineIndex,
            clusterIndex: ch.cluster,
            type: "right",
            block: false,
          };
        }
      }
    }
    return undefined;
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
        direction,
      });
      if (location === undefined) {
        return { rect: undefined, selectionRects: [] };
      } else {
        if (location.type === "end") {
          const { lineIndex } = location;
          const dx = edgeInset.left;
          const dy = edgeInset.top + (fontSize + lineSpacing) * lineIndex;
          const line = this.lines[lineIndex];
          const bounds = line.boundingBox?.scaled(scale);
          if (bounds === undefined) {
            return {
              rect: new Rect(dx, dy, 0, fontSize),
              selectionRects: [],
            };
          } else {
            return {
              rect: new Rect(
                dx + bounds.x + bounds.width,
                dy + bounds.y,
                0,
                bounds.height,
              ),
              selectionRects: [],
            };
          }
        } else {
          const { lineIndex, clusterIndex, type } = location;
          const dx = edgeInset.left;
          const dy = edgeInset.top + (fontSize + lineSpacing) * lineIndex;
          const line = this.lines[lineIndex];
          const cluster = line.clusters[clusterIndex];
          if (cluster === undefined) {
            return {
              rect: new Rect(dx, dy, 0, fontSize),
              selectionRects: [],
            };
          }
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
              selectionRects: location.block
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
      }
    } else {
      const startLocation = this.cursorLocation({
        location: selectionStart,
        direction: "forward",
      });
      const endLocation = this.cursorLocation({
        location: selectionEnd,
        direction: "backward",
      });
      const selectionRects: Rect[] = [];
      if (startLocation !== undefined && endLocation !== undefined) {
        const start = {
          line: startLocation.lineIndex,
          cluster:
            startLocation.type === "end"
              ? Number.MAX_SAFE_INTEGER
              : startLocation.clusterIndex,
        };
        const end = {
          line: endLocation.lineIndex,
          cluster:
            endLocation.type === "end"
              ? Number.MAX_SAFE_INTEGER
              : endLocation.clusterIndex,
        };
        const dx = edgeInset.left;
        for (let i = 0; i < this.lines.length; i++) {
          const dy = edgeInset.top + (fontSize + lineSpacing) * i;
          const line = this.lines[i];
          const bb = new BoundingBox();
          for (let j = 0; j < line.clusters.length; j++) {
            const c = { line: i, cluster: j };
            if (compareLineAndCluster(start, c) === 0) {
              if (startLocation.type !== "left") {
                continue;
              }
            } else if (compareLineAndCluster(end, c) === 0) {
              if (endLocation.type !== "right") {
                continue;
              }
            } else if (
              0 <= compareLineAndCluster(start, c) ||
              compareLineAndCluster(c, end) >= 0
            ) {
              continue;
            }
            const cluster = line.clusters[j];
            const bounds = cluster.bounds?.scaled(scale);
            if (bounds) {
              bb.add(
                new Rect(
                  dx + bounds.x,
                  dy + bounds.y,
                  bounds.width,
                  bounds.height,
                ),
              );
            }
          }
          const lineBounds = bb.rect;
          if (lineBounds) {
            selectionRects.push(lineBounds);
          }
        }
      }
      return { rect: undefined, selectionRects };
    }
  }

  get textWithControls(): string {
    return this.lines.map((line) => line.result).join("\n");
  }

  get textWithoutControls(): string {
    return this.lines.map((line) => line.textWithoutControls).join("\n");
  }
}

function compareLineAndCluster(
  left: { line: number; cluster: number },
  right: { line: number; cluster: number },
): number {
  if (left.line === right.line) {
    return left.cluster - right.cluster;
  } else {
    return left.line - right.line;
  }
}

type CharBase = {
  char: string;
  charPlain?: string;
  raw: string;
  rawOffset: number;
  resultOffset: number;
  ctrl: boolean;
  sign: boolean;
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
    readonly rawOffset: number,
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
          raw: raw.substring(index, index + 1),
          rawOffset: index,
          ctrl: false,
          resultOffset: offset,
          sign: false,
        });
        index += 1;
      } else {
        const ctrl = map[1].length === 0 || SignList.isFormatControl(map[1]);
        const sign = SignList.isSign(map[1]);
        chars.push({
          char: map[1],
          raw: map[0],
          rawOffset: index,
          resultOffset: offset,
          ctrl,
          sign,
        });
        index = index + map[0].length;
        offset += map[1].length;
      }
    }
    for (let i = 0; i < chars.length; i++) {
      if (chars[i].char.startsWith("𓊈")) {
        for (let j = i + 1; j < chars.length; j++) {
          const ch = chars[j];
          if (SignList.enclosureBeginning.some((c) => ch.char.startsWith(c))) {
            break;
          } else if (
            SignList.enclosureTerminal.some((c) => ch.char.endsWith(c))
          ) {
            ch.char = "\u{1343d}𓊉";
            break;
          }
        }
      }
    }
    for (let i = 0; i < chars.length; i++) {
      if (chars[i].char !== "&") {
        continue;
      }
      if (i === 0 || !chars[i - 1].sign) {
        continue;
      }
      if (i + 1 >= chars.length || !chars[i + 1].sign) {
        continue;
      }
      const left = chars[i - 1];
      const center = chars[i];
      const right = chars[i + 1];
      const type = SignList.insertionType(left.char, right.char);
      switch (type) {
        case "topStart":
          chars[i - 1] = { ...left, char: right.char, charPlain: left.char };
          chars[i] = { ...center, char: SignList.topStartInsertion };
          chars[i + 1] = { ...right, char: left.char, charPlain: right.char };
          break;
        case "bottomStart":
          chars[i - 1] = { ...left, char: right.char, charPlain: left.char };
          chars[i] = { ...center, char: SignList.bottomStartInsertion };
          chars[i + 1] = { ...right, char: left.char, charPlain: right.char };
          break;
        case "topEnd":
          chars[i] = {
            ...center,
            char: SignList.topEndInsertion,
            charPlain: "",
          };
          break;
        case "bottomEnd":
          chars[i] = {
            ...center,
            char: SignList.bottomEndInsertion,
            charPlain: "",
          };
          break;
      }
      if (type === "topStart" || type === "bottomStart") {
        if (
          i + 3 < chars.length &&
          chars[i + 2].char === "&" &&
          chars[i + 3].sign
        ) {
          const t = SignList.insertionType(right.char, chars[i + 3].char);
          switch (t) {
            case "topEnd":
              chars[i + 2] = {
                ...chars[i + 2],
                char: SignList.topEndInsertion,
              };
              break;
            case "bottomEnd":
              chars[i + 2] = {
                ...chars[i + 2],
                char: SignList.bottomEndInsertion,
              };
              break;
          }
          i += 3;
        }
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
        i + 1 < clusters.length
          ? clusters[i + 1].resultOffset
          : this.result.length;
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

  get textWithoutControls(): string {
    let ret = this.chars.map((ch) => ch.charPlain ?? ch.char).join("");
    for (let code = 0x13430; code <= 0x13455; code++) {
      ret = ret.replaceAll(String.fromCodePoint(code), "");
    }
    return ret;
  }
}
