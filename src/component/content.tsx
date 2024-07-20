import { FC } from "react";
import React from "react";
import { Content } from "../content";

export const ContentComponent: FC<{ content: Content; fontSize: number }> = ({
  content,
  fontSize,
}) => {
  return (
    <div className="hieroglyphs" style={{ fontSize, userSelect: "text" }}>
      {content.result}
    </div>
  );
};
