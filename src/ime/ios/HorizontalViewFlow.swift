import UIKit

@MainActor
class HorizontalViewFlow {
  private var x: CGFloat
  private var y: CGFloat
  private let height: CGFloat
  private let gap: CGFloat
  
  init(origin: CGPoint, height: CGFloat, gap: CGFloat) {
    self.x = origin.x
    self.y = origin.y
    self.height = height
    self.gap = gap
  }
  
  func next(width: CGFloat, button: UIView) {
    let n = self.x + gap
    button.frame = .init(x: n, y: y, width: width, height: height)
    self.x = n + width
  }
  
  func reset(x: CGFloat, y: CGFloat) {
    self.x = x
    self.y = y
  }
}
