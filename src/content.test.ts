import { describe, expect, test } from "vitest";
import { Content } from "./content";
import { init } from "./init";
import { SignList } from "./sign-list";

describe("content", async () => {
  const font = await init();
  test("basic", () => {
    const c = new Content("i w sDm m A1 r : 1 n km m t : niwt", font);
    expect(c.textWithControls).toBe("𓇋𓅱𓄔𓅓𓀀𓂋\u{13430}𓏤𓈖𓆎𓅓𓏏\u{13430}𓊖");
    expect(
      c.cursorLocation({ location: 8, direction: "forward" }),
    ).toStrictEqual({ lineIndex: 0, clusterIndex: 3, type: "left" });
    expect(
      c.cursorLocation({ location: 9, direction: "forward" }),
    ).toStrictEqual({
      lineIndex: 0,
      clusterIndex: 3,
      type: "right",
      block: true,
    });
    expect(
      c.cursorLocation({ location: 10, direction: "forward" }),
    ).toStrictEqual({ lineIndex: 0, clusterIndex: 4, type: "left" });
  });
});
