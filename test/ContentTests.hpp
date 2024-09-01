#include <deque>
#include <optional>
#include <variant>

namespace ksesh::test {

class Item {
public:
  virtual ~Item(){};
};

class Simple : public Item {
  Simple(std::u32string const &raw, std::u32string const &text) : fRaw(raw), fText(text) {
  }

public:
  static std::shared_ptr<Simple> Create(std::u32string const &s) {
    auto t = s;
    while (t.starts_with(U' ')) {
      t.erase(t.begin());
    }
    while (t.ends_with(U' ')) {
      t.erase(t.begin() + t.size() - 1);
    }
    class S : public Simple {
    public:
      S(std::u32string const &raw, std::u32string const &text) : Simple(raw, text) {}
    };
    return std::make_shared<S>(s, t);
  }

public:
  std::u32string const fRaw;
  std::u32string fText;
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

class VBox : public Item {
public:
  std::deque<std::shared_ptr<Item>> fChildren;
};

class Ligature : public Item {
public:
  std::deque<std::shared_ptr<Item>> fChildren;
};

class Placeholder : public Item {
};

class Line {
public:
  std::deque<std::shared_ptr<Item>> fItems;
};

struct Rect {
  float x;
  float y;
  float width;
  float height;
};

class Typeface {
public:
  virtual ~Typeface() {}
  virtual std::optional<Rect> getBounds(std::u32string const &glyph) = 0;
};

class GraphicsContext {
public:
  virtual ~GraphicsContext() {}
  virtual void draw(std::u32string const &glyph, Typeface &typeface) = 0;
};

class Document {
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

  struct Glue {
    Glue() = delete;
    struct Horizontal {};
    struct Vertical {};
    struct Ligature {};
  };
  using Entry = std::variant<std::shared_ptr<Item>, Glue::Horizontal, Glue::Vertical, Glue::Ligature>;

  template <class GlueType, class ContainerItemType>
  static void GlueEntries(std::deque<Entry> &entries) {
    using namespace std;
    if (entries.empty()) {
      return;
    }
    bool changed = true;
    while (changed) {
      changed = false;
      for (size_t i = 0; i < entries.size(); i++) {
        if (holds_alternative<GlueType>(entries[i])) {
          changed = true;
          auto hbox = make_shared<ContainerItemType>();
          if (i > 0 && holds_alternative<shared_ptr<Item>>(entries[i - 1])) {
            hbox->fChildren.push_back(get<shared_ptr<Item>>(entries[i - 1]));
          } else {
            hbox->fChildren.push_back(make_shared<Placeholder>());
          }
          size_t j = i + 1;
          while (j < entries.size() && holds_alternative<shared_ptr<Item>>(entries[j])) {
            auto item = get<shared_ptr<Item>>(entries[j]);
            hbox->fChildren.push_back(item);
            if (j + 1 >= entries.size() || !holds_alternative<GlueType>(entries[j + 1])) {
              break;
            }
            j += 2;
          }
          size_t erase = i > 0 ? i - 1 : i;
          for (size_t k = erase; k <= j; k++) {
            entries.erase(entries.begin() + erase);
          }
          entries.insert(entries.begin() + erase, hbox);
        }
      }
    }
  }

  static void ParseGroup(std::u32string const &s, size_t &offset, std::deque<std::shared_ptr<Item>> &buffer, char32_t terminator) {
    using namespace std;
    deque<Entry> entries;
    ParseGroupSequential(s, offset, entries, terminator);
    GlueEntries<Glue::Ligature, Ligature>(entries);
    GlueEntries<Glue::Horizontal, HBox>(entries);
    GlueEntries<Glue::Vertical, VBox>(entries);
    for (auto const &it : entries) {
      if (holds_alternative<shared_ptr<Item>>(it)) {
        buffer.push_back(get<shared_ptr<Item>>(it));
      }
    }
  }

