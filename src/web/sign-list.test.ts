import { SignList } from "./sign-list";
import { describe, expect, test } from "vitest";

describe("sign-list", async () => {
  test("mapping", async () => {
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
