extension UnicodeScalar {
  var isHieroglyph: Bool {
    // Egyptian Hieroglyphs
    // Range: 13000–1342F
    if 0x13000 <= value && value <= 0x1342F {
      return true
    }
    // Egyptian Hieroglyphs Extended-A
    // Range: 13460–143FF
    if 0x13460 <= value && value <= 0x143FF {
      return true
    }
    // Egyptian Hieroglyph Format Controls
    // Range: 13430–1345F
    if 0x13430 <= value && value <= 0x1345F {
      return true
    }
    return false
  }
  
  var isVariationSelector: Bool {
    return 0xFE00 <= value && value <= 0xFE0F
  }
}
