import { describe, expect, test } from "vitest";
import { Content } from "./content";
import { init } from "./init";

describe("content", async () => {
  const font = await init();
  test("ctor", () => {
    const c = new Content("A", font);
    expect(c.textWithControls).toBe("𓄿");
  });
});
