import { Line } from "./line";

export class Content {
  readonly ok: boolean;
  readonly lines: ReadonlyArray<Line>;

  constructor(text: string) {
    this.ok = false;
    this.lines = [];
  }
}
