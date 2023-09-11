#pragma once

#ifndef XY_API_H__
#define XY_API_H__

#ifndef XY_DYNAMIC_LIBRARY

#define XY_API

#else

#ifdef WIN32

#ifdef _MSC_VER

#ifdef XY_EXPORT_DLL
#define XY_API __declspec(dllexport)
#else
#define XY_API __declspec(dllimport)
#endif

#elif defined(__GNUC__)

#ifdef XY_EXPORT_DLL
#define XY_API __attribute__(dllexport)
#else
#define XY_API __attribute__(dllimport)
#endif /*Windows, GCC*/

#endif

#elif defined(linux)

#if defined(__GNUC__)
#ifdef XY_EXPORT_DLL
#define XY_API __attribute__((visibility ("default")))
#else
#define XY_API
#endif
#endif /*Linux, GCC*/

#endif

#endif /*XY_STATIC*/

#endif
