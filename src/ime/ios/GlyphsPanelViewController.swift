import UIKit

@MainActor
protocol GlyphsPanelViewControllerDelegate: AnyObject {
  func glyphsPanelViewControllerWillDismiss(_ sender: GlyphsPanelViewController)
  func glyphsPanelViewController(_ sender: GlyphsPanelViewController, didTouchUpInsideKeyCap text: String)
  func glyphsPanelViewController(_ sender: GlyphsPanelViewController, didChangeShifted shifted: Bool)
}

class GlyphsPanelViewController: UIViewController {
  weak var delegate: GlyphsPanelViewControllerDelegate?

  @IBOutlet var container: ContainerView!
  
  private let glyphs: [(String, String)]
  private let buttons: [Button]
  private let buttonSize: CGSize
  private let gap: CGSize
  private var shifted: Bool {
    didSet {
      guard oldValue != shifted else {
        return
      }
      updateShiftState()
    }
  }
  private var shift: Button!
  private weak var parentShiftButton: UIView?
  private let swipeDownToCloseThreshold: CGFloat = 150
  
  init(glyphs: [(String, String)], buttonSize: CGSize, gap: CGSize, shifted: Bool, parentShiftButton: UIView) {
    self.glyphs = glyphs
    self.buttonSize = buttonSize
    self.gap = gap
    var buttons: [Button] = []
    for (key, glyph) in glyphs {
      let button = Button(category: key, bottom: glyph)
      button.shifted = shifted
      buttons.append(button)
    }
    self.buttons = buttons
    self.shifted = shifted
    self.shift = Button(keyCapRole: .default)
    shift.leftIcon = UIImageView(image: Button.shiftIcon(shifted: shifted))
    self.parentShiftButton = parentShiftButton
    super.init(nibName: "GlyphsPanelViewController", bundle: nil)
  }
  
  required init?(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }
  
  override func viewDidLoad() {
    super.viewDidLoad()

    for button in buttons {
      container.addSubview(button)
      button.addTarget(self, action: #selector(onTapKeyCap(_:)), for: .touchUpInside)
    }
    container.clipsToBounds = false
    container.autoresizesSubviews = false

    container.addSubview(shift)
    
    let pan = UIPanGestureRecognizer(target: self, action: #selector(onPanContainer(_:)))
    view.addGestureRecognizer(pan)
    shift.addTarget(self, action: #selector(onTapShiftKey(_:)), for: .touchUpInside)
  }
 
  override func viewDidLayoutSubviews() {
    let size = container.bounds.size
    let columns = Int(floor(size.width + gap.width) / (buttonSize.width + gap.width))
    let x0 = (size.width - buttonSize.width * CGFloat(columns) - gap.width * CGFloat(columns - 1)) * 0.5
    let flow = HorizontalViewFlow(
      origin: .init(x: x0 - gap.width, y: 0),
      height: buttonSize.height,
      gap: gap.width
    )
    var index: Int = 0
    for row in 0 ..< buttons.count {
      let y = (buttonSize.height + gap.height) * CGFloat(row)
      flow.reset(x: x0 - gap.width, y: y)
      for _ in 0 ..< columns {
        guard index < buttons.count else {
          break
        }
        let button = buttons[index]
        flow.next(width: buttonSize.width, button: button)
        index += 1
      }
      guard index < buttons.count else {
        break
      }
    }
    if let parentShiftButton {
      let frame = container.convert(parentShiftButton.frame, from: parentShiftButton.superview)
      self.shift.frame = frame
    }
    super.viewDidLayoutSubviews()
  }

  @objc private func onPanContainer(_ sender: UIPanGestureRecognizer) {
    let translation = sender.translation(in: container)
    let velocity = sender.velocity(in: container)
    switch sender.state {
    case .began:
      container.transform = .identity
      shift.transform = .identity
      view.alpha = 1
    case .changed:
      container.transform = .init(translationX: 0, y: max(0, translation.y))
      shift.transform = .init(translationX: 0, y: -max(0, translation.y))
      view.alpha = max(0, 1 - translation.y / swipeDownToCloseThreshold)
    case .ended:
      container.transform = .init(translationX: 0, y: max(0, translation.y))
      shift.transform = .init(translationX: 0, y: -max(0, translation.y))
      if translation.y > swipeDownToCloseThreshold || velocity.y > 1000 {
        delegate?.glyphsPanelViewControllerWillDismiss(self)
        dismiss(animated: false)
        sender.isEnabled = false
        sender.isEnabled = true
      } else {
        container.transform = .identity
        shift.transform = .identity
        view.alpha = 1
      }
    case .cancelled:
      container.transform = .identity
      shift.transform = .identity
      view.alpha = 1
    default:
      break
    }
  }
  
  @objc private func onTapShiftKey(_ sender: Button) {
    shifted.toggle()
    delegate?.glyphsPanelViewController(self, didChangeShifted: shifted)
  }
  
  private func updateShiftState() {
    buttons.forEach { button in
      button.shifted = shifted
    }
    shift.leftIcon = UIImageView(image: Button.shiftIcon(shifted: shifted))
  }

  @objc private func onTapKeyCap(_ sender: Button) {
    guard let key = sender.topText, let glyph = sender.bottomText else {
      return
    }
    let text = if shifted {
      key
    } else {
      glyph
    }
    delegate?.glyphsPanelViewController(self, didTouchUpInsideKeyCap: text)
  }
}
