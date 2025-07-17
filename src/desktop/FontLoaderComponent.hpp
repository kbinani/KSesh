#pragma once

namespace ksesh {

class FontLoaderComponent : public juce::SplashScreen, public juce::AsyncUpdater {
  using Self = FontLoaderComponent;

  class Line {
  public:
    Line(juce::String const &text, bool error) : fText(text), fError(error) {}

    juce::String fText;
    bool fError;
  };

  using LoadResult = std::variant<std::shared_ptr<FontAdapter>, eglyf::Status, ksesh::Status>;

public:
  class Delegate {
  public:
    virtual ~Delegate() {}
    virtual void fontLoaderComponentDidFinishLoadingFont(FontSet const &fontSet, bool error) = 0;
  };

  explicit FontLoaderComponent(Delegate *delegate) : juce::SplashScreen(JUCE_APPLICATION_NAME_STRING, 640, 360, true), fDelegate(delegate) {
    std::promise<LoadResult> promiseEgyptianText;
    fFontFutureEgyptianText = promiseEgyptianText.get_future();
    std::thread(&Self::loadEgyptianTextFont, this, std::move(promiseEgyptianText)).detach();

    std::promise<LoadResult> promiseNewGardiner;
    fFontFutureNewGardiner = promiseNewGardiner.get_future();
    std::thread(&Self::transformFont, this, std::move(promiseNewGardiner), FontFamily::NewGardiner).detach();

    std::promise<LoadResult> promiseNotoSans;
    fFontFutureNotoSans = promiseNotoSans.get_future();
    std::thread(&Self::transformFont, this, std::move(promiseNotoSans), FontFamily::NotoSans).detach();

    auto base = juce::Colour::fromRGB(0x50, 0x9B, 0xEA);
    fForegroundColor = base.darker().darker().darker();
    fBackgroundColor = base.brighter();
    fForegroundErrorColor = juce::Colour::fromRGB(0xdc, 0x19, 0x35);

    fAppIcon = juce::ImageFileFormat::loadFrom(BinaryData::icon_512x512_png, BinaryData::icon_512x512_pngSize);
    fStart = std::chrono::steady_clock::now();
    appendLog("Loading EgyptianText font...", false);
    appendLog("Loading NewGardiner font...", false);
    appendLog("Loading Noto Sans Egyptian Hieroglyphs font...", false);
  }

  ~FontLoaderComponent() {
    if (fFontFutureEgyptianText.valid()) {
      fFontFutureEgyptianText.get();
    }
    if (fFontFutureNewGardiner.valid()) {
      fFontFutureNewGardiner.get();
    }
    if (fFontFutureNotoSans.valid()) {
      fFontFutureNotoSans.get();
    }
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(fBackgroundColor);
    if (fAppIcon.isValid()) {
      g.setOpacity(1.0f);
      g.drawImageWithin(fAppIcon, 30, 30, 120, 120, {});
    }
    g.setColour(fForegroundColor);
    g.setFont(50);
    float x = 170;
    float baseline = 140;
    g.drawSingleLineText(JUCE_APPLICATION_NAME_STRING, x, baseline);
    x += juce::GlyphArrangement::getStringWidth(g.getCurrentFont(), JUCE_APPLICATION_NAME_STRING);
    g.setFont(25);
    g.drawSingleLineText(" " JUCE_APPLICATION_VERSION_STRING, x, baseline);
  }

