#pragma once

namespace ksesh {

class Clipboard {
public:
  enum class Type {
    Emf,
  };

  static bool Available(Type t) {
    switch (t) {
    case Type::Emf:
#if JUCE_WINDOWS
      return (bool)::IsClipboardFormatAvailable(CF_ENHMETAFILE);
#endif
      break;
    }
    return false;
  }

  static bool Store(std::string const &data, Type type) {
    if (!Available(type)) {
      return false;
    }
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
      memcpy(ptr, data.c_str(), data.size());
      HENHMETAFILE f = ::SetEnhMetaFileBits(data.size(), (BYTE *const)ptr);
      if (INVALID_HANDLE_VALUE == f) {
        ::GlobalUnlock(mem);
        ::GlobalFree(mem);
        return false;
      }
      ::GlobalUnlock(mem);
      return (bool)::SetClipboardData(CF_ENHMETAFILE, f);
    }
    }
#else
#endif
    return false;
  }
};

} // namespace ksesh
