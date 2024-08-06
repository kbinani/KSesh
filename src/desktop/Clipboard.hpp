#pragma once

namespace ksesh {

class Clipboard {
public:
  enum class Type {
#if defined(JUCE_WINDOWS)
    Emf,
#endif
    Png,
#if defined(JUCE_MAC)
    Pdf,
#endif
  };

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
      if (!mem) {
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
      auto img = format.decodeImage(stream);
      if (img.isNull()) {
        return false;
      }
      int const width = img.getWidth();
      int const height = img.getHeight();
      juce::Image::BitmapData bitmapData(img, juce::Image::BitmapData::ReadWriteMode::readOnly);
      int bitCount = 32;
      int stride = (((width * bitCount) + 31) & ~31) >> 3;
      HGLOBAL mem = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT, sizeof(BITMAPINFO) + stride * height);
      if (!mem) {
        return false;
      }
      BITMAPINFO *info = (BITMAPINFO *)::GlobalLock(mem);
      if (!info) {
        ::GlobalFree(mem);
        return false;
      }
      info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
      info->bmiHeader.biWidth = width;
      info->bmiHeader.biHeight = height;
      info->bmiHeader.biPlanes = 1;
      info->bmiHeader.biBitCount = bitCount;
      info->bmiHeader.biCompression = BI_RGB;
      info->bmiHeader.biSizeImage = stride * height;
      info->bmiHeader.biXPelsPerMeter = 0;
      info->bmiHeader.biYPelsPerMeter = 0;
      info->bmiHeader.biClrUsed = 0;
      info->bmiHeader.biClrImportant = 0;
      uint8_t *dst = (uint8_t *)info + sizeof(BITMAPINFOHEADER);
      for (int y = 0; y < height; y++) {
        uint8_t *linePtr = bitmapData.getLinePointer(height - y - 1);
        memcpy(dst + y * stride, linePtr, std::min(stride, bitmapData.lineStride));
      }
      ::GlobalUnlock(mem);
      return (bool)::SetClipboardData(CF_DIB, mem);
    }
    }
#endif
#if JUCE_MAC
    switch (type) {
    case Type::Png: {
      NSPasteboard *pb = [NSPasteboard generalPasteboard];
      [pb declareTypes:[NSArray arrayWithObject:NSPasteboardTypePNG] owner:nil];
      NSData *ns = [NSData dataWithBytes:data.data() length:data.size()];
      [pb setData:ns forType:NSPasteboardTypePNG];
      return true;
    }
    case Type::Pdf: {
      NSPasteboard *pb = [NSPasteboard generalPasteboard];
      [pb declareTypes:[NSArray arrayWithObject:NSPasteboardTypePDF] owner:nil];
      NSData *ns = [NSData dataWithBytes:data.data() length:data.size()];
      [pb setData:ns forType:NSPasteboardTypePDF];
      return true;
    }
    }
#endif
    return false;
  }
};

} // namespace ksesh
