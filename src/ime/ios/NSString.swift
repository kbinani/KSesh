import Foundation

extension NSString {
  func backspaceDeletingRange(at index: Int) -> Int {
    let leading = self.substring(to: index)
    guard let scalar = leading.unicodeScalars.last else {
      return 0
    }
    if scalar.isHieroglyph || scalar.isVariationSelector {
      return scalar.utf16.count
    }
    let composedRange = self.rangeOfComposedCharacterSequences(for: .init(location: index - 1, length: 1))
    return index - composedRange.location
  }
}
