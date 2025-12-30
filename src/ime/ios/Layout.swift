import CoreGraphics

struct Layout {
  let sideMargin: CGFloat = 15
  let topMargin: CGFloat = 11
  let bottomMargin: CGFloat = 11
  let numMaxButtonsPerRow: Int = 14
  let hGap: CGFloat = 14
  let vGap: CGFloat = 12
  
  let regularButtonWidth: CGFloat
  let buttonHeight: CGFloat
  let alephButtonWidth: CGFloat
  let returnButtonWidth: CGFloat
  let shiftButtonWidth: CGFloat
  let globeButtonWidth: CGFloat
  let spaceButtonWidth: CGFloat
  let closeButtonWidth: CGFloat
  
  init(width: CGFloat) {
    let maxButtonWidth = (width - sideMargin * 2 - hGap * CGFloat(numMaxButtonsPerRow - 1)) / CGFloat(numMaxButtonsPerRow)
    self.regularButtonWidth = min(maxButtonWidth, 82)
    self.buttonHeight = regularButtonWidth * 74 / 82
    let returnOverAbc: CGFloat = CGFloat(278) / CGFloat(278 + 335)
    self.alephButtonWidth = (width - sideMargin * 2 - regularButtonWidth * 10 - hGap * 11) * returnOverAbc
    self.returnButtonWidth = width - sideMargin * 2 - regularButtonWidth * 10 - hGap * 11 - alephButtonWidth
    self.shiftButtonWidth = width - sideMargin * 2 - regularButtonWidth * 12 - hGap * 12
    self.closeButtonWidth = alephButtonWidth
    self.globeButtonWidth = regularButtonWidth
    self.spaceButtonWidth = width - sideMargin * 2 - globeButtonWidth - regularButtonWidth * 8 - closeButtonWidth - hGap * 10
  }
  
  func top(row: Int) -> CGFloat {
    return topMargin + vGap * CGFloat(row) + buttonHeight * CGFloat(row)
  }
  
  var preferredHeight: CGFloat {
    return ceil(topMargin + buttonHeight * 4 + vGap * 3 + bottomMargin)
  }
}
