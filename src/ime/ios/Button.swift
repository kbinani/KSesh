import UIKit

class Button: UIButton {
  enum KeyCapRole {
    case regular
    case special
  }
  
  private let keyCapRole: KeyCapRole
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
  
  convenience init(top: String? = nil, middle: String? = nil, bottom: String? = nil) {
    self.init(keyCapRole: .regular)
    self.topLabel.text = top
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
    let middleLabel = UILabel()
    let bottomLabel = UILabel()
    self.topLabel = topLabel
    self.middleLabel = middleLabel
    self.bottomLabel = bottomLabel
    self.addSubview(middleLabel)
    self.addSubview(topLabel)
    self.addSubview(bottomLabel)
    
    topLabel.textAlignment = .left
    topLabel.contentMode = .top
    middleLabel.textAlignment = .center
    middleLabel.contentMode = .center
    bottomLabel.textAlignment = .right
    bottomLabel.contentMode   = .bottom
    
    self.layer.cornerRadius = 10
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
    let textColor: UIColor
    let baseBackgroundColor: UIColor
    if case .dark = appearance {
      textColor = UIColor.white
      if isHighlighted {
        baseBackgroundColor = #colorLiteral(red: 0.4863533378, green: 0.4863144159, blue: 0.4910370708, alpha: 1)
      } else {
        baseBackgroundColor = #colorLiteral(red: 0.2353515327, green: 0.2353515029, blue: 0.2353515029, alpha: 1)
      }
    } else {
      textColor = UIColor.black
      switch keyCapRole {
      case .regular:
        if isHighlighted {
          baseBackgroundColor = #colorLiteral(red: 0.6213026643, green: 0.6484116316, blue: 0.6926683784, alpha: 1)
        } else {
          baseBackgroundColor = .white
        }
      case .special:
        if isHighlighted {
          baseBackgroundColor = .white
        } else {
          baseBackgroundColor = #colorLiteral(red: 0.6235535145, green: 0.6482734084, blue: 0.6943539977, alpha: 1)
        }
      }
    }
    configuration?.baseBackgroundColor = baseBackgroundColor
    topLabel.textColor = textColor
    middleLabel.textColor = textColor
    bottomLabel.textColor = textColor
  }
  
  override func layoutSubviews() {
    let size = self.bounds.size
    let hMargin: CGFloat = 8
    let vMargin: CGFloat = 8
    let width = size.width - 2 * hMargin
    topLabel.frame = .init(
      x: hMargin,
      y: vMargin,
      width: width,
      height: topLabel.font.pointSize
    )
    middleLabel.frame = .init(
      x: hMargin,
      y: size.height * 0.5 - middleLabel.font.pointSize * 0.5,
      width: width,
      height: middleLabel.font.pointSize
    )
    bottomLabel.frame = .init(
      x: hMargin,
      y: size.height - vMargin - bottomLabel.font.pointSize,
      width: width,
      height: bottomLabel.font.pointSize
    )
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
