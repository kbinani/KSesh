import UIKit

protocol KeyCap: UIView {
  var appearance: UIKeyboardAppearance? { get set }
}

protocol ShiftableKeyCap: KeyCap {
  var shifted: Bool { get set }
}
