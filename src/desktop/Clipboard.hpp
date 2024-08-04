#pragma once

namespace ksesh {

class Clipboard {
public:
  enum class Type {
#if JUCE_WINDOWS
    Emf,
#endif
    Png,
  };

  #if 0
  static bool Available(Type t) {
#if JUCE_WINDOWS
    if (!::OpenClipboard(nullptr)) {
      return false;
    }
    struct Closer {
      ~Closer() {
        ::CloseClipboard();
      }
    } closer;
    switch (t) {
    case Type::Emf:
      return (bool)::IsClipboardFormatAvailable(CF_ENHMETAFILE);
    case Type::Png:
      return (bool)::IsClipboardFormatAvailable(CF_DIB);
    }
#endif
    return false;
  }
  #endif

  static bool Store(std::string_view data, Type type) {
#if JUCE_WINDOWS
    if (!::OpenClipboard(nullptr)) {
      return false;
    }
    struct Closer {
      ~Closer() {
        ::CloseClipboard();
      }
    } closer;
    if (!::EmptyClipboard()) {
      return false;
    }
    switch (type) {
    case Type::Emf: {
      HGLOBAL mem = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT, data.size());
      if (INVALID_HANDLE_VALUE == mem) {
        return false;
      }
      void *ptr = GlobalLock(mem);
      if (!ptr) {
        return false;
      }
      memcpy(ptr, data.data(), data.size());
      HENHMETAFILE f = ::SetEnhMetaFileBits(data.size(), (BYTE *const)ptr);
      if (INVALID_HANDLE_VALUE == f) {
        ::GlobalUnlock(mem);
        ::GlobalFree(mem);
        return false;
      }
      ::GlobalUnlock(mem);
      return (bool)::SetClipboardData(CF_ENHMETAFILE, f);
    }
    case Type::Png: {
      juce::PNGImageFormat format;
      juce::MemoryInputStream stream(data.data(), data.size(), false);
      stream.setPosition(0);
      auto src = format.decodeImage(stream);
      if (src.isNull()) {
        return false;
      }
      int const width = src.getWidth();
      int const height = src.getHeight();
      juce::Image img(juce::Image::PixelFormat::ARGB, width, height, true);
      {
        juce::Graphics g(img);
        g.fillAll(juce::Colours::white);
        g.drawImageAt(src, 0, 0);
      }
      auto pixelData = img.getPixelData();
      int bpp;
      switch (img.getFormat()) {
      case juce::Image::PixelFormat::ARGB:
        bpp = 4;
        break;
      case juce::Image::PixelFormat::RGB:
        bpp = 3;
        break;
      default:
        return false;
      }
      int bitCount = bpp * 8;
      int stride = (((width * bitCount) + 31) & ~31) >> 3;
      HGLOBAL mem = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT, sizeof(BITMAPINFO) + stride * height);
      if (INVALID_HANDLE_VALUE == mem) {
        return false;
      }
      BITMAPINFO *info = (BITMAPINFO *)::GlobalLock(mem);
      info->bmiHeader.biSize = sizeof(BITMAPINFO);
      info->bmiHeader.biWidth = img.getWidth();
      info->bmiHeader.biHeight = img.getHeight();
      info->bmiHeader.biPlanes = 1;
      info->bmiHeader.biBitCount = bitCount;
      info->bmiHeader.biCompression = BI_RGB;
      info->bmiHeader.biSizeImage = stride * height;
      info->bmiHeader.biXPelsPerMeter = 0;
      info->bmiHeader.biYPelsPerMeter = 0;
      info->bmiHeader.biClrUsed = 0;
      info->bmiHeader.biClrImportant = 0;
      uint8_t *dst = (uint8_t*)info + sizeof(BITMAPINFO);
      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
          auto color = img.getPixelAt(x, height - y - 1);
          RGBQUAD c;
          c.rgbRed = color.getRed();
          c.rgbGreen = color.getGreen();
          c.rgbBlue = color.getBlue();
          c.rgbReserved = color.getAlpha();
          *(RGBQUAD *)(dst + (y * stride + bpp * x)) = c;
        }
      }
      ::GlobalUnlock(mem);
      return (bool)::SetClipboardData(CF_DIB, mem);
    }
    }
#else
#endif
    return false;
  }
};

} // namespace ksesh
