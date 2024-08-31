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

class Group : public Item {
public:
  std::deque<std::shared_ptr<Item>> fChildren;
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

static void ParseGroup(std::u32string const &s, size_t &offset, std::deque<std::shared_ptr<Item>> &items, char32_t terminator) {
  using namespace std;
  while (offset < s.size()) {
    auto start = Whitespace(s, offset);
    auto end = start;
    for (; end < s.size(); end++) {
      char32_t ch = s[end];
      if (ch == U' ') {
        end = Whitespace(s, end);
        if (start < end) {
          auto text = s.substr(start, end - start);
          items.push_back(make_shared<Simple>(text));
        }
        offset = end;
        break;
      } else if (ch == U'(') {
        if (start < end) {
          auto text = s.substr(start, end - start);
          items.push_back(make_shared<Simple>(text));
        }
        offset = end + 1;
        auto group = make_shared<Group>();
        ParseGroup(s, offset, group->fChildren, U')');
        items.push_back(group);
        break;
      } else if (ch == U':' || ch == U'<' || ch == U'*') {
        // TODO:
        break;
      } else if (ch == terminator) {
        if (start < end) {
          auto text = s.substr(start, end - start);
          items.push_back(make_shared<Simple>(text));
        }
        offset = end + 1;
        return;
      }
    }
  }
}

static std::shared_ptr<Document> Parse(std::u32string const &s) {
  using namespace std;
  auto document = make_shared<Document>();
  size_t offset = 0;
  while (offset < s.size()) {
    auto line = make_shared<Line>();
    ParseGroup(s, offset, line->fItems, U'\n');
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
  SUBCASE("group") {
    auto d = Parse(U"A1(A2 (B1)C1)");
    REQUIRE(d);
    CHECK(d->fLines.size() == 1);
    auto l = d->fLines[0];
    CHECK(l->fItems.size() == 2);
    auto s = dynamic_pointer_cast<Simple>(l->fItems[0]);
    auto g = dynamic_pointer_cast<Group>(l->fItems[1]);
    REQUIRE(s);
    CHECK(s->fText == U"A1");
    REQUIRE(g);
    CHECK(g->fChildren.size() == 3);
    auto gc0 = dynamic_pointer_cast<Simple>(g->fChildren[0]);
    auto gc1 = dynamic_pointer_cast<Group>(g->fChildren[1]);
    auto gc2 = dynamic_pointer_cast<Simple>(g->fChildren[2]);
    REQUIRE(gc0);
    CHECK(gc0->fText == U"A2 ");
    REQUIRE(gc1);
    CHECK(gc1->fChildren.size() == 1);
    REQUIRE(gc2);
    CHECK(gc2->fText == U"C1");
    auto gc1c0 = dynamic_pointer_cast<Simple>(gc1->fChildren[0]);
    REQUIRE(gc1c0);
    CHECK(gc1c0->fText == U"B1");
  }
}

} // namespace ksesh::test
