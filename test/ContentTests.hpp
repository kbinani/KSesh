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

class Cartouche : public Item {
public:
  std::u32string fOpen;
  std::deque<std::shared_ptr<Item>> fChildren;
  std::u32string fClose;
};

class HBox : public Item {
public:
  std::deque<std::shared_ptr<Item>> fChildren;
};

class Line {
public:
  std::deque<std::shared_ptr<Item>> fItems;
};

class Document {
public:
  std::deque<std::shared_ptr<Line>> fLines;

  static size_t Whitespace(std::u32string const &str, size_t offset) {
    static std::set<char32_t> const sChars = {U' '};
    return Skip(str, offset, sChars);
  }

  static size_t Skip(std::u32string const &str, size_t offset, std::set<char32_t> const &characters) {
    size_t i = offset;
    while (i < str.size() && characters.find(str[i]) != characters.end()) {
      i++;
    }
    return i;
  }

  static void ParseGroup(std::u32string const &s, size_t &offset, std::deque<std::shared_ptr<Item>> &items, char32_t terminator) {
    using namespace std;
    char32_t glue = 0;
    auto append = [&](shared_ptr<Item> item) {
      if (items.empty()) {
        items.push_back(item);
        return;
      }
      auto back = items.back();
      if (glue == U'*') {
        glue = 0;
        if (auto hbox = dynamic_pointer_cast<HBox>(back)) {
          hbox->fChildren.push_back(item);
          return;
        }
      }
      items.push_back(item);
    };
    while (offset < s.size()) {
      auto start = Whitespace(s, offset);
      auto end = start;
      for (; end < s.size(); end++) {
        char32_t ch = s[end];
        if (ch == U' ') {
          end = Whitespace(s, end);
          if (start < end) {
            auto text = s.substr(start, end - start);
            append(make_shared<Simple>(text));
            start = end;
          }
          offset = end;
          break;
        } else if (ch == U'(') {
          if (start < end) {
            auto text = s.substr(start, end - start);
            append(make_shared<Simple>(text));
            start = end;
          }
          offset = end + 1;
          auto group = make_shared<Group>();
          ParseGroup(s, offset, group->fChildren, U')');
          items.push_back(group);
          break;
        } else if (ch == U'<') {
          if (start < end) {
            auto text = s.substr(start, end - start);
            append(make_shared<Simple>(text));
            start = end;
          }
          offset = end + 1;
          u32string open;
          if (offset < s.size() && s[offset] == U'h') {
            open = U"h";
            offset++;
            if (offset < s.size()) {
              char32_t c = s[offset];
              if (c == U'0' || c == U'1' || c == U'2' || c == U'3') {
                open += c;
                offset = Whitespace(s, offset + 1);
              } else {
                offset = Whitespace(s, offset);
              }
            }
          } else if (offset < s.size()) {
            char32_t c = s[offset];
            if (c == U'0' || c == U'1' || c == U'2') {
              open += c;
              offset = Whitespace(s, offset + 1);
            }
          }
          auto cartouche = make_shared<Cartouche>();
          cartouche->fOpen = open;
          ParseGroup(s, offset, cartouche->fChildren, U'>');
          if (!cartouche->fChildren.empty()) {
            if (auto last = dynamic_pointer_cast<Simple>(cartouche->fChildren.back())) {
              auto c = last->fText;
              if (c == U"0" || c == U"1" || c == U"2" || c == U"h0" || c == U"h1" || c == U"h2" || c == U"h3") {
                cartouche->fClose = c;
                cartouche->fChildren.pop_back();
              }
            }
          }
          append(cartouche);
          break;
        } else if (ch == U'*') {
          if (start < end) {
            auto text = s.substr(start, end - start);
            append(make_shared<Simple>(text));
            start = end;
          }
          if (!items.empty()) {
            auto back = items.back();
            if (!dynamic_pointer_cast<HBox>(back)) {
              items.pop_back();
              auto hbox = make_shared<HBox>();
              hbox->fChildren.push_back(back);
              items.push_back(hbox);
            }
          }
          glue = U'*';
          offset = end + 1;
          break;
        } else if (ch == U':' || ch == U'&') {
          // TODO:
          break;
        } else if (ch == terminator) {
          if (start < end) {
            auto text = s.substr(start, end - start);
            append(make_shared<Simple>(text));
            start = end;
          }
          offset = end + 1;
          return;
        } else {
          offset = end + 1;
        }
      }
      if (start < end) {
        auto text = s.substr(start, end - start);
        append(make_shared<Simple>(text));
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
      document->fLines.push_back(line);
    }
    return document;
  }
};

TEST_CASE("Parse") {
  SUBCASE("simple") {
    auto d = Document::Parse(U"A1  B1\nC1");
    REQUIRE(d);
    CHECK(d->fLines.size() == 2);
    auto l0 = d->fLines[0];
    CHECK(l0->fItems.size() == 2);
    auto l1 = d->fLines[1];
    CHECK(l1->fItems.size() == 1);
  }
  SUBCASE("group") {
    auto d = Document::Parse(U"A1(A2 (B1)C1)");
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
  SUBCASE("cartouche") {
    auto d = Document::Parse(U"<1 ra mn xpr 2>");
    REQUIRE(d);
    CHECK(d->fLines.size() == 1);
    auto l = d->fLines[0];
    CHECK(l->fItems.size() == 1);
    auto c0 = dynamic_pointer_cast<Cartouche>(l->fItems[0]);
    REQUIRE(c0);
    CHECK(c0->fChildren.size() == 3);
    CHECK(c0->fOpen == U"1");
    CHECK(c0->fClose == U"2");
    auto c0c0 = dynamic_pointer_cast<Simple>(c0->fChildren[0]);
    auto c0c1 = dynamic_pointer_cast<Simple>(c0->fChildren[1]);
    auto c0c2 = dynamic_pointer_cast<Simple>(c0->fChildren[2]);
    REQUIRE(c0c0);
    CHECK(c0c0->fText == U"ra ");
    REQUIRE(c0c1);
    CHECK(c0c1->fText == U"mn ");
    REQUIRE(c0c2);
    CHECK(c0c2->fText == U"xpr ");
  }
  SUBCASE("hbox") {
    auto d = Document::Parse(U"(ib*Z1)*A1 B1");
    REQUIRE(d);
    CHECK(d->fLines.size() == 1);
    auto l = d->fLines[0];
    CHECK(l->fItems.size() == 2);
    auto c0 = dynamic_pointer_cast<HBox>(l->fItems[0]);
    auto c1 = dynamic_pointer_cast<Simple>(l->fItems[1]);
    REQUIRE(c0);
    CHECK(c0->fChildren.size() == 2);
    auto c0c0 = dynamic_pointer_cast<Group>(c0->fChildren[0]);
    auto c0c1 = dynamic_pointer_cast<Simple>(c0->fChildren[1]);
    REQUIRE(c0c0);
    CHECK(c0c0->fChildren.size() == 1);
    auto c0c0c0 = dynamic_pointer_cast<HBox>(c0c0->fChildren[0]);
    REQUIRE(c0c0c0);
    CHECK(c0c0c0->fChildren.size() == 2);
    auto c0c0c0c0 = dynamic_pointer_cast<Simple>(c0c0c0->fChildren[0]);
    auto c0c0c0c1 = dynamic_pointer_cast<Simple>(c0c0c0->fChildren[1]);
    REQUIRE(c0c0c0c0);
    CHECK(c0c0c0c0->fText == U"ib");
    REQUIRE(c0c0c0c1);
    CHECK(c0c0c0c1->fText == U"Z1");
    REQUIRE(c0c1);
    CHECK(c0c1->fText == U"A1 ");
    REQUIRE(c1);
    CHECK(c1->fText == U"B1");
  }
}

} // namespace ksesh::test
