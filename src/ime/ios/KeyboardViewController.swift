import UIKit

class KeyboardViewController: UIInputViewController {
  // row 0
  private var toLeft: Button!
  private var toRight: Button!
  private var textView: TextView!
  private var backspace: Button!
  // row 1
  private var aleph: Button!
  private var a: Button!
  private var b: Button!
  private var c: Button!
  private var d: Button!
  private var e: Button!
  private var f: Button!
  private var g: Button!
  private var h: Button!
  private var i: Button!
  private var k: Button!
  private var ret: Button!
  // row 2
  private var shift: Button!
  private var l: Button!
  private var m: Button!
  private var n: Button!
  private var o: Button!
  private var p: Button!
  private var q: Button!
  private var r: Button!
  private var s: Button!
  private var t: Button!
  private var u: Button!
  private var v: Button!
  private var w: Button!
  // row 3
  private var globe: Button!
  private var x: Button!
  private var y: Button!
  private var z: Button!
  private var aa: Button!
  private var space: Button!
  private var vj: Button!
  private var hj: Button!
  private var beginSegment: Button!
  private var endSegment: Button!
  private var close: Button!
  private var container: ContainerView!
  
  private var allButtons: [Button] = []
  private var heightConstraint: NSLayoutConstraint!
  private var shifted: Bool = false {
    didSet {
      guard oldValue != shifted else {
        return
      }
      updateShiftState()
    }
  }
  
  override func updateViewConstraints() {
    super.updateViewConstraints()
    updateHeightConstraint()
  }
  
