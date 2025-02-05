namespace ksesh::test {

using Offset32 = uint32_t;

class InputStream {
public:
  virtual ~InputStream() {}
  virtual bool ok() = 0;
  virtual uint32_t u32() = 0;
  virtual uint16_t u16() = 0;
  virtual uint8_t u8() = 0;
  virtual bool read(void *buffer, size_t size) = 0;
  virtual bool seek(int64_t loc) = 0;

  Offset32 o32() {
    return u32();
  }
};

class OutputStream {
public:
  virtual ~OutputStream() {}
  virtual bool u32(uint32_t v) = 0;
  virtual bool u16(uint16_t v) = 0;
  virtual bool u8(uint8_t v) = 0;
  virtual bool write(void *buffer, size_t size) = 0;
  virtual bool seek(int64_t loc) = 0;

  bool o32(Offset32 v) {
    return u32(v);
  }
};

struct Tag {
  std::array<uint8_t, 4> values;

  static std::optional<Tag> Read(InputStream &in) {
    Tag t;
    for (int i = 0; i < t.values.size(); i++) {
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

class Table {
public:
  virtual ~Table() {}
  virtual std::string encode() = 0;
  virtual uint32_t length() = 0;

  static std::optional<uint32_t> Checksum(std::string const &table) {
    if (table.size() % 4 != 0) {
      return std::nullopt;
    }
    uint32_t const *ptr = (uint32_t const *)table.data();
    uint32_t sum = 0;
    uint32_t const *const end = ptr + table.size() / 4;
    while (ptr < end) {
      sum += *ptr++;
    }
    return sum;
  }
};

class ReadonlyTable : public Table {
public:
  explicit ReadonlyTable(std::string const &content, uint32_t length) : content(content), len(length) {}

  std::string encode() override {
    return content;
  }

  uint32_t length() override {
    return len;
  }

public:
  std::string const content;
  uint32_t const len;
};

class FontFile {
public:
  TableDirectory tableDirectory;
  std::map<std::array<uint8_t, 4>, std::shared_ptr<Table>> tables;

  bool write(OutputStream &out) {
    using namespace std;
    if (!out.u32(tableDirectory.sfntVersion)) {
      return false;
    }
    if (!out.u16(tableDirectory.numTables)) {
      return false;
    }
    if (!out.u16(tableDirectory.searchRange)) {
      return false;
    }
    if (!out.u16(tableDirectory.entrySelector)) {
      return false;
    }
    if (!out.u16(tableDirectory.rangeShift)) {
      return false;
    }
    Offset32 const start = 12 + 16 * tableDirectory.numTables;
    Offset32 offset = start;
    vector<string> tableContents;
    for (TableRecord const &tr : tableDirectory.tableRecords) {
      if (!out.write((void *)tr.tag.values.data(), tr.tag.values.size())) {
        return false;
      }
      auto item = tables.find(tr.tag.values);
      if (item == tables.end()) {
        return false;
      }
      auto table = item->second;
      auto data = table->encode();
      auto length = table->length();
      tableContents.push_back(data);
      if (data.size() < length) {
        return false;
      }
      if (data.size() % 4 != 0) {
        return false;
      }
      auto checksum = Table::Checksum(data);
      if (!checksum) {
        return false;
      }
      if (!out.u32(*checksum)) {
        return false;
      }
      if (!out.o32(offset)) {
        return false;
      }
      offset += data.size();
      if (!out.u32(length)) {
        return false;
      }
    }
    if (!out.seek(start)) {
      return false;
    }
    for (auto const &data : tableContents) {
      if (!out.write((void *)data.c_str(), data.size())) {
        return false;
      }
    }
    return true;
  }

  static std::shared_ptr<FontFile> Read(InputStream &in) {
    using namespace std;
    auto ff = make_shared<FontFile>();
    if (auto td = TableDirectory::Read(in); td) {
      ff->tableDirectory = *td;
    } else {
      return nullptr;
    }
    for (uint32_t i = 0; i < ff->tableDirectory.numTables; i++) {
      TableRecord tr = ff->tableDirectory.tableRecords[i];
      uint32_t size = tr.length;
      if (size % 4 != 0) {
        size += 4 - (tr.length % 4);
      }
      string buffer;
      buffer.resize(size);
      if (!in.seek(tr.offset)) {
        return nullptr;
      }
      if (!in.read(buffer.data(), tr.length)) {
        return nullptr;
      }
      ff->tables[tr.tag.values] = make_shared<ReadonlyTable>(buffer, tr.length);
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

  bool read(void *buffer, size_t size) override {
    if (size == 0) {
      return true;
    }
    return s.read(buffer, size) == size;
  }

  bool seek(int64_t loc) override {
    return s.setPosition(loc);
  }

private:
  juce::FileInputStream s;
};

class FileOutputStream : public OutputStream {
public:
  explicit FileOutputStream(juce::File file) : s(file) {
    if (!s.setPosition(0)) {
      return;
    }
    s.truncate();
  }

  bool u32(uint32_t v) override {
    if (s.failedToOpen()) {
      return false;
    }
    return s.writeIntBigEndian(*(int32_t *)&v);
  }

  bool u16(uint16_t v) override {
    if (s.failedToOpen()) {
      return false;
    }
    return s.writeShortBigEndian(*(int16_t *)&v);
  }

  bool u8(uint8_t v) override {
    if (s.failedToOpen()) {
      return false;
    }
    return s.writeByte(*(char *)&v);
  }

  bool write(void *buffer, size_t size) override {
    if (s.failedToOpen()) {
      return false;
    }
    return s.write(buffer, size);
  }

  bool seek(int64_t loc) override {
    if (s.failedToOpen()) {
      return false;
    }
    return s.setPosition(loc);
  }

private:
  juce::FileOutputStream s;
};

TEST_CASE("research") {
  FileInputStream fis(juce::File::getCurrentWorkingDirectory().getChildFile("egyptiantext-COLR.ttf"));
  auto ff = FontFile::Read(fis);
  CHECK(ff);
  FileOutputStream fos(juce::File::getCurrentWorkingDirectory().getChildFile("egyptiantext-COLR-out.ttf"));
  CHECK(ff->write(fos));
}

} // namespace ksesh::test
