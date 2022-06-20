// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OPENCV_LIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OPENCV_LIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef OPENCV_LIB__H
#define OPENCV_LIB__H
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video.hpp"
#include "opencv2/imgcodecs.hpp"
#include <gst/gst.h>

#if defined(_MSC_VER)	//  Microsoft 
#define LIB_API __declspec(dllexport)
#define EXPORT	LIB_API
#define IMPORT __declspec(dllimport)

#if defined (_M_X64) || defined(_M_AMD64) || defined(_M_ARM64)
typedef uint64 intptr;
#else
typedef int intptr;
#endif

#elif defined(__GNUC__)	//  GCC
#define LIB_API __attribute__((visibility("default")))
#define EXPORT	LIB_API
#define IMPORT

#if defined(__aarch64__) || defined(__x64__) || defined(__x86_64__)
typedef uint64 intptr;
#else
typedef int intptr;
#endif

#else
//  do nothing and hope for the best?
#define LIB_API
//#define IMPORT
typedef int intptr;
#pragma warning Unknown dynamic link import/export semantics.
#endif

#ifdef __cplusplus
extern "C" {
#endif

EXPORT void setup(void * ctx, void * iface);


#ifdef __cplusplus
}			//extern "C"
#endif		
#endif		//OPENCV_LIB__H
