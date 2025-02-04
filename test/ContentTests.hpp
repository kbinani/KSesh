namespace ksesh::test {

using Offset32 = uint32_t;

class InputStream {
public:
  virtual ~InputStream() {}
  virtual bool ok() = 0;
  virtual uint32_t u32() = 0;
  virtual uint16_t u16() = 0;
  virtual uint8_t u8() = 0;

  Offset32 o32() {
    return u32();
  }
};

struct Tag {
  uint8_t values[4];

  static std::optional<Tag> Read(InputStream &in) {
    Tag t;
    for (int i = 0; i < 4; i++) {
      t.values[i] = in.u8();
    }
    if (in.ok()) {
      return t;
    } else {
      return std::nullopt;
    }
  }
};

struct TableRecord {
  Tag tag;
  uint32_t checksum;
  Offset32 offset;
  uint32_t length;

  static std::optional<TableRecord> Read(InputStream &in) {
    using namespace std;
    TableRecord tr;
    if (auto tag = Tag::Read(in); tag) {
      tr.tag = *tag;
    } else {
      return nullopt;
    }
    tr.checksum = in.u32();
    tr.offset = in.o32();
    tr.length = in.u32();
    if (in.ok()) {
      return tr;
    } else {
      return nullopt;
    }
  }
};

struct TableDirectory {
  uint32_t sfntVersion;
  uint16_t numTables;
  uint16_t searchRange;
  uint16_t entrySelector;
  uint16_t rangeShift;
  std::vector<TableRecord> tableRecords;

  static std::optional<TableDirectory> Read(InputStream &in) {
    using namespace std;
    TableDirectory td;
    td.sfntVersion = in.u32();
    td.numTables = in.u16();
    td.searchRange = in.u16();
    td.entrySelector = in.u16();
    td.rangeShift = in.u16();
    td.tableRecords.resize(td.numTables);
    for (uint32_t i = 0; i < td.numTables; i++) {
      if (auto record = TableRecord::Read(in); record) {
        td.tableRecords[i] = *record;
      } else {
        return nullopt;
      }
    }
    return td;
  }
};

class FontFile {
public:
  TableDirectory tableDirectory;

  static std::shared_ptr<FontFile> Read(InputStream &in) {
    using namespace std;
    auto ff = make_shared<FontFile>();
    if (auto td = TableDirectory::Read(in); td) {
      ff->tableDirectory = *td;
    } else {
      return nullptr;
    }
    return ff;
  }
};

class FileInputStream : public InputStream {
public:
  explicit FileInputStream(juce::File file) : s(file) {}

  uint32_t u32() override {
    if (!ok()) {
      return 0;
    }
    int32_t v = s.readIntBigEndian();
    return *(uint32_t *)&v;
  }

  uint16_t u16() override {
    if (!ok()) {
      return 0;
    }
    int16_t v = s.readShortBigEndian();
    return *(uint16_t *)&v;
  }

  uint8_t u8() override {
    if (!ok()) {
      return 0;
    }
    char v = s.readByte();
    return *(uint8_t *)&v;
  }

  bool ok() override {
    return s.openedOk() && !s.isExhausted();
  }

private:
  juce::FileInputStream s;
};

TEST_CASE("research") {
  FileInputStream fis(juce::File::getCurrentWorkingDirectory().getChildFile("egyptiantext-COLR.ttf"));
  auto ff = FontFile::Read(fis);
  CHECK(ff);
}

} // namespace ksesh::test