  void paintOverChildren(juce::Graphics &g) override {
    float size = 12;
    float x = 30;
    float y = getHeight() - 30 - size * (fLogLines.size() - 1);
    auto font = juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), size, juce::Font::plain));
    g.setFont(font);
    for (auto const &line : fLogLines) {
      if (line.fError) {
        g.setColour(fForegroundErrorColor);
      } else {
        g.setColour(fForegroundColor);
      }
      g.drawSingleLineText(line.fText, x, y);
      y += size;
    }
  }

  void handleAsyncUpdate() override {
    auto accept = [this](std::shared_ptr<FontAdapter> &font, std::future<LoadResult> &future, juce::String const &name) {
      if (!font && future.valid()) {
        if (future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
          auto result = future.get();
          if (std::holds_alternative<std::shared_ptr<FontAdapter>>(result)) {
            auto r = std::get<std::shared_ptr<FontAdapter>>(result);
            if (r) {
              font.swap(r);
              appendLog(juce::String("Successfully loaded ") + name + " font.", false);
            } else {
              appendLog(juce::String("Failed to load ") + name + " font.", true);
            }
          } else if (std::holds_alternative<eglyf::Status>(result)) {
            appendLog(juce::String("Failed to load ") + name + " font.", true);
            auto st = std::get<eglyf::Status>(result);
            std::stringstream stream;
            st.print(stream, "  ");
            stream.seekg(0);
            std::string line;
            while (std::getline(stream, line)) {
              appendLog(juce::String("  ") + juce::String(line), true);
            }
          } else if (std::holds_alternative<ksesh::Status>(result)) {
            appendLog(juce::String("Failed to load ") + name + " font.", true);
            auto st = std::get<ksesh::Status>(result);
            std::stringstream stream;
            st.print(stream, "  ");
            stream.seekg(0);
            std::string line;
            while (std::getline(stream, line)) {
              appendLog(juce::String("  ") + juce::String(line), true);
            }
          }
          repaint();
        }
      }
    };
    accept(fFontSet.fEgyptianText, fFontFutureEgyptianText, "EgyptianText");
    accept(fFontSet.fNewGardiner, fFontFutureNewGardiner, "NewGardiner");
    accept(fFontSet.fNotoSans, fFontFutureNotoSans, "Noto Sans Egyptian Hieroglyphs");
    if (!fFontFutureEgyptianText.valid() && !fFontFutureNewGardiner.valid() && !fFontFutureNotoSans.valid() && fDelegate) {
      if (fFontSet.ok()) {
        fDelegate->fontLoaderComponentDidFinishLoadingFont(fFontSet, false);
        appendLog("Font loading completed.", false);
      } else {
        fDelegate->fontLoaderComponentDidFinishLoadingFont(fFontSet, true);
        appendLog("Font loading failed.", true);
      }
    }
  }

