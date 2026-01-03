import UIKit

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
  func useGlyphs(_ block: (_ run: CTRun, _ glyph: CGGlyph, _ position: CGPoint, _ stringIndex: CFIndex) -> Bool) -> Bool {
    for run in runs {
      guard run.useGlyphs({ glyph, position, stringIndex in
        return block(run, glyph, position, stringIndex)
      }) else {
        return false
      }
    }
    return true
  }
  
  func textRect(for range: NSRange) -> CGRect? {
    var boundingBox = BoundingBox()
    if range.length == 0 {
      useGlyphs { run, glyph, position, stringIndex in
        guard range.location == stringIndex else {
          return true
        }
        guard let font = run.font else {
          return true
        }
        guard let bounds = font.opticalBounds(glyph), bounds.width > 0, bounds.height > 0 else {
          return true
        }
        boundingBox.add(bounds.offsetBy(dx: position.x, dy: position.y))
        return true
      }
      guard let rect = boundingBox.rect else {
        return nil
      }
      return .init(x: rect.minX, y: rect.minY, width: 0, height: rect.height)
    } else {
      useGlyphs { run, glyph, position, stringIndex in
        guard range.contains(stringIndex) else {
          return true
        }
        guard let font = run.font else {
          return true
        }
        guard let bounds = font.opticalBounds(glyph), bounds.width > 0, bounds.height > 0 else {
          return true
        }
        boundingBox.add(bounds.offsetBy(dx: position.x, dy: position.y))
        return true
      }
      return boundingBox.rect
    }
  }
}
