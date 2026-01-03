import UIKit

class HieroglyphButton: Button, ShiftableKeyCap {
  var shifted: Bool = false {
    didSet {
      guard shifted != oldValue else {
        return
      }
      updateAppearance()
    }
  }
  
  let category: String
  let hieroglyph: String
  
  private let topLabel: UILabel
  private let bottomLabel: UILabel

  init(category: String, hieroglyph: String) {
    self.category = category
    self.hieroglyph = hieroglyph
    topLabel = UILabel()
    bottomLabel = UILabel()
    super.init()
    commonInit()
  }
  
  required init?(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }

  private func commonInit() {
    addSubview(topLabel)
    addSubview(bottomLabel)

    topLabel.text = category
    topLabel.textAlignment = .left
    topLabel.contentMode = .top

    bottomLabel.text = hieroglyph
    bottomLabel.textAlignment = .right
    bottomLabel.contentMode   = .bottom
  }
  
  override func updateAppearance() {
    if shifted {
      topLabel.textColor = appearance?.tintColor
      bottomLabel.textColor = appearance?.secondaryTintColor
    } else {
      topLabel.textColor = appearance?.secondaryTintColor
      bottomLabel.textColor = appearance?.tintColor
    }
    super.updateAppearance()
  }
  
  override func layoutSubviews() {
    let size = self.bounds.size
    let width = size.width - 2 * Style.hMargin

    topLabel.frame = .init(
      x: Style.hMargin,
      y: Style.vMargin,
      width: width,
      height: topLabel.font.pointSize
    )

    bottomLabel.font = Font.get(size: size.height * 0.5)
    bottomLabel.frame = .init(
      x: Style.hMargin,
      y: size.height - Style.vMargin - bottomLabel.font.pointSize,
      width: width,
      height: bottomLabel.font.pointSize
    )

    super.layoutSubviews()
  }
}
