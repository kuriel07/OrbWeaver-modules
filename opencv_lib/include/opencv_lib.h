// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OPENCV_LIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OPENCV_LIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef OPENCV_LIB__H
#define OPENCV_LIB__H
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video.hpp"
#include "opencv2/imgcodecs.hpp"

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

typedef struct cv_array cv_array;
typedef struct cv_array {
	void * ptr;
	void * base;
	struct cv_array * elem_base;
	size_t elem_size;
	int current;
	int count;
} cv_array ;

#ifdef __cplusplus
extern "C" {
#endif

cv_array * cv_new_base_class(void * ptr, void * base, cv_array * elem_base, size_t elem_size, int current, int count);
cv_array * cv_array_clone(cv_array * array);
void cv_interface_array_release(void *, void * ptr);
void cv_interface_release(void *, void * ptr);
EXPORT int imshow(void * ctx, void * iface, char * name, void * mat);
EXPORT void * imread(void * ctx, void * iface, char * path);
EXPORT void * VideoCapture(void * ctx, void * iface, char * path);
EXPORT void * Rect(void * ctx, void * iface, int x, int y, int w, int h);
EXPORT void * Size(void * ctx, void * iface, int w, int h);
//OPENCV_LIB_API void * grab(void * ctx, void * cap);
EXPORT void * Mat(void * ctx, void * iface, int w, int h, int c, int emsz, void * data);
EXPORT int waitKey(void * ctx, void * iface, int ms);
EXPORT void destroyAllWindows(void * ctx, void * iface);
EXPORT void setup(void * ctx, void * iface);
EXPORT void test_callback(void * ctx, void * iface, void * func, char * str);

EXPORT void * CascadeClassifier(void * ctx, void * iface, char * xml_path) ;
EXPORT void * FacemarkLBF(void * ctx, void * iface, char * lbf_path);

void opencv_bio_setup(void * ctx, void * ifx) ;
#ifdef __cplusplus
}			//extern "C"
#endif		
#endif		//OPENCV_LIB__H
