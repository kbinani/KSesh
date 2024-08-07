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
#if defined(JUCE_WINDOWS)
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
      HENHMETAFILE f = ::SetEnhMetaFileBits(data.size(), (BYTE *const)data.data());
      if (INVALID_HANDLE_VALUE == f) {
        return false;
      }
      if (::SetClipboardData(CF_ENHMETAFILE, f)) {
        return true;
      } else {
        ::DeleteEnhMetaFile(f);
        return false;
      }
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
      BITMAPINFOHEADER *header = (BITMAPINFOHEADER *)::GlobalLock(mem);
      if (!header) {
        ::GlobalFree(mem);
        return false;
      }
      header->biSize = sizeof(BITMAPINFOHEADER);
      header->biWidth = width;
      header->biHeight = height;
      header->biPlanes = 1;
      header->biBitCount = bitCount;
      header->biCompression = BI_RGB;
      header->biSizeImage = stride * height;
      uint8_t *dst = (uint8_t *)header + sizeof(BITMAPINFOHEADER);
      for (int y = 0; y < height; y++) {
        uint8_t *linePtr = bitmapData.getLinePointer(height - y - 1);
        memcpy(dst + y * stride, linePtr, std::min(stride, bitmapData.lineStride));
      }
      ::GlobalUnlock(mem);
      if (::SetClipboardData(CF_DIB, mem)) {
        return true;
      } else {
        ::GlobalFree(mem);
        return false;
      }
    }
    }
#endif
#if defined(JUCE_MAC)
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
