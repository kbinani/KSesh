import { Content } from "./content";
import { Font } from "./component/app";
import {
  HarfBuzzBlob,
  HarfBuzzBuffer,
  HarfBuzzFace,
  hb,
  HarfBuzzFont,
} from "harfbuzzjs/harfbuzz";

export type EdgeInset = {
  top: number;
  left: number;
  bottom: number;
  right: number;
};

export function svg(
  content: Content,
  ft: Font,
  fontSize: number,
  edgeInset: EdgeInset,
): Blob {
  const s: string[] = [];
  const blob = new HarfBuzzBlob(new Uint8Array(ft.data));
  const face = new HarfBuzzFace(blob, 0);
  const font = new HarfBuzzFont(face);
  const buffer = new HarfBuzzBuffer();
  buffer.addText(content.result);

  buffer.setDirection("ltr");
  hb.hb_buffer_set_script(buffer.ptr, HB_TAG("E", "g", "y", "p"));

  hb.hb_shape(font.ptr, buffer.ptr, undefined, 0);
  const output = buffer.json();
  console.log(ft.font.ascender, ft.font.descender);

  const scale = (1 / ft.font.unitsPerEm) * fontSize;
  s.push(`  <g transform="translate(${edgeInset.left} ${edgeInset.top})">`);
  s.push(
    `    <g transform="scale(${scale} ${scale}) translate(0 ${ft.font.unitsPerEm + ft.font.descender})">`,
  );

  let x = 0;
  let y = 0;
  let maxX: number = 0;
  for (const g of output) {
    const glyph = ft.font.glyphs.get(g.GlyphId);
    const path = glyph.getPath(
      x + g.XOffset,
      -(y + g.YOffset),
      font.unitsPerEM,
    );
    maxX = Math.max(maxX, path.getBoundingBox().x2);
    if (path.commands.length > 0) {
      s.push("        " + path.toSVG(1));
    }
    x += g.XAdvance;
    y += g.YAdvance;
  }
  buffer.destroy();
  font.destroy();
  face.destroy();
  blob.destroy();

  s.push(`    </g>`);
  s.push(`  </g>`);
  s.push(`</svg>`);

  const width = maxX * scale + edgeInset.left + edgeInset.right;
  const height =
    ((ft.font.ascender - ft.font.descender) / ft.font.unitsPerEm) * fontSize +
    edgeInset.top +
    edgeInset.bottom;
  const header = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${width} ${height}">`;

  const lines = header + "\n" + s.join("\n");

  return new Blob([lines], { type: "image/svg+xml" });
}

function HB_TAG(a: string, b: string, c: string, d: string): number {
  return (
    (a.charCodeAt(0) << 24) +
    (b.charCodeAt(0) << 16) +
    (c.charCodeAt(0) << 8) +
    d.charCodeAt(0)
  );
}