  override func viewDidLoad() {
    super.viewDidLoad()
    
    let proxy = self.textDocumentProxy
    let appearance = proxy.keyboardAppearance
    
    // row 0
    let toLeft = Button(keyCapRole: .default)
    toLeft.centerIcon = UIImageView(image: UIImage(systemName: "arrow.left"))
    toLeft.addTarget(self, action: #selector(toLeftKeyPressed(_:)), for: .touchUpInside)
    self.toLeft = toLeft
    allButtons.append(toLeft)
    let toRight = Button(keyCapRole: .default)
    toRight.centerIcon = UIImageView(image: UIImage(systemName: "arrow.right"))
    toRight.addTarget(self, action: #selector(toRightKeyPressed(_:)), for: .touchUpInside)
    self.toRight = toRight
    allButtons.append(toRight)
    let textView = TextView()
    textView.backgroundColor = .black.withAlphaComponent(0.1)
    textView.appearance = appearance
    self.textView = textView
    let backspace = Button(keyCapRole: .right)
    backspace.rightIcon = UIImageView(image: UIImage(systemName: "delete.left"))
    backspace.addTarget(self, action: #selector(backspaceKeyPressed(_:)), for: .touchUpInside)
    self.backspace = backspace
    allButtons.append(backspace)
    
    // row 1
    let aleph = Button(keyCapRole: .default)
    aleph.bottomText = "ȝ"
    self.aleph = aleph
    allButtons.append(aleph)
    let a = Button(category: "A", bottom: "𓀀")
    self.a = a
    allButtons.append(a)
    let b = Button(category: "B", bottom: "𓁐")
    self.b = b
    allButtons.append(b)
    let c = Button(category: "C", bottom: "𓁚")
    self.c = c
    allButtons.append(c)
    let d = Button(category: "D", bottom: "𓁶")
    self.d = d
    allButtons.append(d)
    let e = Button(category: "E", bottom: "𓃒")
    self.e = e
    allButtons.append(e)
    let f = Button(category: "F", bottom: "𓃾")
    self.f = f
    allButtons.append(f)
    let g = Button(category: "G", bottom: "𓄿")
    self.g = g
    allButtons.append(g)
    let h = Button(category: "H", bottom: "𓅿")
    self.h = h
    allButtons.append(h)
    let i = Button(category: "I", bottom: "𓆈")
    self.i = i
    allButtons.append(i)
    let k = Button(category: "K", bottom: "𓆛")
    self.k = k
    allButtons.append(k)
    let ret = Button(keyCapRole: .right)
    ret.rightIcon = UIImageView(image: UIImage(systemName: "return"))
    ret.addTarget(self, action: #selector(returnKeyPressed(_:)), for: .touchUpInside)
    self.ret = ret
    allButtons.append(ret)
    
    // row 2
    let shift = Button(keyCapRole: .default)
    shift.addTarget(self, action: #selector(shiftKeyPressed(_:)), for: .touchUpInside)
    self.shift = shift
    allButtons.append(shift)
    let l = Button(category: "L", bottom: "𓆣")
    self.l = l
    allButtons.append(l)
    let m = Button(category: "M", bottom: "𓆭")
    self.m = m
    allButtons.append(m)
    let n = Button(category: "N", bottom: "𓇯")
    self.n = n
    allButtons.append(n)
    let o = Button(category: "O", bottom: "𓉐")
    self.o = o
    allButtons.append(o)
    let p = Button(category: "P", bottom: "𓊛")
    self.p = p
    allButtons.append(p)
    let q = Button(category: "Q", bottom: "𓊨")
    self.q = q
    allButtons.append(q)
    let r = Button(category: "R", bottom: "𓊯")
    self.r = r
    allButtons.append(r)
    let s = Button(category: "S", bottom: "𓋑")
    self.s = s
    allButtons.append(s)
    let t = Button(category: "T", bottom: "𓌇")
    self.t = t
    allButtons.append(t)
    let u = Button(category: "U", bottom: "𓌳")
    self.u = u
    allButtons.append(u)
    let v = Button(category: "V", bottom: "𓍢")
    self.v = v
    allButtons.append(v)
    let w = Button(category: "W", bottom: "𓎯")
    self.w = w
    allButtons.append(w)
    
    // row 3
    let globe = Button(keyCapRole: .default)
    globe.leftIcon = UIImageView(image: UIImage(systemName: "globe"))
    self.globe = globe
    allButtons.append(globe)
    let x = Button(category: "X", bottom: "𓏏")
    self.x = x
    allButtons.append(x)
    let y = Button(category: "Y", bottom: "𓏛")
    self.y = y
    allButtons.append(y)
    let z = Button(category: "Z", bottom: "𓏤")
    self.z = z
    allButtons.append(z)
    let aa = Button(category: "Aa", bottom: "𓐍")
    self.aa = aa
    allButtons.append(aa)
    let space = Button(keyCapRole: .default)
    space.addTarget(self, action: #selector(spaceKeyPressed(_:)), for: .touchUpInside)
    self.space = space
    allButtons.append(space)
    let vj = Button(keyCapRole: .default)
    vj.centerIcon = UIImageView(image: .init(systemName: "square.split.1x2"))
    vj.addTarget(self, action: #selector(verticalJoinKeyPressed(_:)), for: .touchUpInside)
    self.vj = vj
    allButtons.append(vj)
    let hj = Button(keyCapRole: .default)
    hj.centerIcon = UIImageView(image: .init(systemName: "square.split.2x1"))
    hj.addTarget(self, action: #selector(horizontalJoinKeyPressed(_:)), for: .touchUpInside)
    self.hj = hj
    allButtons.append(hj)
    let beginSegment = Button(keyCapRole: .default)
    beginSegment.middleText = "("
    beginSegment.addTarget(self, action: #selector(beginSegmentKeyPressed(_:)), for: .touchUpInside)
    self.beginSegment = beginSegment
    allButtons.append(beginSegment)
    let endSegment = Button(keyCapRole: .default)
    endSegment.middleText = ")"
    endSegment.addTarget(self, action: #selector(endSegmentKeyPressed(_:)), for: .touchUpInside)
    self.endSegment = endSegment
    allButtons.append(endSegment)
    let close = Button(keyCapRole: .right)
    close.rightIcon = UIImageView(image: UIImage(systemName: "keyboard.chevron.compact.down"))
    self.close = close
    allButtons.append(close)
    
    let container = ContainerView(frame: .init(origin: .zero, size: .init(width: 1210, height: 315)))
    container.translatesAutoresizingMaskIntoConstraints = false
    container.isOpaque = false
    self.container = container
    
    container.addSubview(textView)
    allButtons.forEach { button in
      button.appearance = appearance
      container.addSubview(button)
    }
    allButtons.forEach { button in
      guard case .hieroglyph = button.keyCapRole else {
        return
      }
      button.addTarget(self, action: #selector(buttonPressed(_:)), for: .touchUpInside)
    }
    view.addSubview(container)
    NSLayoutConstraint.identicalBoundingBox(master: view, slave: container)
    
    let layout = Layout(width: view.bounds.width)
    let heightConstraint = view.heightAnchor.constraint(equalToConstant: layout.preferredHeight)
    heightConstraint.priority = .init(rawValue: UILayoutPriority.required.rawValue - 1)
    heightConstraint.isActive = true
    self.heightConstraint = heightConstraint
    
    globe.addTarget(self, action: #selector(handleInputModeList(from:with:)), for: .allTouchEvents)
    close.addTarget(self, action: #selector(dismissKeyboard), for: .touchUpInside)
    
    updateShiftState()
  }
  
  @objc private func shiftKeyPressed(_ button: Button) {
    shifted.toggle()
  }
  
  private func updateShiftState() {
    shift.leftIcon = UIImageView(image: Button.shiftIcon(shifted: shifted))
    allButtons.forEach { button in
      button.shifted = shifted
    }
  }
  
  @objc private func buttonPressed(_ button: Button) {
    guard case .hieroglyph(let category) = button.keyCapRole else {
      return
    }
    guard let window = self.view.window, let scene = window.windowScene else {
      return
    }
    heightConstraint.constant = scene.screen.bounds.height
    let glyphs = Hieroglyph.get(category: category)
    let layout = Layout(width: view.bounds.width)
    let v = GlyphsPanelViewController(
      glyphs: glyphs,
      buttonSize: button.bounds.size,
      gap: .init(width: layout.hGap, height: layout.vGap),
      shifted: shifted,
      parentShiftButton: shift
    )
    v.delegate = self
    v.modalPresentationStyle = .overFullScreen
    present(v, animated: false)
  }
  
  @objc private func verticalJoinKeyPressed(_ button: UIView) {
    textDocumentProxy.insertText(.verticalJoin)
    updateTextView()
  }
  
  @objc private func horizontalJoinKeyPressed(_ button: UIView) {
    textDocumentProxy.insertText(.horizontalJoin)
    updateTextView()
  }
  
  @objc private func beginSegmentKeyPressed(_ button: UIView) {
    textDocumentProxy.insertText(.beginSegment)
    updateTextView()
  }
  
  @objc private func endSegmentKeyPressed(_ button: UIView) {
    textDocumentProxy.insertText(.endSegment)
    updateTextView()
  }
  
  @objc private func backspaceKeyPressed(_ button: UIView) {
    textDocumentProxy.deleteBackward()
    updateTextView()
  }
  
  @objc private func toLeftKeyPressed(_ button: UIView) {
    guard let leading = textDocumentProxy.documentContextBeforeInput else {
      textDocumentProxy.adjustTextPosition(byCharacterOffset: -1)
      return
    }
    guard let last = leading.unicodeScalars.last else {
      textDocumentProxy.adjustTextPosition(byCharacterOffset: -1)
      return
    }
    textDocumentProxy.adjustTextPosition(byCharacterOffset: -last.utf16.count)
  }
  
  @objc private func toRightKeyPressed(_ button: UIView) {
    guard let trailing = textDocumentProxy.documentContextAfterInput else {
      textDocumentProxy.adjustTextPosition(byCharacterOffset: 1)
      return
    }
    guard let first = trailing.first else {
      textDocumentProxy.adjustTextPosition(byCharacterOffset: 1)
      return
    }
    textDocumentProxy.adjustTextPosition(byCharacterOffset: first.utf16.count)
  }
  
  @objc private func returnKeyPressed(_ button: UIView) {
    textDocumentProxy.insertText("\n")
    updateTextView()
  }
  
  @objc private func spaceKeyPressed(_ button: UIView) {
    textDocumentProxy.insertText(" ")
    updateTextView()
  }
  
  private func updateHeightConstraint() {
    guard let view = viewIfLoaded else {
      return
    }
    let layout = Layout(width: view.bounds.width)
    heightConstraint.constant = layout.preferredHeight
  }
  
  override func viewWillTransition(to size: CGSize, with coordinator: any UIViewControllerTransitionCoordinator) {
    super.viewWillTransition(to: size, with: coordinator)
    updateViewConstraints()
  }
  
  private var appearance: UIKeyboardAppearance? {
    didSet {
      guard appearance != oldValue else {
        return
      }
      allButtons.forEach { button in
        button.appearance = appearance
      }
      textView.appearance = appearance
    }
  }
  
  override func viewWillLayoutSubviews() {
    let j = Layout(width: view.bounds.width)
    let height = j.preferredHeight
    let top = view.bounds.height - height
    
    let flow = HorizontalViewFlow(origin: .init(x: 0, y: top + j.top(row: 0)), height: j.buttonHeight, gap: j.hGap)
    flow.next(width: j.toLeftButtonWidth, button: toLeft)
    flow.next(width: j.toRightButtonWidth, button: toRight)
    flow.next(width: j.textViewWidth, button: textView)
    flow.next(width: j.backspaceButtonWidth, button: backspace)
    
    flow.reset(x: 0, y: top + j.top(row: 1))
    flow.next(width: j.alephButtonWidth, button: aleph)
    flow.next(width: j.regularButtonWidth, button: a)
    flow.next(width: j.regularButtonWidth, button: b)
    flow.next(width: j.regularButtonWidth, button: c)
    flow.next(width: j.regularButtonWidth, button: d)
    flow.next(width: j.regularButtonWidth, button: e)
    flow.next(width: j.regularButtonWidth, button: f)
    flow.next(width: j.regularButtonWidth, button: g)
    flow.next(width: j.regularButtonWidth, button: h)
    flow.next(width: j.regularButtonWidth, button: i)
    flow.next(width: j.regularButtonWidth, button: k)
    flow.next(width: j.returnButtonWidth, button: ret)
    
    flow.reset(x: 0, y: top + j.top(row: 2))
    flow.next(width: j.shiftButtonWidth, button: shift)
    flow.next(width: j.regularButtonWidth, button: l)
    flow.next(width: j.regularButtonWidth, button: m)
    flow.next(width: j.regularButtonWidth, button: n)
    flow.next(width: j.regularButtonWidth, button: o)
    flow.next(width: j.regularButtonWidth, button: p)
    flow.next(width: j.regularButtonWidth, button: q)
    flow.next(width: j.regularButtonWidth, button: r)
    flow.next(width: j.regularButtonWidth, button: s)
    flow.next(width: j.regularButtonWidth, button: t)
    flow.next(width: j.regularButtonWidth, button: u)
    flow.next(width: j.regularButtonWidth, button: v)
    flow.next(width: j.regularButtonWidth, button: w)
    
    flow.reset(x: 0, y: top + j.top(row: 3))
    flow.next(width: j.globeButtonWidth, button: globe)
    flow.next(width: j.regularButtonWidth, button: x)
    flow.next(width: j.regularButtonWidth, button: y)
    flow.next(width: j.regularButtonWidth, button: z)
    flow.next(width: j.regularButtonWidth, button: aa)
    flow.next(width: j.spaceButtonWidth, button: space)
    flow.next(width: j.regularButtonWidth, button: vj)
    flow.next(width: j.regularButtonWidth, button: hj)
    flow.next(width: j.regularButtonWidth, button: beginSegment)
    flow.next(width: j.regularButtonWidth, button: endSegment)
    flow.next(width: j.closeButtonWidth, button: close)
    super.viewWillLayoutSubviews()
  }
  
  override func textDidChange(_ textInput: UITextInput?) {
    appearance = self.textDocumentProxy.keyboardAppearance
    updateTextView()
  }
  
  private func updateTextView() {
    textView.source = .init(textDocumentProxy: textDocumentProxy)
  }
}

extension KeyboardViewController: GlyphsPanelViewControllerDelegate {
  func glyphsPanelViewControllerWillDismiss(_ sender: GlyphsPanelViewController) {
    let layout = Layout(width: view.bounds.width)
    heightConstraint.constant = layout.preferredHeight
  }
  
  func glyphsPanelViewController(_ sender: GlyphsPanelViewController, didTouchUpInsideKeyCap text: String) {
    textDocumentProxy.insertText(text)
    updateTextView()
  }
  
  func glyphsPanelViewController(_ sender: GlyphsPanelViewController, didChangeShifted shifted: Bool) {
    self.shifted = shifted
  }
}
