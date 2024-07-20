import * as React from "react";
import { ChangeEvent, FC, useEffect, useState } from "react";
import { ContentComponent } from "./content";
import { Content } from "../content";

export const App: FC = ({}) => {
  const [content, setContent] = useState<Content>(new Content(""));
  const [changed, setChanged] = useState(false);
  const onChange = (ev: ChangeEvent<HTMLTextAreaElement>) => {
    const text = ev.target.value;
    const content = new Content(text);
    setContent(content);
    setChanged(true);
  };
  const placeholder = "Y3 Y1A";
  useEffect(() => {
    const text = new Content(placeholder);
    setContent(text);
  }, []);
  return (
    <div style={{ display: "flex", width: "100%", height: "100%" }}>
      <textarea
        style={{
          width: "50%",
          height: "100%",
          fontSize: "20px",
          resize: "none",
          outline: "none",
          border: "none",
        }}
        autoFocus={true}
        spellCheck={false}
        placeholder={changed ? undefined : placeholder}
        onChange={onChange}
      />
      <div
        className="hieroglyphs"
        style={{
          width: "50%",
          height: "100%",
          fontSize: "48px",
          opacity: changed ? 1 : 0.5,
        }}
      >
        <ContentComponent content={content} />
      </div>
    </div>
  );
};
