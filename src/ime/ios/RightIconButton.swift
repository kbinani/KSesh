import UIKit

class RightIconButton: Button, KeyCap {
  private let iconView: UIImageView

  init(icon: UIImage?) {
    iconView = UIImageView(image: icon)
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
        x: size.width - Style.hMargin - iconWidth,
        y: size.height - Style.vMargin - iconHeight,
        width: iconWidth,
        height: iconHeight
      )
    }
    super.layoutSubviews()
  }
}
