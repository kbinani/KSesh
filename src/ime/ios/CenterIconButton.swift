import UIKit

class CenterIconButton: Button, KeyCap {
  private let iconView: UIImageView

  init(icon: UIImage?) {
    self.iconView = UIImageView(image: icon)
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
    let iconSizeRatio: CGFloat = 0.3
    
    if let image = iconView.image {
      let iconHeight = size.height * iconSizeRatio
      let iconWidth = iconHeight * image.size.width / image.size.height
      iconView.frame = .init(
        x: size.width * 0.5 - iconWidth * 0.5,
        y: size.height * 0.5 - iconHeight * 0.5,
        width: iconWidth,
        height: iconHeight
      )
    }
    super.layoutSubviews()
  }
}
