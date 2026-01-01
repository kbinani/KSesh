import CoreText

extension CTRun {
  @discardableResult
  func useGlyphs(_ block: (_ glyph: CGGlyph, _ position: CGPoint, _ stringIndex: CFIndex) -> Bool) -> Bool {
    let count = CTRunGetGlyphCount(self)
    var glyphs = [CGGlyph](repeating: 0, count: count)
    var positions = [CGPoint](repeating: .zero, count: count)
    var indices = [CFIndex](repeating: 0, count: count)
    CTRunGetGlyphs(self, .init(location: 0, length: count), &glyphs)
    CTRunGetPositions(self, .init(location: 0, length: count), &positions)
    CTRunGetStringIndices(self, .init(location: 0, length: count), &indices)
    for i in 0..<count {
      guard block(glyphs[i], positions[i], indices[i]) else {
        return false
      }
    }
    return true
  }
}
