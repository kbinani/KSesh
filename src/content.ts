import { Line } from "./line";

export class Content {
  private constructor(readonly lines: ReadonlyArray<Line>) {}

  static Parse(text: string): Content {
    const lines: Line[] = [];
    for (const line of text.split("\n")) {
      const l = Line.Parse(line);
      lines.push(l);
    }
    return new Content(lines);
  }
}
