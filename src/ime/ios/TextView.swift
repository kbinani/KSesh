import UIKit

class TextView: UIView {
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
  
  private var content: TextViewContent?
  
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
  
  var source: TextViewSource = .init(leading: "", trailing: "") {
    didSet {
      guard source != oldValue else {
        return
      }
      updateContent()
      setNeedsDisplay()
    }
  }
  
  private var textColor: UIColor = .black
  
  private func updateContent() {
    guard let metrics = ensureMetrics() else {
      self.content = nil
      return
    }
    guard let font = Font.get(size: metrics.fontSize) else {
      self.content = nil
      return
    }
    self.content = .init(
      string: source.leading + source.trailing,
      font: font,
      centerStringIndex: source.leading.utf16.count,
      caretLineWidth: metrics.caretLineWidth
    )
  }
  
  override func layoutSubviews() {
    super.layoutSubviews()
    updateContent()
    layer.cornerRadius = bounds.height * 0.16
    clipsToBounds = true
  }
  
  override func draw(_ rect: CGRect) {
    guard let content, let ctx = UIGraphicsGetCurrentContext(), let metrics = ensureMetrics() else {
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
      x: size.width * 0.5 - content.center,
      y: size.height * 0.5 - metrics.ascent * 0.5 + metrics.descent * 0.5
    )
    
    ctx.saveGState()
    content.line.useGlyphs { run, glyph, position, stringIndex in
      guard let font = run.font else {
        return true
      }
      if content.deleteRange.lowerBound <= stringIndex && stringIndex < content.deleteRange.upperBound {
        ctx.setFillColor(UIColor.systemRed.cgColor)
      } else {
        ctx.setFillColor(textColor.cgColor)
      }
      CTFontDrawGlyphs(font, [glyph], [position], 1, ctx)
      return true
    }
    ctx.restoreGState()
    
    if let cursorBounds = content.cursorBounds {
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
