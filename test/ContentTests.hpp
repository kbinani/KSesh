#include <deque>

namespace ksesh::test {

class Item {
public:
  virtual ~Item(){};
};

class Simple : public Item {
public:
  explicit Simple(std::u32string const &s) : fText(s) {}

  std::u32string const fText;
};

class HBox : public Item {
public:
};

class VBox : public Item {
public:
};

class Line {
public:
  std::deque<std::shared_ptr<Item>> fItems;
};

class Document {
public:
  std::deque<std::shared_ptr<Line>> fLines;
};

static size_t Whitespace(std::u32string const &str, size_t offset) {
  size_t i = offset;
  while (i < str.size() && str[i] == U' ') {
    i++;
  }
  return i;
}

static void Group(std::u32string const &s, size_t &offset, std::deque<std::shared_ptr<Item>> &items, char32_t terminator) {
  using namespace std;
  while (offset < s.size()) {
    auto start = Whitespace(s, offset);
    auto end = start;
    bool rtn = false;
    for (; end < s.size(); end++) {
      char32_t ch = s[end];
      bool brk = false;
      if (ch == U' ') {
        end = Whitespace(s, end);
        break;
      } else if (ch == U':' || ch == U'(' || ch == U'<' || ch == U'*') {
        // TODO:
        break;
      } else if (ch == terminator) {
        rtn = true;
        break;
      }
    }
    auto text = s.substr(start, end - start);
    items.push_back(make_shared<Simple>(text));
    offset = end;
    if (rtn) {
      return;
    }
  }
}

static std::shared_ptr<Document> Parse(std::u32string const &s) {
  using namespace std;
  auto document = make_shared<Document>();
  size_t offset = 0;
  while (offset < s.size()) {
    auto line = make_shared<Line>();
    Group(s, offset, line->fItems, U'\n');
    offset++;
    document->fLines.push_back(line);
  }
  return document;
}

TEST_CASE("Parse") {
  SUBCASE("simple") {
    auto d = Parse(U"A1  B1\nC1");
    REQUIRE(d);
    CHECK(d->fLines.size() == 2);
    auto l0 = d->fLines[0];
    CHECK(l0->fItems.size() == 2);
    auto l1 = d->fLines[1];
    CHECK(l1->fItems.size() == 1);
  }
}

} // namespace ksesh::test