private:
  void appendLog(juce::String const &m, bool error) {
    float elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - fStart).count() / 1000.0f;
    auto prefix = std::format("[{0:7.3f}]", elapsed);
    fLogLines.push_back(Line(juce::String(prefix) + " " + m, error));
  }

  void transformFont(std::promise<LoadResult> promise, FontFamily fontFamily) {
    defer {
      triggerAsyncUpdate();
    };
    std::string_view data;
    switch (fontFamily) {
    case FontFamily::EgyptianText:
      promise.set_value(nullptr);
      return;
    case FontFamily::NewGardiner:
      data = std::string_view(BinaryData::NewGardiner_ttf, BinaryData::NewGardiner_ttfSize);
      break;
    case FontFamily::NotoSans:
      data = std::string_view(BinaryData::NotoSansEgyptianHieroglyphsRegular_ttf, BinaryData::NotoSansEgyptianHieroglyphsRegular_ttfSize);
      break;
    }

    auto cache = FilePath::FontCacheFilePath(data.data(), data.size());
    if (cache != juce::File() && cache.existsAsFile()) {
      auto font = LoadFontFromCache(cache);
      if (font) {
        promise.set_value(font);
        return;
      } else {
        cache.deleteFile();
      }
    }

    eglyf::ByteInputStream inputStream(data);
    std::shared_ptr<eglyf::Font> font;
    if (auto st = eglyf::Font::Read(inputStream, font); !st.ok()) {
      promise.set_value(st);
      return;
    }
    eglyf::Config config;
    config.enableSubstMdc = false;
    if (auto st = eglyf::Transformer::Transform(font, config); !st.ok()) {
      promise.set_value(st);
      return;
    }
    eglyf::ByteOutputStream outputStream;
    if (auto st = font->write(outputStream); !st.ok()) {
      promise.set_value(st);
      return;
    }
    std::string_view transformed = outputStream.view();
    if (cache != juce::File()) {
      juce::FileOutputStream fileStream(cache);
      if (fileStream.openedOk()) {
        fileStream.setPosition(0);
        fileStream.truncate();
        juce::GZIPCompressorOutputStream compressorStream(&fileStream, -1, false, juce::GZIPCompressorOutputStream::windowBitsGZIP);
        compressorStream.write(transformed.data(), transformed.size());
      }
    }
    HbBlobUniquePtr blob(hb_blob_create_or_fail(transformed.data(), transformed.size(), HB_MEMORY_MODE_DUPLICATE, nullptr, nullptr));
    if (!blob) {
      promise.set_value(KSESH_ERROR);
      return;
    }
    HbFaceUniquePtr face(hb_face_create_or_fail(blob.get(), 0));
    if (!face) {
      promise.set_value(KSESH_ERROR);
      return;
    }
    auto hbFont = HbMakeSharedFontPtr(hb_font_create(face.get()));
    promise.set_value(std::make_shared<FontAdapter>(hbFont));
  }

  void loadEgyptianTextFont(std::promise<LoadResult> promise) {
    defer {
      triggerAsyncUpdate();
    };
    HbBlobUniquePtr blob(hb_blob_create_or_fail(BinaryData::eot_ttf,
                                                BinaryData::eot_ttfSize,
                                                HB_MEMORY_MODE_READONLY,
                                                nullptr,
                                                nullptr));
    if (!blob) {
      promise.set_value(KSESH_ERROR);
      return;
    }
    HbFaceUniquePtr face(hb_face_create_or_fail(blob.get(), 0));
    if (!face) {
      promise.set_value(KSESH_ERROR);
      return;
    }
    auto hbFont = HbMakeSharedFontPtr(hb_font_create(face.get()));
    auto font = std::make_shared<FontAdapter>(hbFont);
    promise.set_value(font);
  }

  static std::shared_ptr<FontAdapter> LoadFontFromCache(juce::File const &file) {
    juce::FileInputStream fileStream(file);
    if (!fileStream.openedOk()) {
      return nullptr;
    }
    juce::GZIPDecompressorInputStream decompressorStream(&fileStream, false, juce::GZIPDecompressorInputStream::gzipFormat);
    std::vector<uint8_t> buffer;
    while (!decompressorStream.isExhausted()) {
      uint8_t chunk[512];
      int read = decompressorStream.read(chunk, 512);
      std::copy_n(chunk, read, std::back_inserter(buffer));
    }
    HbBlobUniquePtr blob(hb_blob_create_or_fail((char const *)buffer.data(), buffer.size(), HB_MEMORY_MODE_DUPLICATE, nullptr, nullptr));
    if (!blob) {
      return nullptr;
    }
    HbFaceUniquePtr face(hb_face_create_or_fail(blob.get(), 0));
    if (!face) {
      return nullptr;
    }
    auto hbFont = HbMakeSharedFontPtr(hb_font_create(face.get()));
    return std::make_shared<FontAdapter>(hbFont);
  }

private:
  Delegate *const fDelegate;
  std::future<LoadResult> fFontFutureEgyptianText;
  std::future<LoadResult> fFontFutureNewGardiner;
  std::future<LoadResult> fFontFutureNotoSans;
  FontSet fFontSet;
  juce::Image fAppIcon;
  std::vector<Line> fLogLines;
  juce::Colour fForegroundColor;
  juce::Colour fForegroundErrorColor;
  juce::Colour fBackgroundColor;
  std::chrono::steady_clock::time_point fStart;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FontLoaderComponent)
};

} // namespace ksesh
