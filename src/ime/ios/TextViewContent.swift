import UIKit

struct TextViewContent {
  let string: NSAttributedString
  let line: CTLine
  let width: CGFloat
  let center: CGFloat
  let centerStringIndex: Int
  let cursorBounds: CGRect?
  let deleteRange: NSRange
  
  init(string: NSAttributedString, line: CTLine, centerStringIndex: Int, caretLineWidth: CGFloat) {
    self.string = string
    self.line = line
    self.centerStringIndex = centerStringIndex
    
    let bounds = line.typographicBounds
    self.width = bounds.width
    
    let ns: NSString = string.string as NSString
    let backspaceDeletingCount = ns.backspaceDeletingRange(at: centerStringIndex)
    let deleteRange = NSRange(
      location: centerStringIndex - backspaceDeletingCount,
      length: backspaceDeletingCount
    )
    self.deleteRange = deleteRange
    
    let center = CTLineGetOffsetForStringIndex(line, centerStringIndex, nil)
    
    let deleteString = ns.substring(with: deleteRange)
    if centerStringIndex == 0 {
      var boundingBox = BoundingBox()
      line.useGlyphs { glyph, position, stringIndex in
        guard stringIndex == 0 else {
          return true
        }
        guard let font = string.attribute(.font, at: stringIndex, effectiveRange: nil) as? UIFont else {
          return true
        }
        guard let bounds = font.opticalBounds(glyph), bounds.width > 0, bounds.height > 0 else {
          return true
        }
        boundingBox.add(bounds.offsetBy(dx: position.x, dy: position.y))
        return true
      }
      if let rect = boundingBox.rect {
        self.cursorBounds = .init(x: rect.minX - caretLineWidth, y: rect.minY, width: 0, height: rect.height)
      } else {
        self.cursorBounds = .init(x: 0, y: 0, width: 0, height: bounds.height)
      }
    } else if (deleteString == .verticalJoin || deleteString == .horizontalJoin) && deleteRange.lowerBound > 0 {
      let leadingRange = ns.rangeOfComposedCharacterSequences(for: .init(location: deleteRange.lowerBound - 1, length: 1))
      var boundingBox = BoundingBox()
      line.useGlyphs { glyph, position, stringIndex in
        guard leadingRange.lowerBound <= stringIndex, stringIndex < leadingRange.upperBound else {
          return true
        }
        guard let font = string.attribute(.font, at: stringIndex, effectiveRange: nil) as? UIFont else {
          return true
        }
        guard let bounds = font.opticalBounds(glyph), bounds.width > 0, bounds.height > 0 else {
          return true
        }
        boundingBox.add(bounds.offsetBy(dx: position.x, dy: position.y))
        return true
      }
      if let rect = boundingBox.rect {
        if deleteString == .verticalJoin {
          // vertical joiner
          self.cursorBounds = .init(x: rect.minX, y: rect.minY - caretLineWidth, width: rect.width, height: 0)
        } else {
          // horizontal joiner
          self.cursorBounds = .init(x: rect.maxX + caretLineWidth, y: rect.minY, width: 0, height: rect.height)
        }
      } else {
        self.cursorBounds = nil
      }
    } else {
      var boundingBox = BoundingBox()
      line.useGlyphs { glyph, position, stringIndex in
        guard deleteRange.lowerBound <= stringIndex, stringIndex < deleteRange.upperBound else {
          return true
        }
        guard let font = string.attribute(.font, at: stringIndex, effectiveRange: nil) as? UIFont else {
          return true
        }
        guard let bounds = font.opticalBounds(glyph), bounds.width > 0, bounds.height > 0 else {
          return true
        }
        boundingBox.add(bounds.offsetBy(dx: position.x, dy: position.y))
        return true
      }
      if let rect = boundingBox.rect {
        self.cursorBounds = .init(x: rect.maxX + caretLineWidth, y: rect.minY, width: 0, height: rect.height)
      } else {
        self.cursorBounds = nil
      }
    }
    if let cursorBounds {
      self.center = cursorBounds.midX
    } else {
      self.center = center
    }
  }
}