  static void ParseGroupSequential(std::u32string const &s, size_t &offset, std::deque<Entry> &entries, char32_t terminator) {
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
            entries.push_back(Simple::Create(text));
            start = end;
          }
          offset = end;
          break;
        } else if (ch == U'(') {
          if (start < end) {
            auto text = s.substr(start, end - start);
            entries.push_back(Simple::Create(text));
            start = end;
          }
          offset = end + 1;
          auto group = make_shared<Group>();
          ParseGroup(s, offset, group->fChildren, U')');
          entries.push_back(group);
          break;
        } else if (ch == U'<') {
          if (start < end) {
            auto text = s.substr(start, end - start);
            entries.push_back(Simple::Create(text));
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
          entries.push_back(cartouche);
          break;
        } else if (ch == U'*' || ch == U':' || ch == U'&') {
          if (start < end) {
            auto text = s.substr(start, end - start);
            entries.push_back(Simple::Create(text));
            start = end;
          }
          if (ch == U'*') {
            entries.push_back(Glue::Horizontal{});
          } else if (ch == U':') {
            entries.push_back(Glue::Vertical{});
          } else {
            entries.push_back(Glue::Ligature{});
          }
          offset = end + 1;
          break;
        } else if (ch == terminator) {
          if (start < end) {
            auto text = s.substr(start, end - start);
            entries.push_back(Simple::Create(text));
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
        entries.push_back(Simple::Create(text));
      }
    }
  }

public:
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

public:
  std::deque<std::shared_ptr<Line>> fLines;
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
    CHECK(gc0->fText == U"A2");
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
    CHECK(c0c0->fText == U"ra");
    REQUIRE(c0c1);
    CHECK(c0c1->fText == U"mn");
    REQUIRE(c0c2);
    CHECK(c0c2->fText == U"xpr");
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
    CHECK(c0c1->fText == U"A1");
    REQUIRE(c1);
    CHECK(c1->fText == U"B1");
  }
  SUBCASE("vbox") {
    auto d = Document::Parse(U"A1:B1*C1:D1");
    REQUIRE(d);
    CHECK(d->fLines.size() == 1);
    auto l = d->fLines[0];
    CHECK(l->fItems.size() == 1);
    auto c0 = dynamic_pointer_cast<VBox>(l->fItems[0]);
    REQUIRE(c0);
    CHECK(c0->fChildren.size() == 3);
    auto c0c0 = dynamic_pointer_cast<Simple>(c0->fChildren[0]);
    auto c0c1 = dynamic_pointer_cast<HBox>(c0->fChildren[1]);
    auto c0c2 = dynamic_pointer_cast<Simple>(c0->fChildren[2]);
    REQUIRE(c0c0);
    REQUIRE(c0c1);
    REQUIRE(c0c2);
  }
  SUBCASE("complex") {
    auto d = Document::Parse(U"gb&ra <i mn:n t&w&t anx>");
    REQUIRE(d);
    REQUIRE(d->fLines.size() == 1);
    auto l = d->fLines[0];
    REQUIRE(l->fItems.size() == 2);
    auto c0 = dynamic_pointer_cast<Ligature>(l->fItems[0]);
    auto c1 = dynamic_pointer_cast<Cartouche>(l->fItems[1]);
    REQUIRE(c0);
    REQUIRE(c1);
    REQUIRE(c0->fChildren.size() == 2);
    auto c0c0 = dynamic_pointer_cast<Simple>(c0->fChildren[0]);
    auto c0c1 = dynamic_pointer_cast<Simple>(c0->fChildren[1]);
    REQUIRE(c0c0);
    REQUIRE(c0c1);
    CHECK(c0c0->fText == U"gb");
    CHECK(c0c1->fText == U"ra");
    REQUIRE(c1->fChildren.size() == 4);
    auto c1c0 = dynamic_pointer_cast<Simple>(c1->fChildren[0]);
    auto c1c1 = dynamic_pointer_cast<VBox>(c1->fChildren[1]);
    auto c1c2 = dynamic_pointer_cast<Ligature>(c1->fChildren[2]);
    auto c1c3 = dynamic_pointer_cast<Simple>(c1->fChildren[3]);
    REQUIRE(c1c0);
    REQUIRE(c1c1);
    REQUIRE(c1c2);
    REQUIRE(c1c3);

    CHECK(c1c0->fText == U"i");

    REQUIRE(c1c1->fChildren.size() == 2);
    auto c1c1c0 = dynamic_pointer_cast<Simple>(c1c1->fChildren[0]);
    auto c1c1c1 = dynamic_pointer_cast<Simple>(c1c1->fChildren[1]);
    REQUIRE(c1c1c0);
    REQUIRE(c1c1c1);
    CHECK(c1c1c0->fText == U"mn");
    CHECK(c1c1c1->fText == U"n");

    REQUIRE(c1c2->fChildren.size() == 3);
    auto c1c2c0 = dynamic_pointer_cast<Simple>(c1c2->fChildren[0]);
    auto c1c2c1 = dynamic_pointer_cast<Simple>(c1c2->fChildren[1]);
    auto c1c2c2 = dynamic_pointer_cast<Simple>(c1c2->fChildren[2]);
    REQUIRE(c1c2c0);
    REQUIRE(c1c2c1);
    REQUIRE(c1c2c2);
    CHECK(c1c2c0->fText == U"t");
    CHECK(c1c2c1->fText == U"w");
    CHECK(c1c2c2->fText == U"t");

    CHECK(c1c3->fText == U"anx");
  }
}

} // namespace ksesh::test
