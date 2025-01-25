#include <deque>
#include <optional>
#include <variant>

#include "Insertions.hpp"
#include "SignList.hpp"

namespace ksesh::test {

class Token {
public:
  virtual ~Token() {};
};

class Simple : public Token {
  Simple(std::u32string const &raw, std::u32string const &text, std::u32string const &sign) : fRaw(raw), fText(text), fSign(sign) {
  }

public:
  static std::shared_ptr<Simple> Create(std::u32string const &s) {
    std::u32string t = s;
    while (t.starts_with(U' ')) {
      t.erase(t.begin());
    }
    while (t.ends_with(U' ')) {
      t.erase(t.begin() + t.size() - 1);
    }
    std::u32string sign = U"";
    if (auto found = SignList::Map(t, 0)) {
      sign = found->second;
    }
    class S : public Simple {
    public:
      S(std::u32string const &raw, std::u32string const &text, std::u32string const &sign) : Simple(raw, text, sign) {}
    };
    return std::make_shared<S>(s, t, sign);
  }

public:
  std::u32string const fRaw;
  std::u32string const fText;
  std::u32string const fSign;
};

class Group : public Token {
public:
  std::deque<std::shared_ptr<Token>> fChildren;
};

class Cartouche : public Token {
public:
  std::u32string fOpen;
  std::deque<std::shared_ptr<Token>> fChildren;
  std::u32string fClose;
};

class HBox : public Token {
public:
  std::deque<std::shared_ptr<Token>> fChildren;
};

class VBox : public Token {
public:
  std::deque<std::shared_ptr<Token>> fChildren;
};

class Ligature : public Token {
public:
  std::deque<std::shared_ptr<Token>> fChildren;
};

class Placeholder : public Token {
};

struct Point {
  float x;
  float y;
};

struct Size {
  float width;
  float height;
};

struct Rect {
  Point origin;
  Size size;
};

class Typeface {
public:
  using GlyphId = uint32_t;
  virtual ~Typeface() {}
  virtual std::optional<GlyphId> getGlyphId(std::u32string const &glyph) = 0;
  virtual std::optional<Rect> getGlyphBounds(GlyphId glyphId) = 0;
};

class GraphicsContext {
public:
  virtual ~GraphicsContext() {}
  virtual void draw(Typeface::GlyphId glyphId, Typeface &typeface) = 0;
};

class Draw {
public:
  virtual ~Draw() {}
};

class DrawGlyph : public Draw {
public:
  void draw(GraphicsContext &ctx, Typeface::GlyphId glyphId, Point origin, float scale) {
    // TODO:
  }
};

class Line {
public:
  float fWidth;
  std::deque<std::shared_ptr<Draw>> fCommands;
  std::deque<std::shared_ptr<Token>> fTokens;
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
  using Entry = std::variant<std::shared_ptr<Token>, Glue::Horizontal, Glue::Vertical, Glue::Ligature>;

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
          if (i > 0 && holds_alternative<shared_ptr<Token>>(entries[i - 1])) {
            hbox->fChildren.push_back(get<shared_ptr<Token>>(entries[i - 1]));
          } else {
            hbox->fChildren.push_back(make_shared<Placeholder>());
          }
          size_t j = i + 1;
          while (j < entries.size() && holds_alternative<shared_ptr<Token>>(entries[j])) {
            auto item = get<shared_ptr<Token>>(entries[j]);
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

  static void ParseGroup(std::u32string const &s, size_t &offset, std::deque<std::shared_ptr<Token>> &buffer, char32_t terminator) {
    using namespace std;
    deque<Entry> entries;
    ParseGroupSequential(s, offset, entries, terminator);
    GlueEntries<Glue::Ligature, Ligature>(entries);
    GlueEntries<Glue::Horizontal, HBox>(entries);
    GlueEntries<Glue::Vertical, VBox>(entries);
    for (auto const &it : entries) {
      if (holds_alternative<shared_ptr<Token>>(it)) {
        buffer.push_back(get<shared_ptr<Token>>(it));
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

  static std::shared_ptr<Line> ParseLine(std::u32string const &s, size_t &offset, Typeface &typeface) {
    using namespace std;
    auto line = make_shared<Line>();
    ParseGroup(s, offset, line->fTokens, U'\n');
    return line;
  }

public:
  static std::shared_ptr<Document> Parse(std::u32string const &s, Typeface &typeface) {
    using namespace std;
    auto document = make_shared<Document>();
    size_t offset = 0;
    while (offset < s.size()) {
      auto line = ParseLine(s, offset, typeface);
      if (!line) {
        return nullptr;
      }
      document->fLines.push_back(line);
    }
    return document;
  }

public:
  std::deque<std::shared_ptr<Line>> fLines;
};

class JuceTypeface : public Typeface {
public:
  std::optional<GlyphId> getGlyphId(std::u32string const &glyph) override {
    return std::nullopt;
  }

  std::optional<Rect> getGlyphBounds(GlyphId glyphId) override {
    return std::nullopt;
  }
};

TEST_CASE("Parse") {
  SUBCASE("simple") {
    auto typeface = std::make_shared<JuceTypeface>();
    auto d = Document::Parse(U"A1  B1\nC1", *typeface);
    REQUIRE(d);
    CHECK(d->fLines.size() == 2);
    auto l0 = d->fLines[0];
    CHECK(l0->fTokens.size() == 2);
    auto l1 = d->fLines[1];
    CHECK(l1->fTokens.size() == 1);
  }
  SUBCASE("group") {
    auto typeface = std::make_shared<JuceTypeface>();
    auto d = Document::Parse(U"A1(A2 (B1)C1)", *typeface);
    REQUIRE(d);
    CHECK(d->fLines.size() == 1);
    auto l = d->fLines[0];
    CHECK(l->fTokens.size() == 2);
    auto s = dynamic_pointer_cast<Simple>(l->fTokens[0]);
    auto g = dynamic_pointer_cast<Group>(l->fTokens[1]);
    REQUIRE(s);
    CHECK(s->fSign == U"𓀀");
    REQUIRE(g);
    CHECK(g->fChildren.size() == 3);
    auto gc0 = dynamic_pointer_cast<Simple>(g->fChildren[0]);
    auto gc1 = dynamic_pointer_cast<Group>(g->fChildren[1]);
    auto gc2 = dynamic_pointer_cast<Simple>(g->fChildren[2]);
    REQUIRE(gc0);
    CHECK(gc0->fSign == U"𓀁");
    REQUIRE(gc1);
    CHECK(gc1->fChildren.size() == 1);
    REQUIRE(gc2);
    CHECK(gc2->fSign == U"𓁚");
    auto gc1c0 = dynamic_pointer_cast<Simple>(gc1->fChildren[0]);
    REQUIRE(gc1c0);
    CHECK(gc1c0->fSign == U"𓁐");
  }
  SUBCASE("cartouche") {
    auto typeface = std::make_shared<JuceTypeface>();
    auto d = Document::Parse(U"<1 ra mn xpr 2>", *typeface);
    REQUIRE(d);
    CHECK(d->fLines.size() == 1);
    auto l = d->fLines[0];
    CHECK(l->fTokens.size() == 1);
    auto c0 = dynamic_pointer_cast<Cartouche>(l->fTokens[0]);
    REQUIRE(c0);
    CHECK(c0->fChildren.size() == 3);
    CHECK(c0->fOpen == U"1");
    CHECK(c0->fClose == U"2");
    auto c0c0 = dynamic_pointer_cast<Simple>(c0->fChildren[0]);
    auto c0c1 = dynamic_pointer_cast<Simple>(c0->fChildren[1]);
    auto c0c2 = dynamic_pointer_cast<Simple>(c0->fChildren[2]);
    REQUIRE(c0c0);
    CHECK(c0c0->fSign == U"𓇳");
    REQUIRE(c0c1);
    CHECK(c0c1->fSign == U"𓏠");
    REQUIRE(c0c2);
    CHECK(c0c2->fSign == U"𓆣");
  }
  SUBCASE("hbox") {
    auto typeface = std::make_shared<JuceTypeface>();
    auto d = Document::Parse(U"(ib*Z1)*A1 B1", *typeface);
    REQUIRE(d);
    CHECK(d->fLines.size() == 1);
    auto l = d->fLines[0];
    CHECK(l->fTokens.size() == 2);
    auto c0 = dynamic_pointer_cast<HBox>(l->fTokens[0]);
    auto c1 = dynamic_pointer_cast<Simple>(l->fTokens[1]);
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
    CHECK(c0c0c0c0->fSign == U"𓄣");
    REQUIRE(c0c0c0c1);
    CHECK(c0c0c0c1->fSign == U"𓏤");
    REQUIRE(c0c1);
    CHECK(c0c1->fSign == U"𓀀");
    REQUIRE(c1);
    CHECK(c1->fSign == U"𓁐");
  }
  SUBCASE("vbox") {
    auto typeface = std::make_shared<JuceTypeface>();
    auto d = Document::Parse(U"A1:B1*C1:D1", *typeface);
    REQUIRE(d);
    CHECK(d->fLines.size() == 1);
    auto l = d->fLines[0];
    CHECK(l->fTokens.size() == 1);
    auto c0 = dynamic_pointer_cast<VBox>(l->fTokens[0]);
    REQUIRE(c0);
    CHECK(c0->fChildren.size() == 3);
    auto c0c0 = dynamic_pointer_cast<Simple>(c0->fChildren[0]);
    auto c0c1 = dynamic_pointer_cast<HBox>(c0->fChildren[1]);
    auto c0c2 = dynamic_pointer_cast<Simple>(c0->fChildren[2]);
    REQUIRE(c0c0);
    REQUIRE(c0c1);
    REQUIRE(c0c2);
    CHECK(c0c0->fSign == U"𓀀");
    CHECK(c0c1->fChildren.size() == 2);
    auto c0c1c0 = dynamic_pointer_cast<Simple>(c0c1->fChildren[0]);
    auto c0c1c1 = dynamic_pointer_cast<Simple>(c0c1->fChildren[1]);
    REQUIRE(c0c1c0);
    REQUIRE(c0c1c1);
    CHECK(c0c1c0->fSign == U"𓁐");
    CHECK(c0c1c1->fSign == U"𓁚");
    CHECK(c0c2->fSign == U"𓁶");
  }
  SUBCASE("complex") {
    auto typeface = std::make_shared<JuceTypeface>();
    auto d = Document::Parse(U"gb&ra <i mn:n t&w&t anx>", *typeface);
    REQUIRE(d);
    REQUIRE(d->fLines.size() == 1);
    auto l = d->fLines[0];
    REQUIRE(l->fTokens.size() == 2);
    auto c0 = dynamic_pointer_cast<Ligature>(l->fTokens[0]);
    auto c1 = dynamic_pointer_cast<Cartouche>(l->fTokens[1]);
    REQUIRE(c0);
    REQUIRE(c1);
    REQUIRE(c0->fChildren.size() == 2);
    auto c0c0 = dynamic_pointer_cast<Simple>(c0->fChildren[0]);
    auto c0c1 = dynamic_pointer_cast<Simple>(c0->fChildren[1]);
    REQUIRE(c0c0);
    REQUIRE(c0c1);
    CHECK(c0c0->fSign == U"𓅬");
    CHECK(c0c1->fSign == U"𓇳");
    REQUIRE(c1->fChildren.size() == 4);
    auto c1c0 = dynamic_pointer_cast<Simple>(c1->fChildren[0]);
    auto c1c1 = dynamic_pointer_cast<VBox>(c1->fChildren[1]);
    auto c1c2 = dynamic_pointer_cast<Ligature>(c1->fChildren[2]);
    auto c1c3 = dynamic_pointer_cast<Simple>(c1->fChildren[3]);
    REQUIRE(c1c0);
    REQUIRE(c1c1);
    REQUIRE(c1c2);
    REQUIRE(c1c3);

    CHECK(c1c0->fSign == U"𓇋");

    REQUIRE(c1c1->fChildren.size() == 2);
    auto c1c1c0 = dynamic_pointer_cast<Simple>(c1c1->fChildren[0]);
    auto c1c1c1 = dynamic_pointer_cast<Simple>(c1c1->fChildren[1]);
    REQUIRE(c1c1c0);
    REQUIRE(c1c1c1);
    CHECK(c1c1c0->fSign == U"𓏠");
    CHECK(c1c1c1->fSign == U"𓈖");

    REQUIRE(c1c2->fChildren.size() == 3);
    auto c1c2c0 = dynamic_pointer_cast<Simple>(c1c2->fChildren[0]);
    auto c1c2c1 = dynamic_pointer_cast<Simple>(c1c2->fChildren[1]);
    auto c1c2c2 = dynamic_pointer_cast<Simple>(c1c2->fChildren[2]);
    REQUIRE(c1c2c0);
    REQUIRE(c1c2c1);
    REQUIRE(c1c2c2);
    CHECK(c1c2c0->fSign == U"𓏏");
    CHECK(c1c2c1->fSign == U"𓅱");
    CHECK(c1c2c2->fSign == U"𓏏");

    CHECK(c1c3->fSign == U"𓋹");
  }
}

} // namespace ksesh::test
