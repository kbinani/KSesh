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
    std::promise<std::shared_ptr<hb_font_t>> promiseEgyptianText;
    fFontFutureEgyptianText = promiseEgyptianText.get_future();
    std::thread(LoadEgyptianTextFont, std::move(promiseEgyptianText)).detach();

    std::promise<std::shared_ptr<hb_font_t>> promiseNewGardiner;
    fFontFutureNewGardiner = promiseNewGardiner.get_future();
    std::thread(&SplashScreen::transformFont, this, std::move(promiseNewGardiner), FontFamily::NewGardiner).detach();

    std::promise<std::shared_ptr<hb_font_t>> promiseNotoSans;
    fFontFutureNotoSans = promiseNotoSans.get_future();
    std::thread(&SplashScreen::transformFont, this, std::move(promiseNotoSans), FontFamily::NotoSans).detach();
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
    g.fillAll(juce::Colours::red);
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
  void transformFont(std::promise<std::shared_ptr<hb_font_t>> promise, FontFamily fontFamily) {
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
    HbBlobUniquePtr blob(hb_blob_create(transformed.data(), transformed.size(), HB_MEMORY_MODE_READONLY, nullptr, nullptr));
    HbFaceUniquePtr face(hb_face_create(blob.get(), 0));
    auto hbFont = HbMakeSharedFontPtr(hb_font_create(face.get()));
    promise.set_value(hbFont);
  }

  static void LoadEgyptianTextFont(std::promise<std::shared_ptr<hb_font_t>> promise) {
    HbBlobUniquePtr blob(hb_blob_create(BinaryData::eot_ttf,
                                        BinaryData::eot_ttfSize,
                                        HB_MEMORY_MODE_READONLY,
                                        nullptr,
                                        nullptr));
    HbFaceUniquePtr face(hb_face_create(blob.get(), 0));
    promise.set_value(HbMakeSharedFontPtr(hb_font_create(face.get())));
  }

private:
  Delegate *const fDelegate;
  std::future<std::shared_ptr<hb_font_t>> fFontFutureEgyptianText;
  std::future<std::shared_ptr<hb_font_t>> fFontFutureNewGardiner;
  std::future<std::shared_ptr<hb_font_t>> fFontFutureNotoSans;
  FontSet fFontSet;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SplashScreen)
};

} // namespace ksesh
