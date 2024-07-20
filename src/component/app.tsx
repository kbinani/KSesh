import * as React from "react";
import {
  ChangeEvent,
  FC,
  SyntheticEvent,
  useEffect,
  useRef,
  useState,
} from "react";
import { ContentComponent } from "./content";
import { Content } from "../content";

const placeholder = "Y3 Y1A";

export const App: FC = ({}) => {
  const [content, setContent] = useState<Content>(new Content(""));
  const [changed, setChanged] = useState(false);
  const [activeSignListTab, setActiveSignListTab] = useState("typing");
  const [typing, setTyping] = useState("A1");
  const textarea = useRef<HTMLTextAreaElement>(null);
  const onChange = (ev: ChangeEvent<HTMLTextAreaElement>) => {
    const text = ev.target.value;
    const content = new Content(text);
    setContent(content);
    setChanged(true);
  };
  const onSelect = (ev: SyntheticEvent<HTMLTextAreaElement>) => {
    if (!textarea.current) {
      return;
    }
    const { value, selectionStart, selectionEnd } = textarea.current;
    const s = value.substring(0, selectionStart);
    let found = -1;
    for (let i = s.length - 1; i >= 0; i--) {
      const c = s[i];
      if (!/[0-9]/.test(c)) {
        found = i;
        break;
      }
    }
    const search =
      s.substring(found) + value.substring(selectionStart, selectionEnd);
    if (/^[A-Z]/.test(search)) {
      setTyping(search);
    } else {
      setTyping("");
    }
  };
  const onClickSign = (id: string, sign: string) => {
    if (!textarea.current) {
      return;
    }
    const { value, selectionStart, selectionEnd } = textarea.current;
    if (
      typing.length > 0 &&
      value.substring(
        selectionStart - typing.length,
        selectionStart + typing.length,
      ) === typing
    ) {
      let insert = id;
      if (
        value.substring(
          selectionStart + typing.length,
          selectionStart + typing.length + 1,
        ) !== " "
      ) {
        insert += " ";
      }
      const next =
        value.substring(0, selectionStart - typing.length) +
        insert +
        value.substring(selectionStart + typing.length);
      textarea.current.value = next;
      const location = selectionStart - typing.length + insert.length;
      textarea.current.setSelectionRange(location, location);
      const content = new Content(next);
      setContent(content);
    } else {
      let insert = id;
      if (value.substring(selectionEnd, selectionEnd + 1) !== " ") {
        insert += " ";
      }
      const next =
        value.substring(0, selectionStart) +
        insert +
        value.substring(selectionEnd);
      textarea.current.setRangeText(
        insert,
        selectionStart,
        selectionEnd,
        "end",
      );
      const content = new Content(next);
      setContent(content);
    }
    textarea.current.focus();
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
          <div style={{ padding: "4px", width: "100%" }}>
            <textarea
              ref={textarea}
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
              onSelect={onSelect}
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
        <div
          style={{ display: "flex", flexFlow: "row wrap", overflowY: "scroll" }}
        >
          {Content.signs
            .filter(([s, _]) => {
              if (activeSignListTab === "typing") {
                if (typing === "") {
                  return false;
                } else {
                  return s.startsWith(typing);
                }
              } else {
                const found = /[0-9]/.exec(s);
                if (!found) {
                  return false;
                }
                return s.substring(0, found.index) === activeSignListTab;
              }
            })
            .map(([id, sign], index) => (
              <div
                className="signListCell"
                key={index}
                onClick={() => onClickSign(id, sign)}
              >
                <div className="signListCellHeader">{id}</div>
                <div className="signListCellSign">{sign}</div>
              </div>
            ))}
        </div>
      </div>
    </div>
  );
};
