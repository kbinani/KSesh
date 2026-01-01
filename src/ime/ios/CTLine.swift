import CoreText

extension CTLine {
  var typographicBounds: CGRect {
    var ascent: CGFloat = 0
    var descent: CGFloat = 0
    var leading: CGFloat = 0
    let width = CTLineGetTypographicBounds(self, &ascent, &descent, &leading)
    return .init(x: leading, y: -ascent, width: width, height: ascent + descent)
  }
  
  var runs: [CTRun] {
    return (CTLineGetGlyphRuns(self) as [AnyObject]).map { $0 as! CTRun }
  }
  
  @discardableResult
  func useGlyphs(_ block: (_ glyph: CGGlyph, _ position: CGPoint, _ stringIndex: CFIndex) -> Bool) -> Bool {
    for run in runs {
      guard run.useGlyphs(block) else {
        return false
      }
    }
    return true
  }
}
