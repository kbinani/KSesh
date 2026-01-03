import UIKit

extension UIKeyboardAppearance {
  var baseBackgroundColor: UIColor {
    if case .dark = self {
      return #colorLiteral(red: 0.2353515327, green: 0.2353515029, blue: 0.2353515029, alpha: 1)
    } else {
      return .white
    }
  }
  
  var tintColor: UIColor {
    if case .dark = self {
      return .white
    } else {
      return .black
    }
  }
  
  var secondaryTintColor: UIColor {
    if case .dark = self {
      return #colorLiteral(red: 0.4665528536, green: 0.4665527344, blue: 0.4665527344, alpha: 1)
    } else {
      return #colorLiteral(red: 0.7524755597, green: 0.7555301785, blue: 0.7636918426, alpha: 1)
    }
  }
}
