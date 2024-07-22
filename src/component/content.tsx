import React, { FC, MutableRefObject, useEffect, useRef } from "react";
import { Content } from "../content";
import { enumerateGlyphs } from "../export";
import { FontData } from "../font-data";

export const ContentComponent: FC<{
  content: MutableRefObject<Content | undefined>;
  font: FontData;
  fontSize: number;
}> = ({ content, fontSize, font }) => {
  const canvas = useRef<HTMLCanvasElement>(null);
  const dirty = useRef<boolean>(true);
  const draw = () => {
    if (!content.current) {
      return;
    }
    const ctx = canvas.current?.getContext("2d");
    if (!ctx) {
      return;
    }
    ctx.save();
    ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);
    const scale = (fontSize / font.ot.unitsPerEm) * window.devicePixelRatio;
    ctx.scale(scale, scale);
    for (const { x, y, gid } of enumerateGlyphs(content.current.buffer, font)) {
      const glyph = font.ot.glyphs.get(gid);
      glyph.draw(ctx, x, y, font.ot.unitsPerEm);
    }
    ctx.restore();
  };
  useEffect(() => {
    dirty.current = true;
  }, [content.current]);
  useEffect(() => {
    let width = window.innerWidth;
    let height = window.innerHeight;
    let dpr = window.devicePixelRatio;
    let cw = canvas.current?.width;
    let ch = canvas.current?.height;
    let last = Date.now();
    const update = () => {
      if (
        window.innerWidth !== width ||
        window.innerHeight !== height ||
        window.devicePixelRatio !== dpr ||
        canvas.current?.width !== cw ||
        canvas.current?.height !== ch
      ) {
        dirty.current = true;
      }
      if (dirty.current) {
        try {
          draw();
          width = window.innerWidth;
          height = window.innerHeight;
          dpr = window.devicePixelRatio;
          cw = canvas.current?.width;
          ch = canvas.current?.height;
          last = Date.now();
          dirty.current = false;
        } catch (e) {
          console.error(e);
        }
      }
      requestAnimationFrame(update);
    };
    requestAnimationFrame(update);
  }, []);
  return (
    <canvas
      ref={canvas}
      width={window.innerWidth * window.devicePixelRatio}
      height={window.innerHeight * window.devicePixelRatio}
      style={{ width: window.innerWidth, height: window.innerHeight }}
    />
  );
};
