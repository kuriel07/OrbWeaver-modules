// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OPENCV_LIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OPENCV_LIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef GTK_LIB__H
#define GTK_LIB__H

#if defined(_MSC_VER)	//  Microsoft 
#define LIB_API __declspec(dllexport)
#define EXPORT	LIB_API
#define IMPORT __declspec(dllimport)

#elif defined(__GNUC__)	//  GCC
#define LIB_API __attribute__((visibility("default")))
#define EXPORT	LIB_API
#define IMPORT

#else
//  do nothing and hope for the best?
#define LIB_API
//#define IMPORT
//typedef int intptr;
#pragma warning Unknown dynamic link import/export semantics.
#endif

#ifdef __cplusplus
extern "C" {
#endif
void gtklib_interface_release(void *, void * ptr);
//GtkWidget
void * gtklib_widget_show(void * ctx, void * vgst);
void * gtklib_widget_hide(void * ctx, void * vgst);
void * gtklib_widget_set(void * ctx, void * vgst, char * name, char * value);
void * gtklib_widget_get(void * ctx, void * vgst, char * name);
//GtkContainer
void * gtklib_container_add(void * ctx, void * vgst, void * obj);
void * gtklib_container_remove(void * ctx, void * vgst, void * obj);
//GtkWindow
void * gtklib_window_show(void * ctx, void * vgst);
void * gtklib_window_set_size(void * ctx, void * vgst, int width, int height);
//GtkButton
void * gtklib_button_on_click(void * ctx, void * ifx, void * callback);
void * gtklib_button_set_text(void * ctx, void * ifx, char * text);
void * gtklib_button_get_text(void * ctx, void * ifx);
//GtkBuilder
void * gtklib_builder_get_object(void * ctx, void * ifx, char * name);

EXPORT void setup(void * ctx, void * iface);


#ifdef __cplusplus
}			//extern "C"
#endif		
#endif		//GTK_LIB__H
