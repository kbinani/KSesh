#pragma once

namespace ksesh {

#if defined(JUCE_WINDOWS)
template <class Handle, auto *deleter>
struct ScopedHANDLE {
  explicit ScopedHANDLE(Handle handle) : fHandle(handle) {
  }

  ~ScopedHANDLE() {
    deleter(fHandle);
  }

  operator Handle() const {
    return fHandle;
  }

  operator bool() const {
    return fHandle != INVALID_HANDLE_VALUE;
  }

private:
  Handle fHandle;
};
#endif

} // namespace ksesh
