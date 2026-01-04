import UIKit

class AlephButton: Button, KeyCap {
  private let iconView: UIImageView!
  var mode: HieroglyphButton.Mode = .hieroglyph {
    didSet {
      guard oldValue != mode else {
        return
      }
      updateMode()
    }
  }
  
  override init() {
    let iconView = UIImageView()
    self.iconView = iconView
    super.init()
    commonInit()
    updateMode()
  }
  
  required init?(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }
  
  private func commonInit() {
    self.addSubview(iconView)
  }
  
  override func updateAppearance() {
    iconView.tintColor = appearance?.tintColor
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
    super.layoutSubviews()
  }
  
  private func updateMode() {
    switch mode {
    case .hieroglyph:
      iconView.image = .init(named: "a1_small")
    case .transcription:
      break
      //TODO:
    }
  }
}
