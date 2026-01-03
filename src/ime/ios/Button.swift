import UIKit

class Button: UIButton {
  enum Style {
    static let hMargin: CGFloat = 8
    static let vMargin: CGFloat = 8
    static let iconSizeRatio: CGFloat = 0.3
  }

  var appearance: UIKeyboardAppearance? {
    didSet {
      guard appearance != oldValue else {
        return
      }
      CATransaction.begin()
      defer {
        CATransaction.commit()
      }
      CATransaction.setDisableActions(true)
      configuration?.baseBackgroundColor = appearance?.baseBackgroundColor
      updateAppearance()
    }
  }
  
  init() {
    super.init(frame: .zero)
    self.autoresizesSubviews = false
    self.translatesAutoresizingMaskIntoConstraints = true
    self.configuration = UIButton.Configuration.filled()
    self.configurationUpdateHandler = { b in
      guard let button = b as? Self else {
        return
      }
      button.updateAppearance()
    }
    self.appearance = .light
  }
  
  required init?(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }
  
  func updateAppearance() {
    configuration?.baseBackgroundColor = appearance?.baseBackgroundColor
  }
  
  override func layoutSubviews() {
    configuration?.background.cornerRadius = self.bounds.height * 0.16
    super.layoutSubviews()
  }
}
