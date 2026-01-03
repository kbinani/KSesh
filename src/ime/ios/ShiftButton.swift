import UIKit

class ShiftButton: Button, ShiftableKeyCap {
  private let iconView: UIImageView

  var shifted: Bool = false {
    didSet {
      guard shifted != oldValue else {
        return
      }
      iconView.image = Self.icon(shifted: shifted)
    }
  }
  
  override init() {
    iconView = UIImageView(image: Self.icon(shifted: false))
    super.init()
    addSubview(iconView)
  }
  
  required init?(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }

  override func updateAppearance() {
    iconView.tintColor = appearance?.tintColor
    super.updateAppearance()
  }
  
  override func layoutSubviews() {
    let size = self.bounds.size

    if let image = iconView.image {
      let iconHeight = size.height * Style.iconSizeRatio
      let iconWidth = iconHeight * image.size.width / image.size.height
      iconView.frame = .init(
        x: Style.hMargin,
        y: size.height - Style.vMargin - iconHeight,
        width: iconWidth,
        height: iconHeight
      )
    }
    super.layoutSubviews()
  }
  
  static func icon(shifted: Bool) -> UIImage? {
    if shifted {
      return UIImage(systemName: "shift.fill")
    } else {
      return UIImage(systemName: "shift")
    }
  }
}
