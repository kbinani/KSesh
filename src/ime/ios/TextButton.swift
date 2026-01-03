import UIKit

class TextButton: Button, KeyCap {
  private let middleLabel: UILabel
  
  init(text: String) {
    middleLabel = UILabel()
    middleLabel.text = text
    super.init()
    commonInit()
  }
  
  required init?(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }

  private func commonInit() {
    self.addSubview(middleLabel)
    
    middleLabel.textAlignment = .center
    middleLabel.contentMode = .center
    middleLabel.baselineAdjustment = .alignCenters
  }
  
  override func updateAppearance() {
    middleLabel.textColor = appearance?.tintColor
    super.updateAppearance()
  }
  
  override func layoutSubviews() {
    let size = self.bounds.size
    let width = size.width - 2 * Style.hMargin

    middleLabel.frame = .init(
      x: Style.hMargin,
      y: size.height * 0.5 - middleLabel.font.pointSize * 0.5,
      width: width,
      height: middleLabel.font.pointSize
    )
    super.layoutSubviews()
  }
}
