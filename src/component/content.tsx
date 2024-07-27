import React, { FC, MutableRefObject, useEffect, useRef } from "react";
import { Content, Cursor } from "../content";
import { EdgeInset, enumeratePath } from "../export";
import { FontData } from "../font-data";
import { useRefState } from "../hook";
import { Direction } from "./app";

export const ContentComponent: FC<{
  content: MutableRefObject<Content | undefined>;
  font: FontData;
  fontSize: number;
  lineSpacing: number;
  edgeInset: EdgeInset;
  cursor: Cursor | undefined;
  cursorPadding: number;
  onCursorLocationChange: (location: number, direction: Direction) => void;
}> = ({
  content,
  fontSize,
  font,
  lineSpacing,
  edgeInset,
  cursor: cursor_,
  cursorPadding,
  onCursorLocationChange,
}) => {
  const canvas = useRef<HTMLCanvasElement>(null);
  const dirty = useRef<boolean>(true);
  const [cursor, setCursor] = useRefState<Cursor | undefined>(cursor_);
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
    const size = cursorPadding;
    if (cursor.current) {
      ctx.fillStyle = "rgba(0, 0, 255, 0.2)";
      for (const rect of cursor.current.selectionRects) {
        ctx.fillRect(
          rect.x - size / 2,
          rect.y - size,
          rect.width + size,
          rect.height + 2 * size,
        );
      }
    }
    for (const path of enumeratePath({
      content: content.current,
      font,
      fontSize,
      lineSpacing,
      edgeInset,
    })) {
      path.draw(ctx);
    }
    if (cursor.current?.rect) {
      ctx.fillStyle = "rgba(0, 0, 255, 0.5)";
      ctx.fillRect(
        cursor.current.rect.x - size / 2,
        cursor.current.rect.y - size,
        cursor.current.rect.width + size,
        cursor.current.rect.height + 2 * size,
      );
    }
    ctx.restore();
  };
  const onMouseDown = (ev: MouseEvent) => {
    ev.preventDefault();
    const c = content.current;
    if (!c) {
      return;
    }
    const bounds = canvas.current?.getBoundingClientRect();
    if (!bounds) {
      return;
    }
    const x = ev.clientX - bounds.x;
    const y = ev.clientY - bounds.y;
    const { location, direction } = c.closestPosition({
      point: { x, y },
      font,
      fontSize,
      lineSpacing,
      edgeInset,
    });
    onCursorLocationChange(location, direction);
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
    canvas.current?.addEventListener("mousedown", onMouseDown);
    requestAnimationFrame(update);
    return () => {
      canvas.current?.removeEventListener("mousedown", onMouseDown);
    };
  }, []);
  return (
    <canvas
      className="contentCanvas"
      ref={canvas}
      width={(window.innerWidth * window.devicePixelRatio) / 2}
      height={(window.innerHeight * window.devicePixelRatio) / 2}
      style={{ width: window.innerWidth / 2, height: window.innerHeight / 2 }}
    />
  );
};
