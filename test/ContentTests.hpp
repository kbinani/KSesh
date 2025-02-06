namespace ksesh::test {

TEST_CASE("research") {
  FileInputStream fis(juce::File::getCurrentWorkingDirectory().getChildFile("egyptiantext-COLR-reordered.ttf"));
  auto ff = FontFile::Read(fis);
  CHECK(ff);
  FileOutputStream fos(juce::File::getCurrentWorkingDirectory().getChildFile("egyptiantext-COLR-out.ttf"));
  CHECK(ff->write(fos));
}

} // namespace ksesh::test
