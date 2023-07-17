#ifndef RIME_SHARE_LIBRARY_
#define RIME_SHARE_LIBRARY_

#include <dlfcn.h>
#include <stdexcept>
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#endif

#include <rime/common.h>

namespace rime {

class SharedLibrary {
public:
  // creates an instance that does not reference any DLL/DSO.
  SharedLibrary(): handle_(nullptr), filename_(nullptr) {}
  SharedLibrary(string filename): handle_(nullptr), filename_(std::move(filename)) {
  // load shared library
#if defined(_WIN32)
    handle_ = LoadLibraryA(m_filename.c_str());
#else
    handle_ = dlopen(filename_.c_str(), RTLD_LAZY);
#endif
    if (!handle_) {
        throw std::runtime_error(dlerror());
    }
  }

  ~SharedLibrary() {
    // unload shared library
    if (handle_) {
#if defined(_WIN32)
      FreeLibrary(m_handle);
#else
      dlclose(handle_);
#endif
    }
  }

  bool is_loaded() const noexcept {
    return handle_ != nullptr;
  }

  void* symbol(string name) const {
    if (!handle_) {
      return nullptr;
    }
#if defined(_WIN32)
    return reinterpret_cast<void*>(GetProcAddress(handle_, name.c_str()));
#else
    return dlsym(handle_, name.c_str());
#endif
  }
  
  static std::filesystem::path suffix() {
    return
#if defined(_WIN32)
      L".dll";
#elif defined(__APPLE__)
      ".dylib";
#else
      ".so";
#endif
  }

private:
#if defined(_WIN32)
    HMODULE handle_;
#else
    void* handle_;
#endif
    string filename_;
};

}

#endif // RIME_SHARE_LIBRARY_
