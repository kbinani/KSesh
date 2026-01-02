import UIKit

extension NSLayoutConstraint {
  static func identicalBoundingBox(master: UIView, slave: UIView) {
    slave.leadingAnchor.constraint(equalTo: master.leadingAnchor).isActive = true
    slave.trailingAnchor.constraint(equalTo: master.trailingAnchor).isActive = true
    slave.topAnchor.constraint(equalTo: master.topAnchor).isActive = true
    slave.bottomAnchor.constraint(equalTo: master.bottomAnchor).isActive = true
  }
}
