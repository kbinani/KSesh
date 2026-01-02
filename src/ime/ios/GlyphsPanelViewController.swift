import UIKit

@MainActor
protocol GlyphsPanelViewControllerDelegate: AnyObject {
  func glyphsPanelViewControllerWillDismiss(_ sender: GlyphsPanelViewController)
  func glyphsPanelViewController(_ sender: GlyphsPanelViewController, didTouchUpInsideKeyCap glyph: String)
}

class GlyphsPanelViewController: UIViewController {
  weak var delegate: GlyphsPanelViewControllerDelegate?

  @IBOutlet var container: ContainerView!
  
  private let glyphs: [(String, String)]
  private let buttons: [Button]
  private let buttonSize: CGSize
  private let gap: CGSize
  
  init(glyphs: [(String, String)], buttonSize: CGSize, gap: CGSize) {
    self.glyphs = glyphs
    self.buttonSize = buttonSize
    self.gap = gap
    var buttons: [Button] = []
    for (key, glyph) in glyphs {
      let button = Button(category: key, bottom: glyph)
      buttons.append(button)
    }
    self.buttons = buttons
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
    
    let tap = UITapGestureRecognizer(target: self, action: #selector(onTap(_:)))
    view.addGestureRecognizer(tap)
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
    super.viewDidLayoutSubviews()
  }
  
  @objc private func onTap(_ sender: AnyObject) {
    delegate?.glyphsPanelViewControllerWillDismiss(self)
    dismiss(animated: false)
  }
  
  @objc private func onTapKeyCap(_ sender: Button) {
    guard let glyph = sender.bottomText else {
      return
    }
    delegate?.glyphsPanelViewController(self, didTouchUpInsideKeyCap: glyph)
  }
}
