import UIKit

struct TextViewContent {
  let string: NSAttributedString
  let line: CTLine
  let width: CGFloat
  let center: CGFloat
  let centerStringIndex: Int
  let cursorBounds: CGRect?
  let deleteRange: NSRange
  
  init(string: String, font: UIFont, centerStringIndex: Int, caretLineWidth: CGFloat) {
    let f = UIFont.systemFont(ofSize: font.pointSize)
    let s = NSMutableAttributedString(string: string, attributes: [.font: f])
    var index: Int = 0
    var last: UnicodeScalar?
    for scalar in s.string.unicodeScalars {
      let count = scalar.utf16.count
      if scalar.isHieroglyph || (scalar.isVariationSelector && last?.isHieroglyph == true) {
        s.addAttribute(.font, value: font, range: .init(location: index, length: count))
      }
      last = scalar
      index += count
    }
    self.centerStringIndex = centerStringIndex
    
    let ns: NSString = s.string as NSString
    let backspaceDeletingCount = ns.backspaceDeletingRange(at: centerStringIndex)
    let deleteRange = NSRange(
      location: centerStringIndex - backspaceDeletingCount,
      length: backspaceDeletingCount
    )
    self.deleteRange = deleteRange
    
    let line = CTLineCreateWithAttributedString(s)
    self.string = s
    self.line = line
    let bounds = line.typographicBounds
    self.width = bounds.width
    
    let center = CTLineGetOffsetForStringIndex(line, centerStringIndex, nil)
    
    let deleteString = ns.substring(with: deleteRange)
    if centerStringIndex == 0 {
      if let rect = line.textRect(for: .init(location: centerStringIndex, length: 0)) {
        self.cursorBounds = .init(x: rect.minX - caretLineWidth, y: rect.minY, width: 0, height: rect.height)
      } else {
        self.cursorBounds = .init(x: 0, y: 0, width: 0, height: bounds.height)
      }
    } else if (deleteString == .verticalJoin || deleteString == .horizontalJoin) && deleteRange.lowerBound > 0 {
      let leadingRange = ns.rangeOfComposedCharacterSequences(for: .init(location: deleteRange.lowerBound - 1, length: 1))
      if let rect = line.textRect(for: leadingRange) {
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
      if let rect = line.textRect(for: deleteRange) {
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
