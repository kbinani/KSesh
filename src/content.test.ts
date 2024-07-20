import { describe, expect, test } from "vitest";
import { Content } from "./content";

describe("content", () => {
  describe("constructor", () => {
    test("basic", () => {
      const c = new Content("M23-Z7-M18-M17-Z4:D54-G17-X1:G1-N25:X1*Z1");
      expect(c.result).toBe("𓇓𓏲𓇍𓇋𓏭𓐰𓂻𓅓𓏏𓐰𓄿𓈉𓐰𓏏𓐱𓏤");
    });
  });
});
