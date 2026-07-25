#pragma once

// Check if C++23 is supported. MSVC reports the language version through
// _MSVC_LANG rather than __cplusplus unless /Zc:__cplusplus is enabled.
constexpr int CPP_VERSION_CXX23 = 202302L;
#if defined(_MSC_VER) && !defined(__clang__)
static_assert(_MSVC_LANG >= CPP_VERSION_CXX23, "C++23 support is required");
#else
static_assert(__cplusplus >= CPP_VERSION_CXX23, "C++23 support is required");
#endif

#ifdef _WIN32
#ifdef NBN_EXPORT_DLL_core
#define nbn_export_core __declspec(dllexport)
#else
#define nbn_export_core __declspec(dllimport)
#endif
#else
#define nbn_export_core __attribute__((visibility("default")))
#endif  // _WIN32
