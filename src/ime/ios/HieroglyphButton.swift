import UIKit

class HieroglyphButton: Button, ShiftableKeyCap {
  enum Mode {
    case hieroglyph
    case transcription
    
    var rotated: Self {
      switch self {
      case .hieroglyph:
        return .transcription
      case .transcription:
        return .hieroglyph
      }
    }
  }
  
  var shifted: Bool = false {
    didSet {
      guard shifted != oldValue else {
        return
      }
      updateAppearance()
      updateMode()
    }
  }
  
  let category: String
  let hieroglyph: String
  let transcription: Transcription?
  var mode: Mode = .hieroglyph {
    didSet {
      guard oldValue != mode else {
        return
      }
      updateMode()
    }
  }
  
  private let topLabel: UILabel
  private let bottomLabel: UILabel
  private let iconView: UIImageView
  
  init(category: String, hieroglyph: String, transcription: Transcription? = nil) {
    self.category = category
    self.hieroglyph = hieroglyph
    self.transcription = transcription
    topLabel = UILabel()
    bottomLabel = UILabel()
    iconView = UIImageView()
    super.init()
    commonInit()
    updateMode()
  }
  
  required init?(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }
  
  private func commonInit() {
    addSubview(topLabel)
    addSubview(bottomLabel)
    addSubview(iconView)
    
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
      height: topLabel.font.pointSize
    )
    
    bottomLabel.font = Font.get(size: size.height * 0.5)
    bottomLabel.frame = .init(
      x: Style.hMargin,
      y: size.height - Style.vMargin - bottomLabel.font.pointSize,
      width: width,
      height: bottomLabel.font.pointSize
    )
    
    if let image = iconView.image {
      let iconHeight = size.height * 0.7
      let iconWidth = iconHeight * image.size.width / image.size.height
      let xOffset: CGFloat = iconWidth * 0.14
      let yOffset: CGFloat = -iconHeight * 0.05
      iconView.frame = .init(
        x: size.width * 0.5 - iconWidth * 0.5 + xOffset,
        y: size.height * 0.5 - iconHeight * 0.5 + yOffset,
        width: iconWidth,
        height: iconHeight
      )
    }
    
    super.layoutSubviews()
  }
  
  private func updateMode() {
    if case .transcription = mode, let transcription {
      topLabel.isHidden = true
      bottomLabel.isHidden = true
      iconView.isHidden = false
      if let image = UIImage(named: transcription.resourceName(shifted: shifted)) {
        iconView.image = image
      } else {
        iconView.image = UIImage(named: transcription.resourceName(shifted: false))
      }
    } else {
      topLabel.isHidden = false
      bottomLabel.isHidden = false
      iconView.isHidden = true
      iconView.image = nil
    }
    setNeedsLayout()
  }
}
