import UIKit

extension UIFont {
  func opticalBounds(_ glyph: CGGlyph) -> CGRect? {
    return CTFontCreatePathForGlyph(self as CTFont, glyph, nil)?.boundingBoxOfPath
  }
}
