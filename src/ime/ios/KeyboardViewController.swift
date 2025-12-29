import UIKit

class KeyboardViewController: UIInputViewController {
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
    
    init(width: CGFloat, needsInputModeSwitchKey: Bool) {
      let maxButtonWidth = (width - sideMargin * 2 - hGap * CGFloat(numMaxButtonsPerRow - 1)) / CGFloat(numMaxButtonsPerRow)
      self.regularButtonWidth = min(maxButtonWidth, 82)
      self.buttonHeight = regularButtonWidth * 74 / 82
      let returnOverAbc: CGFloat = CGFloat(278) / CGFloat(278 + 335)
      self.alephButtonWidth = (width - sideMargin * 2 - regularButtonWidth * 10 - hGap * 11) * returnOverAbc
      self.returnButtonWidth = width - sideMargin * 2 - regularButtonWidth * 10 - hGap * 11 - alephButtonWidth
      self.shiftButtonWidth = width - sideMargin * 2 - regularButtonWidth * 12 - hGap * 12
      self.closeButtonWidth = alephButtonWidth
      if needsInputModeSwitchKey {
        self.globeButtonWidth = regularButtonWidth
        self.spaceButtonWidth = width - sideMargin * 2 - globeButtonWidth - regularButtonWidth * 8 - closeButtonWidth - hGap * 10
      } else {
        self.globeButtonWidth = 0
        self.spaceButtonWidth = width - sideMargin * 2 - regularButtonWidth * 8 - closeButtonWidth - hGap * 9
      }
    }
    
    func top(row: Int) -> CGFloat {
      return topMargin + vGap * CGFloat(row) + buttonHeight * CGFloat(row)
    }
    
