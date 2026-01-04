struct Transcription {
  let small: String
  let capital: String?
  let resource: String
  
  func resourceName(shifted: Bool) -> String {
    if shifted {
      return resource + "_capital"
    } else {
      return resource + "_small"
    }
  }
}
