import UIKit

class AlephButton: Button, KeyCap {
  private var bottomLabel: UILabel!
  
  override init() {
    super.init()
    commonInit()
  }
  
  required init?(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }
  
  private func commonInit() {
    let bottomLabel = UILabel()
    self.bottomLabel = bottomLabel
    let base = UIFont.systemFont(ofSize: UIFont.labelFontSize)
    bottomLabel.font = base.fontDescriptor.withDesign(.serif).map({ descriptor in
      UIFont(descriptor: descriptor, size: UIFont.labelFontSize)
    })
    self.addSubview(bottomLabel)
    
    bottomLabel.textAlignment = .left
    bottomLabel.contentMode   = .bottom
    bottomLabel.text = "ȝ"
  }
  
  override func updateAppearance() {
    bottomLabel.textColor = appearance?.tintColor
    super.updateAppearance()
  }
  
  override func layoutSubviews() {
    let size = self.bounds.size
    let width = size.width - 2 * Style.hMargin
    
    bottomLabel.font = bottomLabel.font.withSize(size.height * 0.3)
    bottomLabel.frame = .init(
      x: Style.hMargin,
      y: size.height - Style.vMargin - bottomLabel.font.pointSize,
      width: width,
      height: bottomLabel.font.pointSize
    )
    super.layoutSubviews()
  }
}
