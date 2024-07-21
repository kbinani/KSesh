import { Content } from "./content";
import { FontData } from "./font-data";
import { HarfBuzzBuffer, hb, HB_TAG } from "./harfbuzz";

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

export function svg(
  content: Content,
  font: FontData,
  fontSize: number,
  edgeInset: EdgeInset,
): Blob {
  const buffer = bufferFromContent(content, font);

  const { ascender, descender, unitsPerEm } = font.ot;
  const scale = (1 / unitsPerEm) * fontSize;

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
  const { ascender, descender, unitsPerEm } = font.ot;
  const scale = (1 / unitsPerEm) * fontSize;

  let maxX = 0;
  for (const { x, y, gid } of enumerateGlyphs(buffer)) {
    const glyph = font.ot.glyphs.get(gid);
    const path = glyph.getPath(x, y, unitsPerEm);
    maxX = Math.max(maxX, path.getBoundingBox().x2);
  }
  const width = maxX * scale + edgeInset.left + edgeInset.right;
  const lineHeight = ((ascender - descender) / unitsPerEm) * fontSize;
  const height = lineHeight + edgeInset.top + edgeInset.bottom;

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
