import UIKit

class TextView: UIView {
  enum Style {
    static let lineHeightRatio: CGFloat = 0.9
  }
  
  private var line: CTLine?
  
  var appearance: UIKeyboardAppearance? = nil {
    didSet {
    }
  }
  
  var text: String = "" {
    didSet {
      guard text != oldValue else {
        return
      }
      update()
    }
  }
  
  private func update() {
    let fontSize = bounds.height * Style.lineHeightRatio
    guard let h = UIFont(name: "EglyfDebugNG-Regular", size: fontSize) else {
      self.line = nil
      return
    }
    let f = UIFont.systemFont(ofSize: fontSize)
    let s = NSMutableAttributedString(string: text, attributes: [.font: f])
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
    guard let line, let ctx = UIGraphicsGetCurrentContext() else {
      return
    }
    let size = bounds.size
    var ascent: CGFloat = 0
    var descent: CGFloat = 0
    var leading: CGFloat = 0
    let width = CTLineGetTypographicBounds(line, &ascent, &descent, &leading)
    ctx.saveGState()
    defer {
      ctx.restoreGState()
    }
    ctx.translateBy(x: 0, y: size.height)
    ctx.scaleBy(x: 1, y: -1)
    ctx.translateBy(x: size.width * 0.5 - width * 0.5, y: size.height * 0.5 - ascent * 0.5 + descent * 0.5)
    CTLineDraw(line, ctx)
  }
}

extension UnicodeScalar {
  var isHieroglyph: Bool {
    // Egyptian Hieroglyphs
    // Range: 13000–1342F
    if 0x13000 <= value && value <= 0x1342F {
      return true
    }
    // Egyptian Hieroglyphs Extended-A
    // Range: 13460–143FF
    if 0x13460 <= value && value <= 0x143FF {
      return true
    }
    // Egyptian Hieroglyph Format Controls
    // Range: 13430–1345F
    if 0x13430 <= value && value <= 0x1345F {
      return true
    }
    return false
  }
  
  var isVariationSelector: Bool {
    return 0xFE00 <= value && value <= 0xFE0F
  }
}
