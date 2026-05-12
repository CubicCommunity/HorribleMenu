#pragma once

#ifdef GEODE_IS_WINDOWS
#ifdef HorribleMenu_EXPORTS
#define BRKD_HORRIBLE_API_DLL __declspec(dllexport)
#else
#define BRKD_HORRIBLE_API_DLL __declspec(dllimport)
#endif
#else
#ifdef HorribleMenu_EXPORTS
#define BRKD_HORRIBLE_API_DLL __attribute__((visibility("default")))
#else
#define BRKD_HORRIBLE_API_DLL
#endif
#endif