import { describe, test, expect } from "vitest";
import { Content } from "./content";

describe("content", () => {
  describe("constructor", () => {
    test("basic", () => {
      const c = Content.Parse("M23-Z7-M18-M17-Z4:D54-G17-X1:G1-N25:X1*Z1");
      expect(c.lines.length).toBe(1);
      expect(c.lines[0].units.length).toBe(8);
      expect(c.lines[0].units[0].segments.length).toBe(1);
      expect(c.lines[0].units[0].segments[0].glyphs.length).toBe(1);
      expect(c.lines[0].units[0].segments[0].glyphs[0].char).toBe("M23");
      expect(c.lines[0].units[1].segments.length).toBe(1);
      expect(c.lines[0].units[1].segments[0].glyphs.length).toBe(1);
      expect(c.lines[0].units[1].segments[0].glyphs[0].char).toBe("Z7");
      expect(c.lines[0].units[2].segments.length).toBe(1);
      expect(c.lines[0].units[2].segments[0].glyphs.length).toBe(1);
      expect(c.lines[0].units[2].segments[0].glyphs[0].char).toBe("M18");
      expect(c.lines[0].units[3].segments.length).toBe(1);
      expect(c.lines[0].units[3].segments[0].glyphs.length).toBe(1);
      expect(c.lines[0].units[3].segments[0].glyphs[0].char).toBe("M17");
      expect(c.lines[0].units[4].segments.length).toBe(2);
      expect(c.lines[0].units[4].segments[0].glyphs.length).toBe(1);
      expect(c.lines[0].units[4].segments[0].glyphs[0].char).toBe("Z4");
      expect(c.lines[0].units[4].segments[1].glyphs.length).toBe(1);
      expect(c.lines[0].units[4].segments[1].glyphs[0].char).toBe("D54");
      expect(c.lines[0].units[5].segments.length).toBe(1);
      expect(c.lines[0].units[5].segments[0].glyphs.length).toBe(1);
      expect(c.lines[0].units[5].segments[0].glyphs[0].char).toBe("G17");
      expect(c.lines[0].units[6].segments.length).toBe(2);
      expect(c.lines[0].units[6].segments[0].glyphs.length).toBe(1);
      expect(c.lines[0].units[6].segments[0].glyphs[0].char).toBe("X1");
      expect(c.lines[0].units[6].segments[1].glyphs.length).toBe(1);
      expect(c.lines[0].units[6].segments[1].glyphs[0].char).toBe("G1");
      expect(c.lines[0].units[7].segments.length).toBe(2);
      expect(c.lines[0].units[7].segments[0].glyphs.length).toBe(1);
      expect(c.lines[0].units[7].segments[0].glyphs[0].char).toBe("N25");
      expect(c.lines[0].units[7].segments[1].glyphs.length).toBe(2);
      expect(c.lines[0].units[7].segments[1].glyphs[0].char).toBe("X1");
      expect(c.lines[0].units[7].segments[1].glyphs[1].char).toBe("Z1");
    });
  });
});
