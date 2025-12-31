import CoreGraphics

struct BoundingBox {
  private var all: CGRect?
  
  mutating func add(_ rect: CGRect) {
    guard rect.width > 0, rect.height > 0 else {
      return
    }
    if let all {
      self.all = all.union(rect)
    } else {
      self.all = rect
    }
  }
  
  var rect: CGRect? {
    return all
  }
}
