namespace ksesh::test {

TEST_CASE("F2DOT14") {
  using namespace std;
  using namespace ksesh::otf;
  vector<pair<int16_t, float>> data = {
      {9583, 0.584899902f},
      {-16383, -0.999938965f},
  };
  for (auto [i, f] : data) {
    otf::F2DOT14 a;
    a.data = i;
    CHECK(a.toFloat() == f);
    auto b = otf::F2DOT14::FromFloat(f);
    CHECK(b.data == a.data);
  }
}

TEST_CASE("research") {
  ksesh::otf::FileInputStream fis(juce::File::getCurrentWorkingDirectory().getChildFile("egyptiantext-COLR.ttf"));
  auto ff = ksesh::otf::FontFile::Read(fis);
  REQUIRE(ff);
  ksesh::otf::FileOutputStream fos(juce::File::getCurrentWorkingDirectory().getChildFile("egyptiantext-COLR-out.ttf"));
  CHECK(ff->write(fos));
}

} // namespace ksesh::test
