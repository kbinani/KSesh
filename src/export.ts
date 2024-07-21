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

export function svg(
  content: Content,
  font: FontData,
  fontSize: number,
  edgeInset: EdgeInset,
): Blob {
  const buffer = new HarfBuzzBuffer();
  buffer.addText(content.result);

  buffer.setDirection("ltr");
  hb.hb_buffer_set_script(buffer.ptr, HB_TAG("E", "g", "y", "p"));

  hb.hb_shape(font.hb.ptr, buffer.ptr, undefined, 0);
  const output = buffer.json();

  const scale = (1 / font.ot.unitsPerEm) * fontSize;

  const ns = "http://www.w3.org/2000/svg";
  const g0 = document.createElementNS(ns, "g");
  g0.setAttribute("translate", `translate(${edgeInset.left} ${edgeInset.top})`);

  const g1 = document.createElementNS(ns, "g") as unknown as SVGGElement;
  g1.setAttribute(
    "transform",
    `scale(${scale} ${scale}) translate(0 ${font.ot.unitsPerEm + font.ot.descender})`,
  );
  g0.appendChild(g1);

  let x = 0;
  let y = 0;
  let maxX: number = 0;
  for (const g of output) {
    const glyph = font.ot.glyphs.get(g.GlyphId);
    const path = glyph.getPath(
      x + g.XOffset,
      -(y + g.YOffset),
      font.hb.unitsPerEM,
    );
    maxX = Math.max(maxX, path.getBoundingBox().x2);
    if (path.commands.length > 0) {
      const str = path.toSVG(2);
      const p = document.createElementNS(ns, "path");
      g1.appendChild(p);
      p.outerHTML = str;
    }
    x += g.XAdvance;
    y += g.YAdvance;
  }
  buffer.destroy();

  const width = maxX * scale + edgeInset.left + edgeInset.right;
  const height =
    ((font.ot.ascender - font.ot.descender) / font.ot.unitsPerEm) * fontSize +
    edgeInset.top +
    edgeInset.bottom;

  const root = document.createElementNS(ns, "svg");
  root.setAttribute("xmlns", ns);
  root.setAttribute("viewBox", `0 0 ${width} ${height}`);
  root.setAttribute("data-text", content.result);
  root.setAttribute("data-raw", content.raw);

  root.appendChild(g0);

  return new Blob([root.outerHTML], { type: "image/svg+xml" });
}
