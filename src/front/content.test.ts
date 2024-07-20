import { describe, expect, test } from "vitest";
import { Content } from "./content";

describe("content", () => {
  describe("constructor", () => {
    test("basic", () => {
      const c = new Content("M23-Z7-M18-M17-Z4:D54-G17-X1:G1-N25:X1*Z1");
      expect(c.result).toBe("𓇓𓏲𓇍𓇋𓏭𓐰𓂻𓅓𓏏𓐰𓄿𓈉𓐰𓏏𓐱𓏤");
    });
  });
  test("mapping", () => {
    const keys = new Set<string>();
    for (const s of Content.special) {
      const found = Content.map(s[0]);
      if (found === undefined) {
        expect.unreachable();
      }
      expect(found[1]).toBe(s[1]);
      expect(keys.has(s[0])).toBe(false);
      keys.add(s[0]);
    }
    for (const s of Content.signs) {
      const found = Content.map(s[0]);
      if (found === undefined) {
        expect.unreachable();
      }
      expect(found[1]).toBe(s[1]);
      expect(keys.has(s[0])).toBe(false);
      keys.add(s[0]);
    }
  });
});
