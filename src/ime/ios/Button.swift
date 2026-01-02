import UIKit

class Button: UIButton {
  enum KeyCapRole {
    case `default`
    case right
    case hieroglyph(category: String)
  }
  
  var leftIcon: UIImageView? {
    didSet {
      oldValue?.removeFromSuperview()
      if let leftIcon {
        leftIcon.tintColor = bottomLabel.textColor
        addSubview(leftIcon)
        setNeedsLayout()
      }
    }
  }
  var rightIcon: UIImageView? {
    didSet {
      oldValue?.removeFromSuperview()
      if let rightIcon {
        rightIcon.tintColor = bottomLabel.textColor
        addSubview(rightIcon)
        setNeedsLayout()
      }
    }
  }
  var centerIcon: UIImageView? {
    didSet {
      oldValue?.removeFromSuperview()
      if let centerIcon {
        centerIcon.tintColor = bottomLabel.textColor
        addSubview(centerIcon)
        setNeedsLayout()
      }
    }
  }
  
  var shifted: Bool = false {
    didSet {
      guard shifted != oldValue else {
        return
      }
      updateColors()
    }
  }
  
  let keyCapRole: KeyCapRole
  private var topLabel: UILabel!
  private var middleLabel: UILabel!
  private var bottomLabel: UILabel!
  
  init(keyCapRole: KeyCapRole) {
    self.keyCapRole = keyCapRole
    super.init(frame: .zero)
    commonInit()
  }
  
  required init?(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }
  
  convenience init(category: String, middle: String? = nil, bottom: String? = nil) {
    self.init(keyCapRole: .hieroglyph(category: category))
    self.topLabel.text = category
    self.middleLabel.text = middle
    self.bottomLabel.text = bottom
  }

  var appearance: UIKeyboardAppearance? = nil {
    didSet {
      updateColors()
    }
  }
  
  private func commonInit() {
    self.autoresizesSubviews = false
    self.translatesAutoresizingMaskIntoConstraints = true

    let topLabel = UILabel()
    self.topLabel = topLabel
    self.addSubview(topLabel)

    let middleLabel = UILabel()
    self.addSubview(middleLabel)
    self.middleLabel = middleLabel

    let bottomLabel = UILabel()
    self.bottomLabel = bottomLabel
    if case .hieroglyph = keyCapRole {
      
    } else {
      let base = UIFont.systemFont(ofSize: UIFont.labelFontSize)
      bottomLabel.font = base.fontDescriptor.withDesign(.serif).map({ descriptor in
        UIFont(descriptor: descriptor, size: UIFont.labelFontSize)
      })
    }
    self.addSubview(bottomLabel)
    
    topLabel.textAlignment = .left
    topLabel.contentMode = .top
    if case .hieroglyph = keyCapRole {
    } else {
      topLabel.minimumScaleFactor = 0
      topLabel.adjustsFontSizeToFitWidth = true
      topLabel.numberOfLines = 2
    }
    middleLabel.textAlignment = .center
    middleLabel.contentMode = .center
    middleLabel.baselineAdjustment = .alignCenters
    switch keyCapRole {
    case .`default`:
      bottomLabel.textAlignment = .left
    case .right:
      bottomLabel.textAlignment = .right
    case .hieroglyph:
      bottomLabel.textAlignment = .right
    }
    bottomLabel.contentMode   = .bottom
    
    self.configuration = UIButton.Configuration.filled()
    self.appearance = .light
    self.configurationUpdateHandler = { b in
      guard let button = b as? Button else {
        return
      }
      button.updateColors()
    }
  }
  
