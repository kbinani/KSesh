import UIKit

class TextView: UIView {
  struct Content: Equatable {
    let leading: String
    let trailing: String
  }

  private enum Style {
    static let lineHeightRatio: CGFloat = 0.9
  }

  private struct Metrics {
    let fontSize: CGFloat
    let ascent: CGFloat
    let descent: CGFloat
  }
  
  private var metrics: Metrics?
  private var line: CTLine?
  
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
      self.line = nil
      return
    }
    guard let h = Font.get(size: metrics.fontSize) else {
      self.line = nil
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
    self.line = line
  }
  
  override func layoutSubviews() {
    super.layoutSubviews()
    update()
    layer.cornerRadius = bounds.height * 0.16
    clipsToBounds = true
  }
  
  override func draw(_ rect: CGRect) {
    guard let line, let ctx = UIGraphicsGetCurrentContext(), let metrics = ensureMetrics() else {
      return
    }
    let size = bounds.size
    let width = CTLineGetTypographicBounds(line, nil, nil, nil)
    ctx.saveGState()
    defer {
      ctx.restoreGState()
    }
    ctx.translateBy(x: 0, y: size.height)
    ctx.scaleBy(x: 1, y: -1)
    ctx.translateBy(x: size.width * 0.5 - width * 0.5, y: size.height * 0.5 - metrics.ascent * 0.5 + metrics.descent * 0.5)
    CTLineDraw(line, ctx)
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
