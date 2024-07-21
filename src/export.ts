import { Content } from "./content";
import { FontData } from "./font-data";
import { HarfBuzzBuffer, hb, HB_TAG } from "./harfbuzz";

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

function bufferFromContent(content: Content, font: FontData): HarfBuzzBuffer {
  const buffer = new HarfBuzzBuffer();
  buffer.addText(content.result);

  buffer.setDirection("ltr");
  hb.hb_buffer_set_script(buffer.ptr, HB_TAG("E", "g", "y", "p"));

  hb.hb_shape(font.hb.ptr, buffer.ptr, undefined, 0);
  return buffer;
}

function* enumerateGlyphs(
  buffer: HarfBuzzBuffer,
): Generator<{ x: number; y: number; gid: number }> {
  const output = buffer.json();
  let x = 0;
  let y = 0;
  for (const g of output) {
    yield { x: x + g.XOffset, y: -(y + g.YOffset), gid: g.GlyphId };
    x += g.XAdvance;
    y += g.YAdvance;
  }
}

function getPageScale(font: FontData, fontSize: number): number {
  const { unitsPerEm } = font.ot;
  return (1 / unitsPerEm) * fontSize;
}

function calculatePageSize(
  buffer: HarfBuzzBuffer,
  font: FontData,
  fontSize: number,
  edgeInset: EdgeInset,
): { width: number; height: number; scale: number } {
  const { ascender, descender, unitsPerEm } = font.ot;
  const scale = getPageScale(font, fontSize);
  let maxX = 0;
  for (const { x, y, gid } of enumerateGlyphs(buffer)) {
    const glyph = font.ot.glyphs.get(gid);
    const path = glyph.getPath(x, y, unitsPerEm);
    maxX = Math.max(maxX, path.getBoundingBox().x2);
  }
  const width = maxX * scale + edgeInset.left + edgeInset.right;
  const lineHeight = ((ascender - descender) / unitsPerEm) * fontSize;
  const height = lineHeight + edgeInset.top + edgeInset.bottom;
  return { width, height, scale };
}

export function svg(
  content: Content,
  font: FontData,
  fontSize: number,
  edgeInset: EdgeInset,
): Blob {
  const buffer = bufferFromContent(content, font);

  const { ascender, descender, unitsPerEm } = font.ot;
  const scale = getPageScale(font, fontSize);

  const ns = "http://www.w3.org/2000/svg";
  const g0 = document.createElementNS(ns, "g");
  g0.setAttribute("translate", `translate(${edgeInset.left} ${edgeInset.top})`);

  const g1 = document.createElementNS(ns, "g");
  g1.setAttribute(
    "transform",
    `scale(${scale} ${scale}) translate(0 ${unitsPerEm + descender})`,
  );
  g0.appendChild(g1);

  let maxX: number = 0;
  for (const { x, y, gid } of enumerateGlyphs(buffer)) {
    const glyph = font.ot.glyphs.get(gid);
    const path = glyph.getPath(x, y, unitsPerEm);
    maxX = Math.max(maxX, path.getBoundingBox().x2);
    if (path.commands.length > 0) {
      const str = path.toSVG(2);
      const pathElement = document.createElementNS(ns, "path");
      g1.appendChild(pathElement);
      pathElement.outerHTML = str;
    }
  }
  buffer.destroy();

  const width = maxX * scale + edgeInset.left + edgeInset.right;
  const lineHeight = ((ascender - descender) / unitsPerEm) * fontSize;
  const height = lineHeight + edgeInset.top + edgeInset.bottom;

  const root = document.createElementNS(ns, "svg");
  root.setAttribute("xmlns", ns);
  root.setAttribute("viewBox", `0 0 ${width} ${height}`);
  root.setAttribute("data-text", content.result);
  root.setAttribute("data-raw", content.raw);

  root.appendChild(g0);

  return new Blob([root.outerHTML], { type: "image/svg+xml" });
}

export async function png(
  content: Content,
  font: FontData,
  fontSize: number,
  edgeInset: EdgeInset,
): Promise<Blob> {
  const buffer = bufferFromContent(content, font);
  const { descender, unitsPerEm } = font.ot;
  const { width, height, scale } = calculatePageSize(
    buffer,
    font,
    fontSize,
    edgeInset,
  );

  const canvas = document.createElement("canvas");
  canvas.width = width;
  canvas.height = height;

  const ctx = canvas.getContext("2d")!;
  ctx.translate(edgeInset.left, edgeInset.top);
  ctx.scale(scale, scale);
  ctx.translate(0, unitsPerEm + descender);

  for (const { x, y, gid } of enumerateGlyphs(buffer)) {
    const glyph = font.ot.glyphs.get(gid);
    glyph.draw(ctx, x, y, unitsPerEm);
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

export async function pdf(
  content: Content,
  font: FontData,
  fontSize: number,
  edgeInset: EdgeInset,
): Promise<Blob> {
  const buffer = bufferFromContent(content, font);
  const { descender, unitsPerEm } = font.ot;
  const { width, height, scale } = calculatePageSize(
    buffer,
    font,
    fontSize,
    edgeInset,
  );

  const doc = new PDFDocument({ size: [width, height] });
  const stream = doc.pipe(blobStream());

  doc.translate(edgeInset.left, edgeInset.top);
  doc.scale(scale, scale);
  doc.translate(0, unitsPerEm + descender);

  for (const { x, y, gid } of enumerateGlyphs(buffer)) {
    const glyph = font.ot.glyphs.get(gid);
    const path = glyph.getPath(x, y, unitsPerEm);
    const data = path.toPathData(2);
    doc.path(data).fill("black");
  }

  doc.end();

  return new Promise((resolve, reject) => {
    stream.on("finish", () => {
      const blob = stream.toBlob("application/pdf");
      resolve(blob);
    });
    stream.on("error", (e: any) => {
      reject();
    });
  });
}