  private func updateColors() {
    CATransaction.begin()
    defer {
      CATransaction.commit()
    }
    CATransaction.setDisableActions(true)
    let mainTextColor: UIColor
    let subTextColor: UIColor
    let baseBackgroundColor: UIColor
    if case .dark = appearance {
      mainTextColor = UIColor.white
      subTextColor = #colorLiteral(red: 0.4665528536, green: 0.4665527344, blue: 0.4665527344, alpha: 1)
      if isHighlighted {
        baseBackgroundColor = #colorLiteral(red: 0.4863533378, green: 0.4863144159, blue: 0.4910370708, alpha: 1)
      } else {
        baseBackgroundColor = #colorLiteral(red: 0.2353515327, green: 0.2353515029, blue: 0.2353515029, alpha: 1)
      }
    } else {
      mainTextColor = UIColor.black
      subTextColor = #colorLiteral(red: 0.7524755597, green: 0.7555301785, blue: 0.7636918426, alpha: 1)
      if isHighlighted {
        baseBackgroundColor = #colorLiteral(red: 0.6213026643, green: 0.6484116316, blue: 0.6926683784, alpha: 1)
      } else {
        baseBackgroundColor = .white
      }
    }
    configuration?.baseBackgroundColor = baseBackgroundColor
    if shifted, case .hieroglyph = keyCapRole {
      topLabel.textColor = mainTextColor
      bottomLabel.textColor = subTextColor
    } else {
      topLabel.textColor = subTextColor
      bottomLabel.textColor = mainTextColor
    }
    middleLabel.textColor = mainTextColor
    leftIcon?.tintColor = mainTextColor
    rightIcon?.tintColor = mainTextColor
    centerIcon?.tintColor = mainTextColor
  }
  
  override func layoutSubviews() {
    let size = self.bounds.size
    let hMargin: CGFloat = 8
    let vMargin: CGFloat = 8
    let width = size.width - 2 * hMargin
    let iconSizeRatio: CGFloat = 0.3

    let topLabelHeight = if case .hieroglyph = keyCapRole {
      topLabel.font.pointSize
    } else {
      topLabel.font.pointSize * CGFloat(topLabel.numberOfLines)
    }
    topLabel.frame = .init(
      x: hMargin,
      y: vMargin,
      width: width,
      height: topLabelHeight
    )

    middleLabel.frame = .init(
      x: hMargin,
      y: size.height * 0.5 - middleLabel.font.pointSize * 0.5,
      width: width,
      height: middleLabel.font.pointSize
    )

    if case .hieroglyph = keyCapRole {
      bottomLabel.font = Font.get(size: size.height * 0.5)
    } else {
      bottomLabel.font = bottomLabel.font.withSize(size.height * 0.3)
    }
    bottomLabel.frame = .init(
      x: hMargin,
      y: size.height - vMargin - bottomLabel.font.pointSize,
      width: width,
      height: bottomLabel.font.pointSize
    )
    if let leftIcon, let image = leftIcon.image {
      leftIcon.tintColor = bottomLabel.textColor
      let iconHeight = size.height * iconSizeRatio
      let iconWidth = iconHeight * image.size.width / image.size.height
      leftIcon.frame = .init(
        x: hMargin,
        y: size.height - vMargin - iconHeight,
        width: iconWidth,
        height: iconHeight
      )
    }
    if let rightIcon, let image = rightIcon.image {
      rightIcon.tintColor = bottomLabel.textColor
      let iconHeight = size.height * iconSizeRatio
      let iconWidth = iconHeight * image.size.width / image.size.height
      rightIcon.frame = .init(
        x: size.width - hMargin - iconWidth,
        y: size.height - vMargin - iconHeight,
        width: iconWidth,
        height: iconHeight
      )
    }
    if let centerIcon, let image = centerIcon.image {
      centerIcon.tintColor = bottomLabel.textColor
      let iconHeight = size.height * iconSizeRatio
      let iconWidth = iconHeight * image.size.width / image.size.height
      centerIcon.frame = .init(
        x: size.width * 0.5 - iconWidth * 0.5,
        y: size.height * 0.5 - iconHeight * 0.5,
        width: iconWidth,
        height: iconHeight
      )
    }

    configuration?.background.cornerRadius = size.height * 0.16
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
  
  var middleText: String? {
    get {
      return middleLabel.text
    }
    set {
      middleLabel.text = newValue
    }
  }
  
  var bottomText: String? {
    get {
      return bottomLabel.text
    }
    set {
      bottomLabel.text = newValue
    }
  }
}

extension Button {
  static func shiftIcon(shifted: Bool) -> UIImage? {
    if shifted {
      return UIImage(systemName: "shift.fill")
    } else {
      return UIImage(systemName: "shift")
    }
  }
}
