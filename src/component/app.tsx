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
    if (content.ok) {
      setContent(content);
    }
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
        defaultValue={"M23-Z7-M18-M17-Z4:D54-G17-X1:G1-N25:X1*Z1"}
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
