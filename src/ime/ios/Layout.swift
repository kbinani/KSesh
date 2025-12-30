import CoreGraphics

struct Layout {
  let sideMargin: CGFloat = 15
  let topMargin: CGFloat = 11
  let bottomMargin: CGFloat = 11
  let numMaxButtonsPerRow: Int = 14
  let hGap: CGFloat
  let vGap: CGFloat
  
  let regularButtonWidth: CGFloat
  let buttonHeight: CGFloat
  let alephButtonWidth: CGFloat
  let returnButtonWidth: CGFloat
  let shiftButtonWidth: CGFloat
  let globeButtonWidth: CGFloat
  let spaceButtonWidth: CGFloat
  let closeButtonWidth: CGFloat
  
  init(width: CGFloat) {
    let baseHGap: CGFloat = 14
    let baseVGap: CGFloat = 12
    let baseButtonWidth: CGFloat = 82
    let baseButtonHeight: CGFloat = 74
    let baseABCButtonWidth: CGFloat = 278
    let baseReturnButtonWidth: CGFloat = 335
    
    self.regularButtonWidth = (width - sideMargin * 2) / (CGFloat(numMaxButtonsPerRow) + (baseHGap / baseButtonWidth) * CGFloat(numMaxButtonsPerRow - 1))
    self.hGap = self.regularButtonWidth * baseHGap / baseButtonWidth
    self.vGap = self.hGap * baseVGap / baseHGap
    self.buttonHeight = regularButtonWidth * baseButtonHeight / baseButtonWidth
    let returnOverAbc: CGFloat = baseABCButtonWidth / (baseABCButtonWidth + baseReturnButtonWidth)
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
