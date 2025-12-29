import UIKit

class ContainerView: UIView {
  override func hitTest(_ point: CGPoint, with event: UIEvent?) -> UIView? {
    let hit = super.hitTest(point, with: event)
    if hit === self {
      return nil
    }
    return hit
  }
}
