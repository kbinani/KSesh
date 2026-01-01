import UIKit

class TextView: UIView {
  struct Content: Equatable {
    let leading: String
    let trailing: String
    
    init(leading: String, trailing: String) {
      if let last = leading.lastIndex(of: "\n") {
        let from = leading.index(after: last)
        self.leading = String(leading[from...])
      } else {
        self.leading = leading
      }
      if let first = trailing.firstIndex(of: "\n") {
        self.trailing = String(trailing[..<first])
      } else {
        self.trailing = trailing
      }
    }
  }

  private enum Style {
    static let lineHeightRatio: CGFloat = 0.9
    static let caretLineWidthRatio: CGFloat = 0.03
  }

  private struct Metrics {
    let fontSize: CGFloat
    let ascent: CGFloat
    let descent: CGFloat
    let caretLineWidth: CGFloat
  }
  
  private var metrics: Metrics?
  
  private struct Presentation {
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
        line.runs.forEach { run in
          run.useGlyphs { glyph, position, stringIndex in
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
        }
        if let rect = boundingBox.rect {
          self.cursorBounds = .init(x: rect.minX - caretLineWidth, y: rect.minY, width: 0, height: rect.height)
        } else {
          self.cursorBounds = .init(x: 0, y: 0, width: 0, height: bounds.height)
        }
      } else if (deleteString == .verticalJoin || deleteString == .horizontalJoin) && deleteRange.lowerBound > 0 {
        let leadingRange = ns.rangeOfComposedCharacterSequences(for: .init(location: deleteRange.lowerBound - 1, length: 1))
        var boundingBox = BoundingBox()
        line.runs.forEach { run in
          run.useGlyphs { glyph, position, stringIndex in
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
        line.runs.forEach { run in
          run.useGlyphs { glyph, position, stringIndex in
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
  private var presentation: Presentation?
  
  var appearance: UIKeyboardAppearance? = nil {
    didSet {
      guard appearance != oldValue else {
        return
      }
      if appearance == .dark {
        textColor = .white
      } else {
        textColor = .black
      }
      setNeedsDisplay()
    }
  }
  
  var content: Content = .init(leading: "", trailing: "") {
    didSet {
      guard content != oldValue else {
        return
      }
      update()
      setNeedsDisplay()
    }
  }

  private var textColor: UIColor = .black
  
  private func update() {
    guard let metrics = ensureMetrics() else {
      self.presentation = nil
      return
    }
    guard let h = Font.get(size: metrics.fontSize) else {
      self.presentation = nil
      return
    }
    let f = UIFont.systemFont(ofSize: metrics.fontSize)
    let s = NSMutableAttributedString(string: content.leading + content.trailing, attributes: [.font: f])
    var index: Int = 0
    var last: UnicodeScalar?
    for scalar in s.string.unicodeScalars {
      let count = scalar.utf16.count
      if scalar.isHieroglyph || (scalar.isVariationSelector && last?.isHieroglyph == true) {
        s.addAttribute(.font, value: h, range: .init(location: index, length: count))
      }
      last = scalar
      index += count
    }
    let line = CTLineCreateWithAttributedString(s)
    self.presentation = .init(
      string: s,
      line: line,
      centerStringIndex: content.leading.utf16.count,
      caretLineWidth: metrics.caretLineWidth
    )
  }
  
  override func layoutSubviews() {
    super.layoutSubviews()
    update()
    layer.cornerRadius = bounds.height * 0.16
    clipsToBounds = true
  }
  
  override func draw(_ rect: CGRect) {
    guard let presentation, let ctx = UIGraphicsGetCurrentContext(), let metrics = ensureMetrics() else {
      return
    }
    let size = bounds.size
    ctx.saveGState()
    defer {
      ctx.restoreGState()
    }
        
    ctx.translateBy(x: 0, y: size.height)
    ctx.scaleBy(x: 1, y: -1)
    ctx.translateBy(
      x: size.width * 0.5 - presentation.center,
      y: size.height * 0.5 - metrics.ascent * 0.5 + metrics.descent * 0.5
    )
    
    if let cursorBounds = presentation.cursorBounds {
      ctx.saveGState()
      ctx.setLineWidth(size.height * Style.caretLineWidthRatio)
      ctx.setStrokeColor(UIColor.systemBlue.cgColor)
      ctx.setLineCap(.round)
      ctx.strokeLineSegments(between: [
        .init(x: cursorBounds.minX, y: cursorBounds.minY),
        .init(x: cursorBounds.maxX, y: cursorBounds.maxY)
      ])
      ctx.restoreGState()
    }
    
    presentation.line.runs.forEach { run in
      run.useGlyphs { glyph, position, stringIndex in
        guard let font = presentation.string.attribute(.font, at: stringIndex, effectiveRange: nil) as? UIFont else {
          return true
        }
        if presentation.deleteRange.lowerBound <= stringIndex && stringIndex < presentation.deleteRange.upperBound {
          ctx.setFillColor(UIColor.systemRed.cgColor)
        } else {
          ctx.setFillColor(textColor.cgColor)
        }
        CTFontDrawGlyphs(font, [glyph], [position], 1, ctx)
        return true
      }
    }
  }
  
  private func ensureMetrics() -> Metrics? {
    let fontSize = bounds.height * Style.lineHeightRatio
    if let metrics, metrics.fontSize == fontSize {
      return metrics
    }
    guard let font = Font.get(size: fontSize) else {
      return nil
    }
    let sample = "𓍹𓐼𓀀𓍹𓐼𓀀𓐽𓍺𓐽𓍺"
    let str = NSAttributedString(string: sample, attributes: [.font: font])
    let line = CTLineCreateWithAttributedString(str)
    let bounds = line.typographicBounds
    let metrics = Metrics(
      fontSize: fontSize,
      ascent: -bounds.minY,
      descent: bounds.height + bounds.minY,
      caretLineWidth: bounds.height * Style.caretLineWidthRatio
    )
    self.metrics = metrics
    return metrics
  }
}
