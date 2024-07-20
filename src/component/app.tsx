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
    <div
      style={{
        display: "flex",
        flexDirection: "column",
        width: "100%",
        height: "100%",
      }}
    >
      <div
        style={{
          height: "22px",
          lineHeight: "22px",
          fontSize: "16px",
          paddingLeft: "4px",
          backgroundColor: "#444",
          color: "#fff",
        }}
      >
        WSesh
      </div>
      <div style={{ display: "flex", height: "100%" }}>
        <div style={{ width: "50%", padding: 4 }}>
          <textarea
            style={{
              width: "100%",
              height: "100%",
              fontSize: "20px",
              resize: "none",
              outline: "none",
              border: "none",
              padding: 0,
            }}
            autoFocus={true}
            spellCheck={false}
            placeholder={changed ? undefined : placeholder}
            onChange={onChange}
          />
        </div>
        <div
          style={{
            width: "50%",
            height: "100%",
            opacity: changed ? 1 : 0.5,
            backgroundColor: "#ccc",
          }}
        >
          <div style={{ padding: 4 }}>
            <ContentComponent content={content} fontSize={48} />
          </div>
        </div>
      </div>
    </div>
  );
};
