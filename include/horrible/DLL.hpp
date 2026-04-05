#pragma once

#ifdef GEODE_IS_WINDOWS
#ifdef BRKD_HORRIBLE_API_EXPORTING
#define BRKD_HORRIBLE_API_DLL __declspec(dllexport)
#else
#define BRKD_HORRIBLE_API_DLL __declspec(dllimport)
#endif
#else
#ifdef BRKD_HORRIBLE_API_EXPORTING
#define BRKD_HORRIBLE_API_DLL __attribute__((visibility("default")))
#else
#define BRKD_HORRIBLE_API_DLL
#endif
#endif