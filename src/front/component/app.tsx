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
const stateVersion = 1;

export const App: FC = ({}) => {
  const [content, setContent] = useState<Content>(new Content(""));
  const [changed, setChanged] = useState(false);
  const [activeSignListTab, setActiveSignListTab] = useState("typing");
  const [typing, setTyping] = useState("A1");
  const [tabRows, setTabRows] = useState(1);
  const [fontSize, setFontSize] = useState<number>(48);
  const textarea = useRef<HTMLTextAreaElement>(null);
  const seshA = useRef<HTMLDivElement>(null);
  const seshB = useRef<HTMLDivElement>(null);
  const [font, setFont] = useState<string>();
  const onChange = (ev: ChangeEvent<HTMLTextAreaElement>) => {
    const text = ev.target.value;
    const content = new Content(text);
    setContent(content);
    setChanged(true);
    setActiveSignListTab("typing");
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
  const onResize = () => {
    const columns = Math.floor(window.innerWidth / 61);
    const rows = Math.ceil((Content.categories.length + 1) / columns);
    setTabRows(rows);
  };
  const onMouseEnter = () => {
    seshB.current?.classList.remove("seshBInit");

    seshA.current?.classList.remove("seshFadeIn");
    seshA.current?.classList.add("seshFadeOut");
    seshB.current?.classList.remove("seshFadeOut");
    seshB.current?.classList.add("seshFadeIn");
  };
  const onMouseLeave = () => {
    seshB.current?.classList.remove("seshBInit");

    seshB.current?.classList.remove("seshFadeIn");
    seshB.current?.classList.add("seshFadeOut");
    seshA.current?.classList.remove("seshFadeOut");
    seshA.current?.classList.add("seshFadeIn");
  };
  const onClickExportSvg = () => {
    if (font === undefined) {
      return;
    }
    const s: string[] = [];
    s.push(
      `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 200 ${fontSize}">`,
    );
    s.push(`  <defs>`);
    s.push(`    <style>`);
    s.push(`      @font-face {`);
    s.push(`        font-family: "Egyptian Text Proto";`);
    s.push(`        src: url("${font}");`);
    s.push(`      }`);
    s.push(`    </style>`);
    s.push(`  </defs>`);
    s.push(
      `  <text style='font-family: "Egyptian Text Proto"; font-size: ${fontSize}px;'>${content.result}</text>`,
    );
    s.push(`</svg>`);
    const blob = new Blob([s.join("\n")], { type: "image/svg+xml" });
    const url = URL.createObjectURL(blob);
    const link = document.createElement("a") as HTMLAnchorElement;
    link.href = url;
    link.download = "result.svg";
    link.click();
    URL.revokeObjectURL(url);
  };
  useEffect(() => {
    const s = window.history.state;
    let raw: string = placeholder;
    if (typeof s === "object" && typeof s["version"] === "number") {
      if (s.version === stateVersion) {
        if (typeof s["state"] === "string") {
          const state = s["state"];
          if (textarea.current && state !== "") {
            raw = state;
            textarea.current.value = state;
            setChanged(true);
          }
        }
      }
    }
    const text = new Content(raw);
    setContent(text);
    onResize();
    const controller = new AbortController();
    fetch("/font/eot.ttf", { signal: controller.signal }).then((res) => {
      res.blob().then((b) => {
        if (controller.signal.aborted) {
          return;
        }
        const reader = new FileReader();
        reader.onloadend = () => {
          if (controller.signal.aborted) {
            return;
          }
          if (typeof reader.result === "string") {
            setFont(reader.result);
          }
        };
        reader.readAsDataURL(b);
      });
    });
    window.addEventListener("resize", onResize);
    return () => {
      controller.abort();
      window.removeEventListener("resize", onResize);
    };
  }, []);
  useEffect(() => {
    window.history.replaceState(
      { version: stateVersion, state: content.raw },
      "",
    );
  }, [content]);
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
          display: "flex",
          height: "26px",
          lineHeight: "26px",
          fontSize: "20px",
          paddingLeft: "4px",
          backgroundColor: "#444",
          color: "#fff",
        }}
      >
        W
        <div
          style={{ color: "#fff", position: "relative", width: "50px" }}
          onMouseEnter={onMouseEnter}
          onMouseLeave={onMouseLeave}
        >
          <div
            ref={seshA}
            style={{
              position: "absolute",
              top: 0,
              left: 0,
              height: "26px",
              overflow: "hidden",
            }}
          >
            Sesh
          </div>
          <div
            ref={seshB}
            className="hieroglyphs seshBInit"
            style={{
              position: "absolute",
              top: 0,
              left: 0,
              height: "26px",
              fontSize: "20px",
              overflow: "hidden",
              lineHeight: "26px",
              paddingTop: "1px",
            }}
          >
            𓏞𓏜
          </div>
        </div>
        <div style={{ width: "20px" }} />
        <div className="menuBar">
          <div
            className="menuBarItem"
            onClick={onClickExportSvg}
            data-enabled={font !== undefined}
          >
            <div className="menuBarItemInner">Export to SVG</div>
          </div>
        </div>
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
            backgroundColor: "#ccc",
          }}
        >
          <div style={{ padding: "8px", opacity: changed ? 1 : 0.5 }}>
            <ContentComponent content={content} fontSize={fontSize} />
          </div>
        </div>
      </div>
      <div style={{ height: "50%" }}>
        <div
          style={{ display: "flex", flexDirection: "column", height: "50vh" }}
        >
          <div
            className="signListTab"
            style={{
              display: "flex",
              flexFlow: "row wrap",
              height: `calc(27px * ${tabRows})`,
              lineHeight: "26px",
              backgroundColor: "#444",
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
                {cat}&nbsp;
                <span className="hieroglyphs">
                  {
                    Content.signs[
                      Content.signs.findIndex(([s, _]) => {
                        return s.startsWith(cat);
                      })
                    ][1]
                  }
                </span>
              </div>
            ))}
            <div className="signListTabButtonSpacer" />
          </div>
          <div
            style={{
              display: "flex",
              flexFlow: "row wrap",
              overflowY: "scroll",
              height: `calc(50vh - 27px * ${tabRows})`,
              alignContent: "start",
            }}
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
                  <div className="signListCellHeader"> {id} </div>
                  <div className="signListCellSign">{sign}</div>
                </div>
              ))}
          </div>
        </div>
      </div>
    </div>
  );
};