import React, { FC, MutableRefObject, useEffect, useRef } from "react";
import { Content } from "../content";
import { EdgeInset, enumeratePath } from "../export";
import { FontData } from "../font-data";
import { Rect } from "../rect";
import { useRefState } from "../hook";

export const ContentComponent: FC<{
  content: MutableRefObject<Content | undefined>;
  font: FontData;
  fontSize: number;
  lineSpacing: number;
  edgeInset: EdgeInset;
  cursor: Rect | undefined;
  cursorPadding: number;
}> = ({
  content,
  fontSize,
  font,
  lineSpacing,
  edgeInset,
  cursor: cursor_,
  cursorPadding,
}) => {
  const canvas = useRef<HTMLCanvasElement>(null);
  const dirty = useRef<boolean>(true);
  const [cursor, setCursor] = useRefState<Rect | undefined>(cursor_);
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
    ctx.scale(window.devicePixelRatio, window.devicePixelRatio);
    for (const path of enumeratePath({
      content: content.current,
      font,
      fontSize,
      lineSpacing,
      edgeInset,
    })) {
      path.draw(ctx);
    }
    const size = cursorPadding;
    if (cursor.current) {
      ctx.fillStyle = "rgba(0, 0, 255, 0.5)";
      ctx.fillRect(
        cursor.current.x - size / 2,
        cursor.current.y - size,
        cursor.current.width + size,
        cursor.current.height + 2 * size,
      );
    }
    ctx.restore();
  };
  useEffect(() => {
    dirty.current = true;
  }, [content.current, cursor.current]);
  useEffect(() => {
    setCursor(cursor_);
  }, [cursor_]);
  useEffect(() => {
    let width = window.innerWidth;
    let height = window.innerHeight;
    let dpr = window.devicePixelRatio;
    let cw = canvas.current?.width;
    let ch = canvas.current?.height;
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
      width={(window.innerWidth * window.devicePixelRatio) / 2}
      height={(window.innerHeight * window.devicePixelRatio) / 2}
      style={{ width: window.innerWidth / 2, height: window.innerHeight / 2 }}
    />
  );
};
