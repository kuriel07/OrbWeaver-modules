// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OPENCV_LIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OPENCV_LIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef WTK_LIB__H
#define WTK_LIB__H
#include <libxml/parser.h>
#include <libxml/tree.h>

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

#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE		0
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define WTK_TYPE_WIDGET			0
#define WTK_TYPE_JS			1
#define WTK_TYPE_CSS			2
#define WTK_TYPE_JS_REF			9
#define WTK_TYPE_CSS_REF		10

typedef struct wtk_node wtk_node;
typedef struct wtk_header wtk_header;
typedef struct wtk_object wtk_object;
typedef struct wtk_script wtk_script;

typedef struct wtk_node {
	int type;
	struct wtk_node * parent;
	struct wtk_node * sibling;
	struct wtk_node * child;
	struct wtk_script * init_script;
} wtk_node;

typedef struct wtk_header {
	wtk_node base;
	int len;
	char content[1];
} wtk_header;

typedef struct wtk_object {
	wtk_node base;
	int id;
	char tag[48];
	char name[128];
	char ns[128];
	int len;
	char content[1];
} wtk_object;

typedef struct wtk_script {
	wtk_node base;
	int len;
	char content[1];
} wtk_script;

typedef struct wtk_context {
	char * content;
	wtk_header * headers;
	wtk_object * objects;
} wtk_context;


void wtk_node_release(wtk_node * node);
wtk_node * wtk_create_node(int type, size_t size);
wtk_node * wtk_node_add_sibling(wtk_node * node, wtk_node * sibling);
wtk_node * wtk_node_add_child(wtk_node * node, wtk_node * child);
wtk_header * wtk_create_header(int type, char * content);
wtk_object * wtk_create_object(int type, int id, char * tag, char * name, char * ns, char * content);
wtk_script * wtk_create_script(int type, char * content);
wtk_context * wtk_create_context(char * content);
wtk_object * wtk_parse_node(int index, char * ns, xmlNode * node) ;
wtk_object * wtk_find_object(wtk_object * object, char * name);
wtk_object * wtk_parse_from_xml(char * ns, xmlDoc * doc);


void wtklib_interface_release(void *, void * ptr);
//WtkWidget
void * wtklib_widget_show(void * ctx, void * vgst);
void * wtklib_widget_hide(void * ctx, void * vgst);
void * wtklib_widget_set(void * ctx, void * vgst, char * name, char * value);
void * wtklib_widget_get(void * ctx, void * vgst, char * name);
void * wtklib_widget_on_click(void * ctx, void * ifx, void * callback);
//WtkContainer
void * wtklib_container_add(void * ctx, void * vgst, void * obj);
void * wtklib_container_remove(void * ctx, void * vgst, void * obj);
//WtkWindow
void * wtklib_window_show(void * ctx, void * vgst);
void * wtklib_window_set_size(void * ctx, void * vgst, int width, int height);
//WtkButton
void * wtklib_button_on_click(void * ctx, void * ifx, void * callback);
void * wtklib_button_set_text(void * ctx, void * ifx, char * text);
void * wtklib_button_get_text(void * ctx, void * ifx);
//WtkBuilder
void * wtklib_builder_get_object(void * ctx, void * ifx, char * name);
void * wtklib_builder_generate_html(void * ctx, void * ifx) ;
void * wtklib_builder_generate_json(void * ctx, void * ifx) ;

EXPORT void setup(void * ctx, void * iface);

//private APIs implementatino
void wtk_init_headers(wtk_context * ctx); 

#ifdef __cplusplus
}			//extern "C"
#endif		
#endif		//GTK_LIB__H
