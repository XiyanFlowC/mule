#ifndef MULERT_API_H__
#define MULERT_API_H__

#ifdef MULERT_STATIC

#define MULERT_API

#else

#ifdef WIN32

#ifdef _MSC_VER

#ifdef MULERT_EXPORTS_DLL
#define MULERT_API __declspec(dllexport)
#else
#define MULERT_API __declspec(dllimport)
#endif

#elif defined(__GNUC__)

#ifdef MULERT_EXPORT_DLL
#define MULERT_API __attribute__(dllexport)
#else
#define MULERT_API __attribute__(dllimport)
#endif /*Windows, GCC*/

#endif

#elif defined(linux)

#if defined(__GNUC__)
#ifdef MULERT_EXPORT_DLL
#define MULERT_API __attribute__((visibility ("default")))
#else
#define MULERT_API
#endif
#endif /*Linux, GCC*/

#endif

#endif /*MULERT_STATIC*/

#endif
