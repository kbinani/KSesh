import { describe, expect, test } from "vitest";
import { SignList } from "./sign-list";

describe("sign-list", () => {
  test("mapping", () => {
    const keys = new Set<string>();
    for (const s of SignList.special) {
      const found = SignList.map(s[0]);
      if (found === undefined) {
        expect.unreachable();
      }
      expect(found[1]).toBe(s[1]);
      expect(keys.has(s[0])).toBe(false);
      keys.add(s[0]);
    }
    for (const s of SignList.signs) {
      const found = SignList.map(s[0]);
      if (found === undefined) {
        expect.unreachable();
      }
      expect(found[1]).toBe(s[1]);
      expect(keys.has(s[0])).toBe(false);
      keys.add(s[0]);
    }
  });
});
