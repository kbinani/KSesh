namespace ksesh::test {

TEST_CASE("research") {
  ksesh::otf::FileInputStream fis(juce::File::getCurrentWorkingDirectory().getChildFile("egyptiantext-COLR.ttf"));
  auto ff = ksesh::otf::FontFile::Read(fis);
  REQUIRE(ff);
  ksesh::otf::FileOutputStream fos(juce::File::getCurrentWorkingDirectory().getChildFile("egyptiantext-COLR-out.ttf"));
  CHECK(ff->write(fos));
}

} // namespace ksesh::test
