namespace ksesh::test {

using Offset32 = uint32_t;

struct Fixed {
  uint32_t value;
};

using LONGDATETIME = int64_t;

class InputStream {
public:
  virtual ~InputStream() {}
  virtual bool ok() = 0;
  virtual int64_t i64() = 0;
  virtual uint32_t u32() = 0;
  virtual int16_t i16() = 0;
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
  virtual bool i64(int64_t v) = 0;
  virtual bool u32(uint32_t v) = 0;
  virtual bool i16(int16_t) = 0;
  virtual bool u16(uint16_t v) = 0;
  virtual bool u8(uint8_t v) = 0;
  virtual bool write(void *buffer, size_t size) = 0;
  virtual bool seek(int64_t loc) = 0;

  bool o32(Offset32 v) {
    return u32(v);
  }
};

class ByteInputStream : public InputStream {
public:
  explicit ByteInputStream(std::string const &buffer) : buffer(buffer) {
  }

  int64_t i64() override {
    if (pos + 8 <= buffer.size()) {
      uint64_t v = juce::ByteOrder::bigEndianInt64(buffer.c_str() + pos);
      pos += 8;
      return *(int64_t*)&v;
    } else {
      pos = buffer.size();
      return 0;
    }
  }

  uint32_t u32() override {
    if (pos + 4 <= buffer.size()) {
      uint32_t v = juce::ByteOrder::bigEndianInt(buffer.c_str() + pos);
      pos += 4;
      return v;
    } else {
      pos = buffer.size();
      return 0;
    }
  }

  int16_t i16() override {
    if (pos + 2 <= buffer.size()) {
      uint16_t v = juce::ByteOrder::bigEndianShort(buffer.c_str() + pos);
      pos += 2;
      return *(int16_t*)&v;
    } else {
      pos = buffer.size();
      return 0;
    }
  }

  uint16_t u16() override {
    if (pos + 2 <= buffer.size()) {
      uint16_t v = juce::ByteOrder::bigEndianShort(buffer.c_str() + pos);
      pos += 2;
      return v;
    } else {
      pos = buffer.size();
      return 0;
    }
  }

  uint8_t u8() override {
    if (pos + 1 <= buffer.size()) {
      char v = buffer[pos];
      pos += 1;
      return *(uint8_t *)&v;
    } else {
      pos = buffer.size();
      return 0;
    }
  }

  bool read(void* buf, size_t size) override {
    if (pos + size <= buffer.size()) {
      pos += size;
      std::copy_n(buffer.c_str() + pos, size, (char *)buf);
      return true;
    } else {
      pos = buffer.size();
      return false;
    }
  }

  bool seek(int64_t loc) override {
    if (0 <= pos + loc && pos + loc <= buffer.size()) {
      pos = loc;
      return true;
    } else {
      return false;
    }
  }

  bool ok() override {
    return pos < buffer.size();
  }

private:
  size_t pos = 0;
  std::string buffer;
};

class ByteOutputStream : public OutputStream {
public:
  bool i64(int64_t v) override {
    uint64_t t = juce::ByteOrder::bigEndianInt64(&v);
    if (loc + 8 > buffer.size()) {
      buffer.resize(loc + 8);
    }
    std::copy_n((char const *)&t, 8, buffer.data() + loc);
    loc += 8;
    return true;
  }

  bool u32(uint32_t v) override {
    uint32_t t = juce::ByteOrder::bigEndianInt(&v);
    if (loc + 4 > buffer.size()) {
      buffer.resize(loc + 4);
    }
    std::copy_n((char const *)&t, 4, buffer.data() + loc);
    loc += 4;
    return true;
  }

  bool i16(int16_t v) override {
    uint16_t t = juce::ByteOrder::bigEndianShort(&v);
    if (loc + 2 > buffer.size()) {
      buffer.resize(loc + 2);
    }
    std::copy_n((char const *)&t, 2, buffer.data() + loc);
    loc += 2;
    return true;
  }

  bool u16(uint16_t v) override {
    uint16_t t = juce::ByteOrder::bigEndianShort(&v);
    if (loc + 2 > buffer.size()) {
      buffer.resize(loc + 2);
    }
    std::copy_n((char const *)&t, 2, buffer.data() + loc);
    loc += 2;
    return true;
  }

  bool u8(uint8_t v) override {
    if (loc + 1 > buffer.size()) {
      buffer.resize(loc + 1);
    }
    buffer[loc] = *(char*)&v;
    loc += 1;
    return true;
  }

  bool write(void* buf, size_t size) override {
    if (loc + size > buffer.size()) {
      buffer.resize(loc + size);
    }
    std::copy_n((char const *)buf, size, buffer.data() + loc);
    loc += size;
    return true;
  }

  bool seek(int64_t l) override {
    if (l >= 0) {
      loc = l;
      return true;
    } else {
      return false;
    }
  }

  std::string data() {
    std::string s;
    s.assign((char const*)buffer.data(), buffer.size());
    return s;
  }

private:
  std::vector<uint8_t> buffer;
  size_t loc = 0;
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

