import UIKit

class AlephButton: Button, KeyCap {
  private let iconView: UIImageView
  private let label: UILabel
  var mode: HieroglyphButton.Mode = .hieroglyph {
    didSet {
      guard oldValue != mode else {
        return
      }
      updateMode()
    }
  }
  
  override init() {
    iconView = UIImageView()
    label = UILabel()
    super.init()
    commonInit()
    updateMode()
  }
  
  required init?(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }
  
  private func commonInit() {
    addSubview(iconView)
    label.text = "𓀀"
    addSubview(label)
  }
  
  override func updateAppearance() {
    iconView.tintColor = appearance?.tintColor
    label.tintColor = appearance?.tintColor
    super.updateAppearance()
  }
  
  override func layoutSubviews() {
    let size = bounds.size
    let iconSize = size.height * 0.7
    if let image = iconView.image {
      let iconHeight = iconSize
      let iconWidth = iconHeight * image.size.width / image.size.height
      iconView.frame = .init(
        x: 0,
        y: size.height - iconHeight,
        width: iconWidth,
        height: iconHeight
      )
    }
    label.font = Font.get(size: size.height * 0.5)
    label.frame = .init(
      x: Style.hMargin,
      y: size.height - Style.vMargin - label.font.pointSize,
      width: size.width - Style.hMargin * 2,
      height: label.font.pointSize
    )
    super.layoutSubviews()
  }
  
  private func updateMode() {
    switch mode {
    case .hieroglyph:
      iconView.image = .init(named: "a1_small")
      iconView.isHidden = false
      label.isHidden = true
    case .transcription:
      label.isHidden = false
      iconView.isHidden = true
    }
  }
}
