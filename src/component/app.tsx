import * as React from "react";
import { ChangeEvent, FC, useEffect, useState } from "react";
import { ContentComponent } from "./content";
import { Content } from "../content";

const placeholder = "Y3 Y1A";

export const App: FC = ({}) => {
  const [content, setContent] = useState<Content>(new Content(""));
  const [changed, setChanged] = useState(false);
  const [activeSignListTab, setActiveSignListTab] = useState("typing");
  const onChange = (ev: ChangeEvent<HTMLTextAreaElement>) => {
    const text = ev.target.value;
    const content = new Content(text);
    setContent(content);
    setChanged(true);
  };
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
          height: "26px",
          lineHeight: "26px",
          fontSize: "20px",
          paddingLeft: "4px",
          backgroundColor: "#444",
          color: "#fff",
        }}
      >
        WSesh
      </div>
      <div style={{ display: "flex", height: "50%" }}>
        <div style={{ display: "flex", width: "50%" }}>
          <div style={{ padding: "4px" }}>
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
        </div>
        <div
          style={{
            width: "50%",
            height: "100%",
            opacity: changed ? 1 : 0.5,
            backgroundColor: "#ccc",
          }}
        >
          <div style={{ padding: "8px" }}>
            <ContentComponent content={content} fontSize={48} />
          </div>
        </div>
      </div>
      <div style={{ display: "flex", flexDirection: "column", height: "50%" }}>
        <div
          className="signListTab"
          style={{
            display: "flex",
            height: "26px",
            lineHeight: "26px",
            backgroundColor: "#444",
            overflowY: "scroll",
          }}
        >
          <div
            className="signListTabButton"
            data-active={activeSignListTab === "typing"}
            onClick={() => setActiveSignListTab("typing")}
          >
            typing
          </div>
          {Content.categories.map((cat, index) => (
            <div
              className="signListTabButton"
              data-active={activeSignListTab === cat}
              onClick={() => setActiveSignListTab(cat)}
              key={index}
            >
              {cat}
            </div>
          ))}
        </div>
        <div>content</div>
      </div>
    </div>
  );
};