  static constexpr std::array<uint8_t, 4> FCC(char a, char b, char c, char d) {
    std::array<uint8_t, 4> v;
    v[0] = *(uint8_t *)&a;
    v[1] = *(uint8_t *)&b;
    v[2] = *(uint8_t *)&c;
    v[3] = *(uint8_t *)&d;
    return v;
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
  struct EncodeResult {
    std::string data;
    uint32_t length;
  };
  virtual std::optional<EncodeResult> encode() = 0;

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

  std::optional<EncodeResult> encode() override {
    EncodeResult er;
    er.data = content;
    er.length = len;
    return er;
  }

public:
  std::string const content;
  uint32_t const len;
};

// 'head'
class FontHeaderTable : public Table {
public:
  static std::shared_ptr<FontHeaderTable> Read(InputStream &in) {
    using namespace std;
    auto r = make_shared<FontHeaderTable>();
    r->majorVersion = in.u16();
    r->minorVersion = in.u16();
    r->fontRevision.value = in.u32();
    r->checksumAdjustment = in.u32();
    r->magicNumber = in.u32();
    r->flags = in.u16();
    r->unitsPerEm = in.u16();
    r->created = in.i64();
    r->modified = in.i64();
    r->xMin = in.i16();
    r->yMin = in.i16();
    r->xMax = in.i16();
    r->yMax = in.i16();
    r->macStyle = in.u16();
    r->lowestRecPPEM = in.u16();
    r->fontDirectionHint = in.i16();
    r->indexToLocFormat = in.i16();
    r->glyphDataFormat = in.i16();
    if (in.ok()) {
      return r;
    } else {
      return nullptr;
    }
  }

  std::optional<EncodeResult> encode() override {
    using namespace std;
    ByteOutputStream out;
    if (!out.u16(majorVersion)) {
      return nullopt;
    }
    if (!out.u16(minorVersion)) {
      return nullopt;
    }
    if (!out.u32(fontRevision.value)) {
      return nullopt;
    }
    if (!out.u32(checksumAdjustment)) {
      return nullopt;
    }
    if (!out.u32(magicNumber)) {
      return nullopt;
    }
    if (!out.u16(flags)) {
      return nullopt;
    }
    if (!out.u16(unitsPerEm)) {
      return nullopt;
    }
    if (!out.i64(created)) {
      return nullopt;
    }
    if (!out.i64(modified)) {
      return nullopt;
    }
    if (!out.i16(xMin)) {
      return nullopt;
    }
    if (!out.i16(yMin)) {
      return nullopt;
    }
    if (!out.i16(xMax)) {
      return nullopt;
    }
    if (!out.i16(yMax)) {
      return nullopt;
    }
    if (!out.u16(macStyle)) {
      return nullopt;
    }
    if (!out.u16(lowestRecPPEM)) {
      return nullopt;
    }
    if (!out.i16(fontDirectionHint)) {
      return nullopt;
    }
    if (!out.i16(indexToLocFormat)) {
      return nullopt;
    }
    if (!out.i16(glyphDataFormat)) {
      return nullopt;
    }
    auto data = out.data();
    EncodeResult er;
    er.length = data.size();
    if (er.length % 4 != 0) {
      data.resize(er.length + (4 - (er.length % 4)));
    }
    er.data = data;
    return er;
  }

public:
  uint16_t majorVersion;
  uint16_t minorVersion;
  Fixed fontRevision;
  uint32_t checksumAdjustment;
  uint32_t magicNumber;
  uint16_t flags;
  uint16_t unitsPerEm;
  LONGDATETIME created;
  LONGDATETIME modified;
  int16_t xMin;
  int16_t yMin;
  int16_t xMax;
  int16_t yMax;
  uint16_t macStyle;
  uint16_t lowestRecPPEM;
  int16_t fontDirectionHint;
  int16_t indexToLocFormat;
  int16_t glyphDataFormat;
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
      auto encoded = table->encode();
      if (!encoded) {
        return false;
      }
      uint32_t length = encoded->length;
      tableContents.push_back(encoded->data);
      if (encoded->data.size() < length) {
        return false;
      }
      if (encoded->data.size() % 4 != 0) {
        return false;
      }
      auto checksum = Table::Checksum(encoded->data);
      if (!checksum) {
        return false;
      }
      if (!out.u32(*checksum)) {
        return false;
      }
      if (!out.o32(offset)) {
        return false;
      }
      offset += encoded->data.size();
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
      if (tr.tag.values == Tag::FCC('h', 'e', 'a', 'd')) {
        ByteInputStream slice(buffer);
        auto head = FontHeaderTable::Read(slice);
        if (!head) {
          return nullptr;
        }
        ff->tables[tr.tag.values] = head;
      } else {
        ff->tables[tr.tag.values] = make_shared<ReadonlyTable>(buffer, tr.length);
      }
    }
    return ff;
  }
};

class FileInputStream : public InputStream {
public:
  explicit FileInputStream(juce::File file) : s(file) {}

  int64_t i64() override {
    if (!ok()) {
      return 0;
    }
    return s.readInt64BigEndian();
  }

  uint32_t u32() override {
    if (!ok()) {
      return 0;
    }
    int32_t v = s.readIntBigEndian();
    return *(uint32_t *)&v;
  }

  int16_t i16() override {
    if (!ok()) {
      return 0;
    }
    return s.readShortBigEndian();
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

  bool i64(int64_t v) override {
    if (s.failedToOpen()) {
      return false;
    }
    return s.writeInt64BigEndian(v);
  }

  bool u32(uint32_t v) override {
    if (s.failedToOpen()) {
      return false;
    }
    return s.writeIntBigEndian(*(int32_t *)&v);
  }

  bool i16(int16_t v) override {
    if (s.failedToOpen()) {
      return false;
    }
    return s.writeShortBigEndian(v);
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
  FileInputStream fis(juce::File::getCurrentWorkingDirectory().getChildFile("egyptiantext-COLR-reordered.ttf"));
  auto ff = FontFile::Read(fis);
  CHECK(ff);
  FileOutputStream fos(juce::File::getCurrentWorkingDirectory().getChildFile("egyptiantext-COLR-out.ttf"));
  CHECK(ff->write(fos));
}

} // namespace ksesh::test
