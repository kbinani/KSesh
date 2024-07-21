import { describe, expect, test } from "vitest";
import { Content } from "./content";

describe("content", () => {
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
