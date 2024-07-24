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
import { Content, Cursor } from "../content";
import {
  base64FromBuffer,
  download,
  EdgeInset,
  pdf,
  png,
  svg,
  writeClipboard,
} from "../export";
import { loadHarfbuzz } from "../harfbuzz";
import { FontData } from "../font-data";
import { staticData } from "../static-data";
import { useRefState } from "../hook";
import { SignList } from "../sign-list";
import { About } from "./about";

const placeholder = "Y3 Y1A";
const stateVersion = 1;
const padding = 8;
const cursorPadding = 4;

export type Direction = "forward" | "backward";
type TextSelection = {
  start: number;
  end: number;
  direction: Direction | undefined;
};

export const App: FC = ({}) => {
  const [content, setContent] = useRefState<Content | undefined>(undefined);
  const [changed, setChanged] = useState(false);
  const [activeSignListTab, setActiveSignListTab] = useState("typing");
  const [typing, setTyping] = useState("A1");
  const [tabRows, setTabRows] = useState(1);
  const [fontSize, setFontSize] = useState<number>(48);
  const [lineSpacing, setLineSpacing] = useState<number>(12);
  const [font, setFont] = useState<FontData>();
  const [cursor, setCursor] = useState<Cursor>();
  const [focus, setFocus] = useState(false);
  const [textSelection, setTextSelection] = useState<TextSelection>({
    start: 0,
    end: 0,
    direction: "forward",
  });
  const [isAboutVisible, setAboutVisible] = useState(false);
  const main = useRef<HTMLDivElement>(null);
  const textarea = useRef<HTMLTextAreaElement>(null);
  const seshA = useRef<HTMLDivElement>(null);
  const seshB = useRef<HTMLDivElement>(null);
  const edgeInset: EdgeInset = {
    top: padding,
    left: padding,
    right: padding,
    bottom: padding,
  };
  const onChange = (ev: ChangeEvent<HTMLTextAreaElement>) => {
    if (font === undefined) {
      return;
    }
    const text = ev.target.value;
    const content = new Content(text, font);
    setContent(content);
    setChanged(true);
    setActiveSignListTab("typing");
  };
  const onSelect = (ev: SyntheticEvent<HTMLTextAreaElement>) => {
    if (!textarea.current) {
      return;
    }
    const { value, selectionStart, selectionEnd } = textarea.current;
    let direction: Direction | undefined;
    if (selectionStart === selectionEnd) {
      const last =
        textSelection.direction === "forward"
          ? textSelection.end
          : textSelection.start;
      direction = last < selectionStart ? "forward" : "backward";
      setTextSelection({
        start: selectionStart,
        end: selectionEnd,
        direction,
      });
    } else {
      setTextSelection({
        start: selectionStart,
        end: selectionEnd,
        direction: undefined,
      });
    }
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
    let c: Cursor | undefined;
    if (font) {
      c = content.current?.cursor({
        selectionStart,
        selectionEnd,
        font,
        fontSize,
        lineSpacing,
        edgeInset,
        direction: direction ?? "forward",
      });
    }
    if (c) {
      setCursor(c);
    } else {
      setCursor(undefined);
    }
  };
  const onClickSign = (id: string, sign: string) => {
    if (!textarea.current || font === undefined) {
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
      const content = new Content(next, font);
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
      const content = new Content(next, font);
      setContent(content);
    }
    textarea.current.focus();
    setChanged(true);
  };
  const onResize = () => {
    const columns = Math.floor(window.innerWidth / 61);
    const rows = Math.ceil((SignList.categories.length + 1) / columns);
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
    if (font === undefined || content.current === undefined) {
      return;
    }
    const blob = svg({
      content: content.current,
      font,
      fontSize,
      lineSpacing,
      edgeInset,
    });
    download(blob, "result.svg");
  };
  const onClickExportPng = async () => {
    if (font === undefined || content.current === undefined) {
      return;
    }
    const blob = await png({
      content: content.current,
      font,
      fontSize,
      lineSpacing,
      edgeInset,
    });
    download(blob, "result.png");
  };
  const onClickExportPdf = async () => {
    if (font === undefined || content.current === undefined) {
      return;
    }
    const blob = await pdf({
      content: content.current,
      font,
      fontSize,
      lineSpacing,
      edgeInset,
    });
    download(blob, "result.pdf");
  };
  const onClickCopyPng = async () => {
    if (font === undefined || content.current === undefined) {
      return;
    }
    const blob = await png({
      content: content.current,
      font,
      fontSize,
      lineSpacing,
      edgeInset,
    });
    writeClipboard(blob);
  };
  const onClickCopyPlainUnicode = () => {
    if (font === undefined || content.current === undefined) {
      return;
    }
    const blob = new Blob([content.current.plainText], { type: "text/plain" });
    writeClipboard(blob);
  };
  const onClickCopyUnicode = () => {
    if (font === undefined || content.current === undefined) {
      return;
    }
    const blob = new Blob(
      [content.current.lines.map((line) => line.plainText).join("\n")],
      { type: "text/plain" },
    );
    writeClipboard(blob);
  };
  const onFocus = () => {
    setFocus(true);
  };
  const onBlur = () => {
    setFocus(false);
  };
  useEffect(() => {
    const s = window.history.state;
    let raw: string = placeholder;
    if (
      s != null &&
      typeof s === "object" &&
      typeof s["version"] === "number"
    ) {
      if (s.version === stateVersion) {
        if (typeof s["state"] === "string") {
          const state = s["state"];
          if (state !== "") {
            raw = state;
            setChanged(true);
          }
        }
      }
    }
    if (textarea.current) {
      textarea.current.value = raw;
    }
    onResize();
    const base64String = base64FromBuffer(staticData.harfbuzz);
    const controller = new AbortController();
    Promise.all([
      loadHarfbuzz("data:application/wasm;base64," + base64String),
      document.fonts.load("48px 'Egyptian Text Proto'", "𓏞𓏜"),
    ])
      .then(() => {
        if (controller.signal.aborted) {
          return;
        }
        if (main.current) {
          main.current.classList.remove("mainInit");
          main.current.classList.add("mainFadeIn");
        }
        setFont(new FontData(staticData.eot));
      })
      .catch((e) => console.error(e));
    window.addEventListener("resize", onResize);
    return () => {
      controller.abort();
      font?.destroy();
      content.current?.destroy();
      window.removeEventListener("resize", onResize);
    };
  }, []);
  useEffect(() => {
    const c = content.current;
    if (c) {
      window.history.replaceState({ version: stateVersion, state: c.raw }, "");
    }
    return () => {
      c?.destroy();
    };
  }, [content.current]);
  useEffect(() => {
    if (content.current !== undefined || font === undefined) {
      return;
    }
    const value = textarea.current?.value;
    if (value === undefined) {
      return;
    }
    const c = new Content(value, font);
    setContent(c);
  }, [font]);
  return (
    <div ref={main} className="main mainInit">
      <div className="header">
        <div
          style={{ display: "flex", cursor: "pointer" }}
          onClick={() => setAboutVisible(true)}
        >
          <div>W</div>
          <div
            className="seshContainer"
            onMouseEnter={onMouseEnter}
            onMouseLeave={onMouseLeave}
          >
            <div ref={seshA} className="seshA">
              Sesh
            </div>
            <div ref={seshB} className="hieroglyphs seshB seshBInit">
              𓏞𓏜
            </div>
          </div>
        </div>
        <div style={{ width: "20px" }} />
        <div className="menuBar">
          <div className="menuBarCategoryHeader">
            <div className="menuBarCategoryHeaderInner">Export as file:</div>
          </div>
          <div
            className="menuBarItem"
            onClick={onClickExportPng}
            data-enabled={font !== undefined}
          >
            <div className="menuBarItemInner">PNG</div>
          </div>
          <div
            className="menuBarItem"
            onClick={onClickExportSvg}
            data-enabled={font !== undefined}
          >
            <div className="menuBarItemInner">SVG</div>
          </div>
          <div
            className="menuBarItem"
            onClick={onClickExportPdf}
            data-enabled={font !== undefined}
          >
            <div className="menuBarItemInner">PDF</div>
          </div>
          <div style={{ width: "40px" }} />
          <div className="menuBarCategoryHeader">
            <div className="menuBarCategoryHeaderInner">Copy to clipboard:</div>
          </div>
          <div
            className="menuBarItem"
            onClick={onClickCopyPng}
            data-enabled={font !== undefined}
          >
            <div className="menuBarItemInner">PNG</div>
          </div>
          <div
            className="menuBarItem"
            onClick={onClickCopyPlainUnicode}
            data-enabled={font !== undefined}
            title={
              "Copy the hieroglyph text without Egyptian Hieroglyph Format Controls"
            }
          >
            <div className="menuBarItemInner">Plain Text</div>
          </div>
          <div
            className="menuBarItem"
            onClick={onClickCopyUnicode}
            data-enabled={font !== undefined}
            title={
              "Copy the hieroglyph text with Egyptian Hieroglyph Format Controls"
            }
          >
            <div className="menuBarItemInner">Text</div>
          </div>
        </div>
      </div>
      <div style={{ display: "flex", height: "50vh" }}>
        <div style={{ display: "flex", width: "50%" }}>
          <div style={{ padding: "4px", width: "100%" }}>
            <textarea
              ref={textarea}
              className="textArea"
              autoFocus={true}
              spellCheck={false}
              placeholder={changed ? undefined : placeholder}
              onChange={onChange}
              onSelect={onSelect}
              onFocus={onFocus}
              onBlur={onBlur}
            />
          </div>
        </div>
        <div
          style={{
            width: "50%",
            height: "100%",
            backgroundColor: "#ccc",
            overflow: "hidden",
          }}
        >
          <div
            style={{
              position: "relative",
              opacity: changed ? 1 : 0.5,
            }}
          >
            {content.current && font && (
              <ContentComponent
                content={content}
                fontSize={fontSize}
                font={font}
                lineSpacing={lineSpacing}
                edgeInset={edgeInset}
                cursor={focus ? cursor : undefined}
                cursorPadding={cursorPadding}
              />
            )}
          </div>
        </div>
      </div>
      <div style={{ height: "calc(50vh - var(--headerHeight))" }}>
        <div style={{ display: "flex", flexDirection: "column" }}>
          <div
            className="signListTab"
            style={{ height: `calc(27px * ${tabRows})` }}
          >
            <div
              className="signListTabButton"
              data-active={activeSignListTab === "typing"}
              onClick={() => setActiveSignListTab("typing")}
            >
              typing
            </div>
            {SignList.categories.map((cat, index) => (
              <div
                className="signListTabButton"
                data-active={activeSignListTab === cat}
                onClick={() => setActiveSignListTab(cat)}
                key={index}
              >
                {cat}&nbsp;
                <span className="hieroglyphs">
                  {
                    SignList.signs[
                      SignList.signs.findIndex(([s, _]) => {
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
            className="signList"
            style={{
              height: `calc(50vh - 27px * ${tabRows} - var(--headerHeight))`,
            }}
          >
            {SignList.signs
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
                  title={SignList.mapReverse(sign)?.join(", ")}
                >
                  <div className="signListCellHeader"> {id} </div>
                  <div className="signListCellSign">{sign}</div>
                </div>
              ))}
          </div>
        </div>
      </div>
      {isAboutVisible && <About onClose={() => setAboutVisible(false)} />}
    </div>
  );
};
