import * as React from "react";
import { ChangeEvent, FC, useState } from "react";
import { ContentComponent } from "./content";
import { Content } from "../content";

export const App: FC = ({}) => {
  const [content, setContent] = useState<Content>(new Content(""));
  const onChange = (ev: ChangeEvent<HTMLTextAreaElement>) => {
    const text = ev.target.value;
    if (!text) {
      return;
    }
    const content = new Content(text);
    setContent(content);
  };
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
        spellCheck={false}
        defaultValue={"Y3 A1"}
        onChange={onChange}
      />
      <div
        className="hieroglyphs"
        style={{ width: "50%", height: "100%", fontSize: "48px" }}
      >
        <ContentComponent content={content} />
      </div>
    </div>
  );
};
