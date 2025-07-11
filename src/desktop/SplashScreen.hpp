#pragma once

namespace ksesh {

class SplashScreen : public juce::SplashScreen, public juce::AsyncUpdater {
public:
  class Delegate {
  public:
    virtual ~Delegate() {}
    virtual void splashScreenDidFinishLoadingFont(FontSet const &fontSet) = 0;
  };

  explicit SplashScreen(Delegate *delegate) : juce::SplashScreen("KSesh", 640, 360, true), fDelegate(delegate) {
    std::promise<std::shared_ptr<FontAdapter>> promiseEgyptianText;
    fFontFutureEgyptianText = promiseEgyptianText.get_future();
    std::thread(LoadEgyptianTextFont, std::move(promiseEgyptianText)).detach();

    std::promise<std::shared_ptr<FontAdapter>> promiseNewGardiner;
    fFontFutureNewGardiner = promiseNewGardiner.get_future();
    std::thread(&SplashScreen::transformFont, this, std::move(promiseNewGardiner), FontFamily::NewGardiner).detach();

    std::promise<std::shared_ptr<FontAdapter>> promiseNotoSans;
    fFontFutureNotoSans = promiseNotoSans.get_future();
    std::thread(&SplashScreen::transformFont, this, std::move(promiseNotoSans), FontFamily::NotoSans).detach();

    fAppIcon = juce::ImageFileFormat::loadFrom(BinaryData::icon_512x512_png, BinaryData::icon_512x512_pngSize);
  }

  ~SplashScreen() {
    if (fFontFutureNewGardiner.valid()) {
      fFontFutureNewGardiner.get();
    }
    if (fFontFutureNotoSans.valid()) {
      fFontFutureNotoSans.get();
    }
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(juce::Colours::white.darker());
    if (fAppIcon.isValid()) {
      g.drawImageWithin(fAppIcon, 30, 30, 120, 120, {});
    }
    g.setColour(juce::Colours::black.brighter());
    g.setFont(50);
    float x = 170;
    float baseline = 140;
    g.drawSingleLineText(JUCE_APPLICATION_NAME_STRING, x, baseline);
    x += juce::GlyphArrangement::getStringWidth(g.getCurrentFont(), JUCE_APPLICATION_NAME_STRING);
    g.setFont(25);
    g.drawSingleLineText(" " JUCE_APPLICATION_VERSION_STRING, x, baseline);
  }

  void handleAsyncUpdate() override {
    if (!fFontSet.fEgyptianText && fFontFutureEgyptianText.valid()) {
      if (fFontFutureEgyptianText.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
        auto font = fFontFutureEgyptianText.get();
        if (font) {
          fFontSet.fEgyptianText = font;
        }
      }
    }
    if (!fFontSet.fNewGardiner && fFontFutureNewGardiner.valid()) {
      if (fFontFutureNewGardiner.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
        auto font = fFontFutureNewGardiner.get();
        if (font) {
          fFontSet.fNewGardiner = font;
        }
      }
    }
    if (!fFontSet.fNotoSans && fFontFutureNotoSans.valid()) {
      if (fFontFutureNotoSans.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
        auto font = fFontFutureNotoSans.get();
        if (font) {
          fFontSet.fNotoSans = font;
        }
      }
    }
    if (!fFontFutureEgyptianText.valid() && !fFontFutureNewGardiner.valid() && !fFontFutureNotoSans.valid() && fFontSet.fEgyptianText) {
      fDelegate->splashScreenDidFinishLoadingFont(fFontSet);
    }
  }

private:
  void transformFont(std::promise<std::shared_ptr<FontAdapter>> promise, FontFamily fontFamily) {
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
    eglyf::ByteInputStream inputStream(data);
    std::shared_ptr<eglyf::Font> font;
    if (auto st = eglyf::Font::Read(inputStream, font); !st.ok()) {
      promise.set_value(nullptr);
      return;
    }
    eglyf::Config config;
    config.enableSubstMdc = false;
    if (auto st = eglyf::Transformer::Transform(font, config); !st.ok()) {
      promise.set_value(nullptr);
      return;
    }
    eglyf::ByteOutputStream outputStream;
    if (auto st = font->write(outputStream); !st.ok()) {
      promise.set_value(nullptr);
      return;
    }
    std::string transformed = outputStream.data();
    HbBlobUniquePtr blob(hb_blob_create(transformed.data(), transformed.size(), HB_MEMORY_MODE_DUPLICATE, nullptr, nullptr));
    HbFaceUniquePtr face(hb_face_create(blob.get(), 0));
    auto hbFont = HbMakeSharedFontPtr(hb_font_create(face.get()));
    promise.set_value(std::make_shared<FontAdapter>(hbFont));
  }

  static void LoadEgyptianTextFont(std::promise<std::shared_ptr<FontAdapter>> promise) {
    HbBlobUniquePtr blob(hb_blob_create(BinaryData::eot_ttf,
                                        BinaryData::eot_ttfSize,
                                        HB_MEMORY_MODE_READONLY,
                                        nullptr,
                                        nullptr));
    HbFaceUniquePtr face(hb_face_create(blob.get(), 0));
    auto hbFont = HbMakeSharedFontPtr(hb_font_create(face.get()));
    auto font = std::make_shared<FontAdapter>(hbFont);
    promise.set_value(font);
  }

private:
  Delegate *const fDelegate;
  std::future<std::shared_ptr<FontAdapter>> fFontFutureEgyptianText;
  std::future<std::shared_ptr<FontAdapter>> fFontFutureNewGardiner;
  std::future<std::shared_ptr<FontAdapter>> fFontFutureNotoSans;
  FontSet fFontSet;
  juce::Image fAppIcon;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SplashScreen)
};

} // namespace ksesh