    var preferredHeight: CGFloat {
      return topMargin + buttonHeight * 4 + vGap * 3 + bottomMargin
    }
  }
  
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
  private var beginGroup: Button!
  private var endGroup: Button!
  private var close: Button!
  private var container: ContainerView!
  
  private var allButtons: [Button] = []
  private var heightConstraint: NSLayoutConstraint?
  
  override func updateViewConstraints() {
    super.updateViewConstraints()
    
    let layout = Layout(width: view.bounds.width, needsInputModeSwitchKey: needsInputModeSwitchKey)
    if let heightConstraint {
      heightConstraint.constant = layout.preferredHeight
    } else if let iv = container {
      let heightConstraint = NSLayoutConstraint(
        item: iv,
        attribute: .height,
        relatedBy: .equal,
        toItem: nil,
        attribute: .notAnAttribute,
        multiplier: 0,
        constant: layout.preferredHeight
      )
      heightConstraint.priority = .init(rawValue: 999)
      self.heightConstraint = heightConstraint
      iv.addConstraint(heightConstraint)
    }
  }
  
  override func viewDidLoad() {
    super.viewDidLoad()
    
    let proxy = self.textDocumentProxy
    let appearance = proxy.keyboardAppearance
    
    let aleph = Button(keyCapRole: .special)
    self.aleph = aleph
    allButtons.append(aleph)
    let a = Button(top: "A", bottom: "𓀀")
    self.a = a
    allButtons.append(a)
    let b = Button(top: "B", bottom: "𓁐")
    self.b = b
    allButtons.append(b)
    let c = Button(top: "C", bottom: "𓁚")
    self.c = c
    allButtons.append(c)
    let d = Button(top: "D", bottom: "𓁶")
    self.d = d
    allButtons.append(d)
    let e = Button(top: "E", bottom: "𓃒")
    self.e = e
    allButtons.append(e)
    let f = Button(top: "F", bottom: "𓃾")
    self.f = f
    allButtons.append(f)
    let g = Button(top: "G", bottom: "𓄿")
    self.g = g
    allButtons.append(g)
    let h = Button(top: "H", bottom: "𓅿")
    self.h = h
    allButtons.append(h)
    let i = Button(top: "I", bottom: "𓆈")
    self.i = i
    allButtons.append(i)
    let k = Button(top: "K", bottom: "𓆛")
    self.k = k
    allButtons.append(k)
    let ret = Button(keyCapRole: .special)
    ret.bottomText = "return"
    self.ret = ret
    allButtons.append(ret)
    
    let shift = Button(keyCapRole: .special)
    shift.bottomText = "shift"
    self.shift = shift
    allButtons.append(shift)
    let l = Button(top: "L", bottom: "𓆣")
    self.l = l
    allButtons.append(l)
    let m = Button(top: "M", bottom: "𓆭")
    self.m = m
    allButtons.append(m)
    let n = Button(top: "N", bottom: "𓇯")
    self.n = n
    allButtons.append(n)
    let o = Button(top: "O", bottom: "𓉐")
    self.o = o
    allButtons.append(o)
    let p = Button(top: "P", bottom: "𓊛")
    self.p = p
    allButtons.append(p)
    let q = Button(top: "Q", bottom: "𓊨")
    self.q = q
    allButtons.append(q)
    let r = Button(top: "R", bottom: "𓊯")
    self.r = r
    allButtons.append(r)
    let s = Button(top: "S", bottom: "𓋑")
    self.s = s
    allButtons.append(s)
    let t = Button(top: "T", bottom: "𓌇")
    self.t = t
    allButtons.append(t)
    let u = Button(top: "U", bottom: "𓌳")
    self.u = u
    allButtons.append(u)
    let v = Button(top: "V", bottom: "𓍢")
    self.v = v
    allButtons.append(v)
    let w = Button(top: "W", bottom: "𓎯")
    self.w = w
    allButtons.append(w)
    
    let globe = Button(keyCapRole: .special)
    self.globe = globe
    allButtons.append(globe)
    let x = Button(top: "X", bottom: "𓏏")
    self.x = x
    allButtons.append(x)
    let y = Button(top: "Y", bottom: "𓏛")
    self.y = y
    allButtons.append(y)
    let z = Button(top: "Z", bottom: "𓏤")
    self.z = z
    allButtons.append(z)
    let aa = Button(top: "Aa", bottom: "𓐍")
    self.aa = aa
    allButtons.append(aa)
    let space = Button()
    self.space = space
    allButtons.append(space)
    let vj = Button(middle: ":")
    self.vj = vj
    allButtons.append(vj)
    let hj = Button(middle: "*")
    self.hj = hj
    allButtons.append(hj)
    let beginGroup = Button(middle: "(")
    self.beginGroup = beginGroup
    allButtons.append(beginGroup)
    let endGroup = Button(middle: ")")
    self.endGroup = endGroup
    allButtons.append(endGroup)
    let close = Button(keyCapRole: .special)
    self.close = close
    allButtons.append(close)
    
    let container = ContainerView()
    container.translatesAutoresizingMaskIntoConstraints = false
    self.container = container
    allButtons.forEach { button in
      button.appearance = appearance
      container.addSubview(button)
    }
    view.addSubview(container)
    container.leadingAnchor.constraint(equalTo: view.leadingAnchor).isActive = true
    container.trailingAnchor.constraint(equalTo: view.trailingAnchor).isActive = true
    container.topAnchor.constraint(equalTo: view.topAnchor).isActive = true
    container.bottomAnchor.constraint(equalTo: view.bottomAnchor).isActive = true
    
    globe.addTarget(self, action: #selector(handleInputModeList(from:with:)), for: .allTouchEvents)
  }
  
  
  private var appearance: UIKeyboardAppearance? {
    didSet {
      guard appearance != oldValue else {
        return
      }
      allButtons.forEach { button in
        button.appearance = appearance
      }
    }
  }
  
  override func viewWillLayoutSubviews() {
    let j = Layout(width: view.bounds.width, needsInputModeSwitchKey: needsInputModeSwitchKey)
    @MainActor
    class WidthAdder {
      private var x: CGFloat
      private var y: CGFloat
      private let height: CGFloat
      private let gap: CGFloat
      
      init(origin: CGPoint, height: CGFloat, gap: CGFloat) {
        self.x = origin.x
        self.y = origin.y
        self.height = height
        self.gap = gap
      }
      
      func next(width: CGFloat, button: UIView) {
        let n = self.x + gap
        button.frame = .init(x: n, y: y, width: width, height: height)
        self.x = n + width
      }
      
      func reset(x: CGFloat, y: CGFloat) {
        self.x = x
        self.y = y
      }
    }
    let wa = WidthAdder(origin: .init(x: 0, y: j.top(row: 1)), height: j.buttonHeight, gap: j.hGap)
    wa.next(width: j.alephButtonWidth, button: aleph)
    wa.next(width: j.regularButtonWidth, button: a)
    wa.next(width: j.regularButtonWidth, button: b)
    wa.next(width: j.regularButtonWidth, button: c)
    wa.next(width: j.regularButtonWidth, button: d)
    wa.next(width: j.regularButtonWidth, button: e)
    wa.next(width: j.regularButtonWidth, button: f)
    wa.next(width: j.regularButtonWidth, button: g)
    wa.next(width: j.regularButtonWidth, button: h)
    wa.next(width: j.regularButtonWidth, button: i)
    wa.next(width: j.regularButtonWidth, button: k)
    wa.next(width: j.returnButtonWidth, button: ret)
    
    wa.reset(x: 0, y: j.top(row: 2))
    wa.next(width: j.shiftButtonWidth, button: shift)
    wa.next(width: j.regularButtonWidth, button: l)
    wa.next(width: j.regularButtonWidth, button: m)
    wa.next(width: j.regularButtonWidth, button: n)
    wa.next(width: j.regularButtonWidth, button: o)
    wa.next(width: j.regularButtonWidth, button: p)
    wa.next(width: j.regularButtonWidth, button: q)
    wa.next(width: j.regularButtonWidth, button: r)
    wa.next(width: j.regularButtonWidth, button: s)
    wa.next(width: j.regularButtonWidth, button: t)
    wa.next(width: j.regularButtonWidth, button: u)
    wa.next(width: j.regularButtonWidth, button: v)
    wa.next(width: j.regularButtonWidth, button: w)
    
    wa.reset(x: 0, y: j.top(row: 3))
    if needsInputModeSwitchKey {
      wa.next(width: j.globeButtonWidth, button: globe)
      globe.isHidden = false
    } else {
      globe.isHidden = true
    }
    wa.next(width: j.regularButtonWidth, button: x)
    wa.next(width: j.regularButtonWidth, button: y)
    wa.next(width: j.regularButtonWidth, button: z)
    wa.next(width: j.regularButtonWidth, button: aa)
    wa.next(width: j.spaceButtonWidth, button: space)
    wa.next(width: j.regularButtonWidth, button: vj)
    wa.next(width: j.regularButtonWidth, button: hj)
    wa.next(width: j.regularButtonWidth, button: beginGroup)
    wa.next(width: j.regularButtonWidth, button: endGroup)
    wa.next(width: j.closeButtonWidth, button: close)
    super.viewWillLayoutSubviews()
  }
  
  override func textWillChange(_ textInput: UITextInput?) {
    // The app is about to change the document's contents. Perform any preparation here.
  }
  
  override func textDidChange(_ textInput: UITextInput?) {
    // The app has just changed the document's contents, the document context has been updated.
    
    self.appearance = self.textDocumentProxy.keyboardAppearance
  }
}
