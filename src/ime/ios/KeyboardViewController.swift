import UIKit

class KeyboardViewController: UIInputViewController {
  // row 0
  private var toLeft: CenterIconButton!
  private var toRight: CenterIconButton!
  private var textView: TextView!
  private var backspace: BackspaceButton!
  // row 1
  private var aleph: AlephButton!
  private var a: HieroglyphButton!
  private var b: HieroglyphButton!
  private var c: HieroglyphButton!
  private var d: HieroglyphButton!
  private var e: HieroglyphButton!
  private var f: HieroglyphButton!
  private var g: HieroglyphButton!
  private var h: HieroglyphButton!
  private var i: HieroglyphButton!
  private var k: HieroglyphButton!
  private var ret: RightIconButton!
  // row 2
  private var shift: ShiftButton!
  private var l: HieroglyphButton!
  private var m: HieroglyphButton!
  private var n: HieroglyphButton!
  private var o: HieroglyphButton!
  private var p: HieroglyphButton!
  private var q: HieroglyphButton!
  private var r: HieroglyphButton!
  private var s: HieroglyphButton!
  private var t: HieroglyphButton!
  private var u: HieroglyphButton!
  private var v: HieroglyphButton!
  private var w: HieroglyphButton!
  // row 3
  private var globe: LeftIconButton!
  private var x: HieroglyphButton!
  private var y: HieroglyphButton!
  private var z: HieroglyphButton!
  private var aa: HieroglyphButton!
  private var space: TextButton!
  private var vj: CenterIconButton!
  private var hj: CenterIconButton!
  private var beginSegment: TextButton!
  private var endSegment: TextButton!
  private var close: RightIconButton!
  private var container: ContainerView!
  
