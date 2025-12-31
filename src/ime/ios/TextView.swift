import UIKit

class TextView: UIView {
  struct Content: Equatable {
    let leading: String
    let trailing: String
  }

  private enum Style {
    static let lineHeightRatio: CGFloat = 0.9
    static let caretLineWidthRatio: CGFloat = 0.025
  }

  private struct Metrics {
    let fontSize: CGFloat
    let ascent: CGFloat
    let descent: CGFloat
  }
  
  private var metrics: Metrics?
  
  private struct Presentation {
    let line: CTLine
    let width: CGFloat
    let center: CGFloat
  }
  private var presentation: Presentation?
  
  var appearance: UIKeyboardAppearance? = nil {
    didSet {
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
    let center = CTLineGetOffsetForStringIndex(line, content.leading.utf16.count, nil)
    self.presentation = .init(line: line, width: line.typographicBounds.width, center: center)
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

    ctx.saveGState()
    ctx.setLineWidth(size.height * Style.caretLineWidthRatio)
    ctx.setStrokeColor(UIColor.systemBlue.cgColor)
    ctx.setLineCap(.round)
    ctx.strokeLineSegments(between: [
      .init(x: size.width * 0.5, y: size.height * 0.5 - size.height * Style.lineHeightRatio * 0.5),
      .init(x: size.width * 0.5, y: size.height * 0.5 + size.height * Style.lineHeightRatio * 0.5)
    ])
    ctx.restoreGState()

    ctx.translateBy(x: 0, y: size.height)
    ctx.scaleBy(x: 1, y: -1)
    ctx.translateBy(x: size.width * 0.5 - presentation.center, y: size.height * 0.5 - metrics.ascent * 0.5 + metrics.descent * 0.5)
    
    CTLineDraw(presentation.line, ctx)
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
    let metrics = Metrics(fontSize: fontSize, ascent: -bounds.minY, descent: bounds.height + bounds.minY)
    self.metrics = metrics
    return metrics
  }
}
