import UIKit

struct TextViewSource: Equatable {
  let leading: String
  let trailing: String
  
  init(leading: String, trailing: String) {
    if let last = leading.lastIndex(of: "\n") {
      let from = leading.index(after: last)
      self.leading = String(leading[from...])
    } else {
      self.leading = leading
    }
    if let first = trailing.firstIndex(of: "\n") {
      self.trailing = String(trailing[..<first])
    } else {
      self.trailing = trailing
    }
  }
  
  @MainActor
  init(textDocumentProxy: UITextDocumentProxy) {
    self.init(
      leading: textDocumentProxy.documentContextBeforeInput ?? "",
      trailing: textDocumentProxy.documentContextAfterInput ?? ""
    )
  }
}