  private var allButtons: [KeyCap] = []
  private var shiftableButtons: [ShiftableKeyCap] = []
  private var hieroglyphButtons: [HieroglyphButton] = []
  private var heightConstraint: NSLayoutConstraint!
  private var shifted: Bool = false {
    didSet {
      guard oldValue != shifted else {
        return
      }
      updateShiftState()
    }
  }
  private var mode: HieroglyphButton.Mode = .hieroglyph {
    didSet {
      guard oldValue != mode else {
        return
      }
      updateMode()
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
    let toLeft = CenterIconButton(icon: .init(systemName: "arrow.left"))
    toLeft.addTarget(self, action: #selector(toLeftKeyPressed(_:)), for: .touchUpInside)
    self.toLeft = toLeft
    allButtons.append(toLeft)
    let toRight = CenterIconButton(icon: .init(systemName: "arrow.right"))
    toRight.addTarget(self, action: #selector(toRightKeyPressed(_:)), for: .touchUpInside)
    self.toRight = toRight
    allButtons.append(toRight)
    let textView = TextView()
    textView.backgroundColor = .black.withAlphaComponent(0.1)
    textView.appearance = appearance
    self.textView = textView
    let backspace = BackspaceButton()
    backspace.addTarget(self, action: #selector(backspaceKeyPressed(_:)), for: .touchUpInside)
    self.backspace = backspace
    allButtons.append(backspace)
    
    // row 1
    let aleph = AlephButton()
    aleph.addTarget(self, action: #selector(alephButtonPressed(_:)), for: .touchUpInside)
    self.aleph = aleph
    allButtons.append(aleph)
    let a = HieroglyphButton(
      category: "A",
      hieroglyph: "𓀀",
      transcription: .init(small: "ȝ", capital: nil, resource: "a1")
    )
    self.a = a
    allButtons.append(a)
    let b = HieroglyphButton(
      category: "B",
      hieroglyph: "𓁐",
      transcription: .init(small: "ı͗", capital: "I͗", resource: "i")
    )
    self.b = b
    allButtons.append(b)
    let c = HieroglyphButton(
      category: "C",
      hieroglyph: "𓁚",
      transcription: .init(small: "y", capital: nil, resource: "y")
    )
    self.c = c
    allButtons.append(c)
    let d = HieroglyphButton(
      category: "D",
      hieroglyph: "𓁶",
      transcription: .init(small: "ï", capital: nil, resource: "i1")
    )
    self.d = d
    allButtons.append(d)
    let e = HieroglyphButton(
      category: "E",
      hieroglyph: "𓃒",
      transcription: .init(small: "ʿ", capital: nil, resource: "a2")
    )
    self.e = e
    allButtons.append(e)
    let f = HieroglyphButton(
      category: "F",
      hieroglyph: "𓃾",
      transcription: .init(small: "w", capital: "W", resource: "w")
    )
    self.f = f
    allButtons.append(f)
    let g = HieroglyphButton(
      category: "G",
      hieroglyph: "𓄿",
      transcription: .init(small: "b", capital: "B", resource: "b")
    )
    self.g = g
    allButtons.append(g)
    let h = HieroglyphButton(
      category: "H",
      hieroglyph: "𓅿",
      transcription: .init(small: "p", capital: "P", resource: "p")
    )
    self.h = h
    allButtons.append(h)
    let i = HieroglyphButton(
      category: "I",
      hieroglyph: "𓆈",
      transcription: .init(small: "f", capital: "F", resource: "f")
    )
    self.i = i
    allButtons.append(i)
    let k = HieroglyphButton(
      category: "K",
      hieroglyph: "𓆛",
      transcription: .init(small: "m", capital: "M", resource: "m")
    )
    self.k = k
    allButtons.append(k)
    let ret = RightIconButton(icon: .init(systemName: "return"))
    ret.addTarget(self, action: #selector(returnKeyPressed(_:)), for: .touchUpInside)
    self.ret = ret
    allButtons.append(ret)
    
    // row 2
    let shift = ShiftButton()
    shift.addTarget(self, action: #selector(shiftKeyPressed(_:)), for: .touchUpInside)
    self.shift = shift
    allButtons.append(shift)
    let l = HieroglyphButton(
      category: "L",
      hieroglyph: "𓆣",
      transcription: .init(small: "n", capital: "N", resource: "n")
    )
    self.l = l
    allButtons.append(l)
    let m = HieroglyphButton(
      category: "M",
      hieroglyph: "𓆭",
      transcription: .init(small: "r", capital: "R", resource: "r")
    )
    self.m = m
    allButtons.append(m)
    let n = HieroglyphButton(
      category: "N",
      hieroglyph: "𓇯",
      transcription: .init(small: "h", capital: "H", resource: "h")
    )
    self.n = n
    allButtons.append(n)
    let o = HieroglyphButton(
      category: "O",
      hieroglyph: "𓉐",
      transcription: .init(small: "ḥ", capital: "Ḥ", resource: "h1")
    )
    self.o = o
    allButtons.append(o)
    let p = HieroglyphButton(
      category: "P",
      hieroglyph: "𓊛",
      transcription: .init(small: "ḫ", capital: "Ḫ", resource: "h2")
    )
    self.p = p
    allButtons.append(p)
    let q = HieroglyphButton(
      category: "Q",
      hieroglyph: "𓊨",
      transcription: .init(small: "ẖ", capital: "H̱", resource: "h3")
    )
    self.q = q
    allButtons.append(q)
    let r = HieroglyphButton(
      category: "R",
      hieroglyph: "𓊯",
      transcription: .init(small: "z", capital: "Z", resource: "z")
    )
    self.r = r
    allButtons.append(r)
    let s = HieroglyphButton(
      category: "S",
      hieroglyph: "𓋑",
      transcription: .init(small: "s", capital: "S", resource: "s")
    )
    self.s = s
    allButtons.append(s)
    let t = HieroglyphButton(
      category: "T",
      hieroglyph: "𓌇",
      transcription: .init(small: "š", capital: "Š", resource: "s1")
    )
    self.t = t
    allButtons.append(t)
    let u = HieroglyphButton(
      category: "U",
      hieroglyph: "𓌳",
      transcription: .init(small: "q", capital: "Q", resource: "q")
    )
    self.u = u
    allButtons.append(u)
    let v = HieroglyphButton(
      category: "V",
      hieroglyph: "𓍢",
      transcription: .init(small: "k", capital: "K", resource: "k")
    )
    self.v = v
    allButtons.append(v)
    let w = HieroglyphButton(
      category: "W",
      hieroglyph: "𓎯",
      transcription: .init(small: "g", capital: "G", resource: "g")
    )
    self.w = w
    allButtons.append(w)
    
    // row 3
    let globe = LeftIconButton(icon: .init(systemName: "globe"))
    self.globe = globe
    allButtons.append(globe)
    let x = HieroglyphButton(
      category: "X",
      hieroglyph: "𓏏",
      transcription: .init(small: "t", capital: "T", resource: "t")
    )
    self.x = x
    allButtons.append(x)
    let y = HieroglyphButton(
      category: "Y",
      hieroglyph: "𓏛",
      transcription: .init(small: "ṯ", capital: "Ṯ", resource: "t1")
    )
    self.y = y
    allButtons.append(y)
    let z = HieroglyphButton(
      category: "Z",
      hieroglyph: "𓏤",
      transcription: .init(small: "d", capital: "D", resource: "d")
    )
    self.z = z
    allButtons.append(z)
    let aa = HieroglyphButton(
      category: "Aa",
      hieroglyph: "𓐍",
      transcription: .init(small: "ḏ", capital: "Ḏ", resource: "d1")
    )
    self.aa = aa
    allButtons.append(aa)
    let space = TextButton(text: "")
    space.addTarget(self, action: #selector(spaceKeyPressed(_:)), for: .touchUpInside)
    self.space = space
    allButtons.append(space)
    let vj = CenterIconButton(icon: .init(systemName: "square.split.1x2"))
    vj.addTarget(self, action: #selector(verticalJoinKeyPressed(_:)), for: .touchUpInside)
    self.vj = vj
    allButtons.append(vj)
    let hj = CenterIconButton(icon: .init(systemName: "square.split.2x1"))
    hj.addTarget(self, action: #selector(horizontalJoinKeyPressed(_:)), for: .touchUpInside)
    self.hj = hj
    allButtons.append(hj)
    let beginSegment = TextButton(text: "(")
    beginSegment.addTarget(self, action: #selector(beginSegmentKeyPressed(_:)), for: .touchUpInside)
    self.beginSegment = beginSegment
    allButtons.append(beginSegment)
    let endSegment = TextButton(text: ")")
    endSegment.addTarget(self, action: #selector(endSegmentKeyPressed(_:)), for: .touchUpInside)
    self.endSegment = endSegment
    allButtons.append(endSegment)
    let close = RightIconButton(icon: .init(systemName: "keyboard.chevron.compact.down"))
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
    shiftableButtons.append(contentsOf: allButtons.compactMap { $0 as? ShiftableKeyCap })
    hieroglyphButtons.append(contentsOf: allButtons.compactMap { $0 as? HieroglyphButton })
    allButtons.forEach { button in
      guard let hieroglyph = button as? HieroglyphButton else {
        return
      }
      hieroglyph.addTarget(self, action: #selector(buttonPressed(_:)), for: .touchUpInside)
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
  
  @objc private func alephButtonPressed(_ button: AlephButton) {
    mode = mode.rotated
  }
  
  private func updateMode() {
    hieroglyphButtons.forEach { button in
      button.mode = mode
    }
    aleph.mode = mode
  }
  
  @objc private func shiftKeyPressed(_ button: HieroglyphButton) {
    shifted.toggle()
  }
  
  private func updateShiftState() {
    shift.shifted = shifted
    shiftableButtons.forEach { button in
      button.shifted = shifted
    }
  }
  
  @objc private func buttonPressed(_ button: HieroglyphButton) {
    guard let window = self.view.window, let scene = window.windowScene else {
      return
    }
    heightConstraint.constant = scene.screen.bounds.height
    let glyphs = Hieroglyph.get(category: button.category)
    let layout = Layout(width: view.bounds.width)
    let v = GlyphsPanelViewController(
      glyphs: glyphs,
      buttonSize: button.bounds.size,
      gap: .init(width: layout.hGap, height: layout.vGap),
      shifted: shifted,
      parentShiftButton: shift,
      appearance: textDocumentProxy.keyboardAppearance
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
    if let before = textDocumentProxy.documentContextBeforeInput, let last = before.unicodeScalars.last, let name = last.properties.name {
      backspace.topText = name
    } else {
      backspace.topText = ""
    }
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
