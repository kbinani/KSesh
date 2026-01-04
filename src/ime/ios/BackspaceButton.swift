import UIKit

class BackspaceButton: Button, KeyCap {
  private let iconView: UIImageView
  private let topLabel: UILabel
  
  override init() {
    topLabel = UILabel()
    iconView = UIImageView(image: .init(systemName: "delete.left"))
    super.init()
    commonInit()
  }
  
  required init?(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }

  private func commonInit() {
    addSubview(topLabel)
    topLabel.textAlignment = .left
    topLabel.minimumScaleFactor = 0
    topLabel.adjustsFontSizeToFitWidth = true
    topLabel.numberOfLines = 2
    
    addSubview(iconView)
  }
  
  override func updateAppearance() {
    topLabel.textColor = appearance?.secondaryTintColor
    iconView.tintColor = appearance?.tintColor
    super.updateAppearance()
  }
  
  override func layoutSubviews() {
    let size = self.bounds.size
    let width = size.width - 2 * Style.hMargin

    topLabel.frame = .init(
      x: Style.hMargin,
      y: Style.vMargin,
      width: width,
      height: topLabel.font.pointSize * CGFloat(topLabel.numberOfLines)
    )

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
  
  var topText: String? {
    get {
      return topLabel.text
    }
    set {
      topLabel.text = newValue
    }
  }
}
