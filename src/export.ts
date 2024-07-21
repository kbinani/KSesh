import { Content } from "./content";
import { FontData } from "./font-data";
import { HarfBuzzBuffer, hb, HB_TAG } from "./harfbuzz";

export type EdgeInset = {
  top: number;
  left: number;
  bottom: number;
  right: number;
};

export function svg(
  content: Content,
  font: FontData,
  fontSize: number,
  edgeInset: EdgeInset,
): Blob {
  const s: string[] = [];
  const buffer = new HarfBuzzBuffer();
  buffer.addText(content.result);

  buffer.setDirection("ltr");
  hb.hb_buffer_set_script(buffer.ptr, HB_TAG("E", "g", "y", "p"));

  hb.hb_shape(font.hb.ptr, buffer.ptr, undefined, 0);
  const output = buffer.json();

  const scale = (1 / font.ot.unitsPerEm) * fontSize;
  s.push(`  <g transform="translate(${edgeInset.left} ${edgeInset.top})">`);
  s.push(
    `    <g transform="scale(${scale} ${scale}) translate(0 ${font.ot.unitsPerEm + font.ot.descender})">`,
  );

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
      s.push("        " + path.toSVG(1));
    }
    x += g.XAdvance;
    y += g.YAdvance;
  }
  buffer.destroy();

  s.push(`    </g>`);
  s.push(`  </g>`);
  s.push(`</svg>`);

  const width = maxX * scale + edgeInset.left + edgeInset.right;
  const height =
    ((font.ot.ascender - font.ot.descender) / font.ot.unitsPerEm) * fontSize +
    edgeInset.top +
    edgeInset.bottom;
  const header = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${width} ${height}">`;

  const lines = header + "\n" + s.join("\n");

  return new Blob([lines], { type: "image/svg+xml" });
}
