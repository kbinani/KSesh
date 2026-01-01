import UIKit

@MainActor
protocol GlyphsPanelViewControllerDelegate: AnyObject {
  func glyphsPanelViewControllerWillDismiss(_ sender: GlyphsPanelViewController)
}

class GlyphsPanelViewController: UIViewController {
  weak var delegate: GlyphsPanelViewControllerDelegate?
  
  class Cell: UICollectionViewCell {
    var key: String?
    var glyph: String?
    private var button: Button?
    
    override func prepareForReuse() {
      self.key = nil
      self.glyph = nil
      button?.topText = ""
      button?.bottomText = ""
    }
    
    override func apply(_ layoutAttributes: UICollectionViewLayoutAttributes) {
      button?.frame = .init(origin: .zero, size: layoutAttributes.size)
    }
    
    func setup(key: String, glyph: String) {
      self.key = key
      self.glyph = glyph
      if let button  {
        button.topText = key
        button.bottomText = glyph
      } else {
        let b = Button(category: key, bottom: glyph)
        b.frame = .init(origin: .zero, size: contentView.bounds.size)
        self.button = b
        contentView.addSubview(b)
      }
    }
  }
  
  @IBOutlet var collectionView: UICollectionView!
  @IBOutlet var layout: UICollectionViewFlowLayout!
  
  private let glyphs: [(String, String)]
  private let buttonSize: CGSize
  private let gap: CGSize
  
  init(glyphs: [(String, String)], buttonSize: CGSize, gap: CGSize) {
    self.glyphs = glyphs
    self.buttonSize = buttonSize
    self.gap = gap
    super.init(nibName: "GlyphsPanelViewController", bundle: nil)
  }
  
  required init?(coder: NSCoder) {
    fatalError("init(coder:) has not been implemented")
  }
  
  override func viewDidLoad() {
    super.viewDidLoad()
    
    layout.itemSize = buttonSize
    layout.minimumInteritemSpacing = gap.width
    layout.minimumLineSpacing = gap.height
    collectionView.register(Cell.self, forCellWithReuseIdentifier: "cell")
    collectionView.dataSource = self
    
    let tap = UITapGestureRecognizer(target: self, action: #selector(onTap(_:)))
    view.addGestureRecognizer(tap)
  }
  
  @objc private func onTap(_ sender: AnyObject) {
    delegate?.glyphsPanelViewControllerWillDismiss(self)
    dismiss(animated: true)
  }
}

extension GlyphsPanelViewController: UICollectionViewDataSource {
  func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
    guard section == 0 else {
      return 0
    }
    return glyphs.count
  }
  
  func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
    let index = indexPath.row
    let (key, glyph) = glyphs[index]
    let cell = collectionView.dequeueReusableCell(withReuseIdentifier: "cell", for: indexPath)
    if let typed = cell as? Cell {
      typed.setup(key: key, glyph: glyph)
    }
    return cell
  }
}
