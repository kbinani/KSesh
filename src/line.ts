export class Line {
  static Parse(line: string): Line {
    const units: Unit[] = [];
    for (const source of line.split("-")) {
      const unit = Unit.Parse(source);
      units.push(unit);
    }
    return new Line(units);
  }

  private constructor(readonly units: ReadonlyArray<Unit>) {}
}

export class Unit {
  private constructor(readonly segments: ReadonlyArray<Segment>) {}

  static Parse(source: string): Unit {
    const segments: Segment[] = [];
    for (const s of source.split(":")) {
      const segment = Segment.Parse(s);
      segments.push(segment);
    }
    return new Unit(segments);
  }
}

export class Segment {
  private constructor(readonly glyphs: ReadonlyArray<Glyph>) {}

  static Parse(source: string): Segment {
    const glyphs: Glyph[] = [];
    for (const g of source.split("*")) {
      const glyph = Glyph.Parse(g);
      glyphs.push(glyph);
    }
    return new Segment(glyphs);
  }
}

export class Glyph {
  private constructor(readonly char: string) {}

  static Parse(source: string): Glyph {
    return new Glyph(source);
  }
}
