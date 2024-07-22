import { Content } from "./content";
import { FontData } from "./font-data";
import {
  GlyphInformation,
  HarfBuzzBuffer,
  hb,
  HB_BUFFER_CLUSTER_LEVEL_CHARACTERS,
  HB_TAG,
} from "./harfbuzz";

const PDFDocument = require("./pdfkit.standalone.js") as PDFKit.PDFDocument;
const blobStream = require("../node_modules/blob-stream/.js");

export type EdgeInset = {
  top: number;
  left: number;
  bottom: number;
  right: number;
};

export function base64FromBuffer(buffer: Uint8Array): string {
  return btoa(
    buffer.reduce((data, byte) => data + String.fromCharCode(byte), ""),
  );
}

export function download(blob: Blob, filename: string) {
  const url = URL.createObjectURL(blob);
  const link = document.createElement("a") as HTMLAnchorElement;
  link.href = url;
  link.download = filename;
  link.click();
  URL.revokeObjectURL(url);
}

export function writeClipboard(blob: Blob) {
  navigator.clipboard.write([
    new ClipboardItem({
      [blob.type]: blob,
    }),
  ]);
}

export function bufferFromText(text: string, font: FontData): HarfBuzzBuffer {
  const buffer = new HarfBuzzBuffer();
  buffer.addText(text);

  buffer.setDirection("ltr");
  hb.hb_buffer_set_script(buffer.ptr, HB_TAG("E", "g", "y", "p"));
  hb.hb_buffer_set_cluster_level(
    buffer.ptr,
    HB_BUFFER_CLUSTER_LEVEL_CHARACTERS,
  );

  hb.hb_shape(font.hb.ptr, buffer.ptr, undefined, 0);
  return buffer;
}

export function* enumerateGlyphs(
  buffer: HarfBuzzBuffer,
  font: FontData,
): Generator<{ x: number; y: number; gid: number; info: GlyphInformation }> {
  const { descender, unitsPerEm } = font.ot;
  const output = buffer.json();
  let x = 0;
  let y = -(unitsPerEm + descender);
  for (const g of output) {
    yield { x: x + g.XOffset, y: -(y + g.YOffset), gid: g.GlyphId, info: g };
    x += g.XAdvance;
    y += g.YAdvance;
  }
}

export function* enumeratePath({
  content,
  font,
  fontSize,
  lineSpacing,
  edgeInset,
}: {
  content: Content;
  font: FontData;
  fontSize: number;
  lineSpacing: number;
  edgeInset: EdgeInset;
}) {
  const scale = getPageScale(font, fontSize);
  const dx = edgeInset.left;
  let dy = edgeInset.top;
  for (const line of content.lines) {
    for (const { x, y, gid } of enumerateGlyphs(line.buffer, font)) {
      const glyph = font.ot.glyphs.get(gid);
      const path = glyph.getPath(
        dx + x * scale,
        dy + y * scale,
        font.ot.unitsPerEm * scale,
      );
      yield path;
    }
    dy += lineSpacing + fontSize;
  }
}

function getPageScale(font: FontData, fontSize: number): number {
  const { unitsPerEm } = font.ot;
  return (1 / unitsPerEm) * fontSize;
}

function calculatePageSize({
  content,
  font,
  fontSize,
  lineSpacing,
  edgeInset,
}: {
  content: Content;
  font: FontData;
  fontSize: number;
  lineSpacing: number;
  edgeInset: EdgeInset;
}): { width: number; height: number } {
  const { ascender, descender, unitsPerEm } = font.ot;
  const scale = getPageScale(font, fontSize);
  let maxX = edgeInset.left;
  for (const path of enumeratePath({
    content,
    font,
    fontSize,
    lineSpacing,
    edgeInset,
  })) {
    maxX = Math.max(maxX, path.getBoundingBox().x2);
  }
  const width = maxX + edgeInset.right;
  const lineHeight = (ascender - descender) * scale;
  const height =
    lineHeight * content.lines.length +
    lineSpacing * (content.lines.length - 1) +
    edgeInset.top +
    edgeInset.bottom;
  return { width, height };
}

export function svg({
  content,
  font,
  fontSize,
  lineSpacing,
  edgeInset,
}: {
  content: Content;
  font: FontData;
  fontSize: number;
  lineSpacing: number;
  edgeInset: EdgeInset;
}): Blob {
  const { width, height } = calculatePageSize({
    content,
    font,
    fontSize,
    lineSpacing,
    edgeInset,
  });

  const ns = "http://www.w3.org/2000/svg";
  const root = document.createElementNS(ns, "svg");

  for (const path of enumeratePath({
    content,
    font,
    fontSize,
    lineSpacing,
    edgeInset,
  })) {
    if (path.commands.length > 0) {
      const str = path.toSVG(2);
      const pathElement = document.createElementNS(ns, "path");
      root.appendChild(pathElement);
      pathElement.outerHTML = str;
    }
  }

  root.setAttribute("xmlns", ns);
  root.setAttribute("viewBox", `0 0 ${width} ${height}`);
  root.setAttribute("data-text", content.plainText);
  root.setAttribute("data-raw", content.raw);

  return new Blob([root.outerHTML], { type: "image/svg+xml" });
}

export async function png({
  content,
  font,
  fontSize,
  lineSpacing,
  edgeInset,
}: {
  content: Content;
  font: FontData;
  fontSize: number;
  lineSpacing: number;
  edgeInset: EdgeInset;
}): Promise<Blob> {
  const { width, height } = calculatePageSize({
    content,
    font,
    fontSize,
    lineSpacing,
    edgeInset,
  });

  const canvas = document.createElement("canvas");
  canvas.width = width;
  canvas.height = height;

  const ctx = canvas.getContext("2d")!;
  for (const path of enumeratePath({
    content,
    font,
    fontSize,
    lineSpacing,
    edgeInset,
  })) {
    path.draw(ctx);
  }

  return new Promise((resolve, reject) => {
    try {
      canvas.toBlob((data) => {
        if (data) {
          resolve(data);
        } else {
          reject();
        }
      }, "image/png");
    } catch {
      reject();
    }
  });
}

export async function pdf({
  content,
  font,
  fontSize,
  lineSpacing,
  edgeInset,
}: {
  content: Content;
  font: FontData;
  fontSize: number;
  lineSpacing: number;
  edgeInset: EdgeInset;
}): Promise<Blob> {
  const { width, height } = calculatePageSize({
    content,
    font,
    fontSize,
    lineSpacing,
    edgeInset,
  });

  const doc = new PDFDocument({ size: [width, height] });
  const stream = doc.pipe(blobStream());

  for (const path of enumeratePath({
    content,
    font,
    fontSize,
    lineSpacing,
    edgeInset,
  })) {
    const data = path.toPathData(5);
    doc.path(data).fill("black");
  }

  const promise = new Promise<Blob>((resolve, reject) => {
    stream.on("finish", () => {
      const blob = stream.toBlob("application/pdf");
      resolve(blob);
    });
    stream.on("error", (e: any) => {
      reject();
    });
  });

  doc.end();

  return promise;
}
