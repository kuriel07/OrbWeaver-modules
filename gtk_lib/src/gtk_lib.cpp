// dllmain.cpp : Defines the entry point for the DLL application.
//#include "stdafx.h"
#include "gtk_lib.h"
#include "orbweaver.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <gtk/gtk.h>

//#define EXPORT		
#ifdef __cplusplus
extern "C" {
#endif

	void gtklib_interface_release(void *, void * ptr) {
		if (ptr != NULL) {
			void * iface = ((void **)ptr)[0];
			if (iface == NULL) return;
			//printf("gstreamer unref %ld\n", iface);
			g_object_unref(iface);
		}
	}

	static void gtklib_load_callback(GtkApplication *app, gpointer handler) {
#if 0
    		GtkWidget *window;

    		window = gtk_application_window_new(app);
    		gtk_window_set_title(GTK_WINDOW(window), "Hello GNOME");
    		gtk_widget_show_all(window);
#else
		void * ctx = vm_function_get_instance(handler);
		printf("ctx : %lld, callback : %lld\n", ctx, handler);
		printf("gtk_load_callback %lld\n", ctx);
		vm_object_p obj = vm_create_interface(ctx, vm_select_interface(ctx, "GtkApplication"), app, gtklib_interface_release);
		printf("vm_exec_callback\n");
		vm_exec_callback(ctx, handler, 1 , obj);
#endif
	}

	EXPORT void *  Run(void * ctx, void * vgst) {
		//GtkApplication * app = (GtkApplication *)vgst;
		//int status = g_application_run(G_APPLICATION(app), 0, NULL);
		gtk_main();
		//return status;
		//gboolean res = gtk_main_iteration_do (FALSE);
		//if(res == TRUE) return vm_load_bool(ctx, TRUE);
		return vm_load_bool(ctx, FALSE);
	}

	EXPORT void * Window(void * ctx, void * vgst, char * title) {
    		GtkWidget *window;
		//GtkApplication * app = (GtkApplication *)vgst;
		//window = gtk_application_window_new(app);
		window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    		gtk_window_set_title(GTK_WINDOW(window), title);
		/* It's a good idea to do this for all windows. */
    		g_signal_connect (window, "destroy",
                        G_CALLBACK (gtk_main_quit), NULL);
		return vm_create_interface(ctx, vm_select_interface(ctx, "GtkWindow"), window, gtklib_interface_release);
	}

	EXPORT void * Label(void * ctx, void * vgst, char * text) {
    		GtkWidget *label;
		//GtkApplication * app = (GtkApplication *)vgst;
		label = gtk_label_new (text);
    		//gtk_window_set_title(GTK_WINDOW(window), title);
		return vm_create_interface(ctx, vm_select_interface(ctx, "GtkLabel"), label, gtklib_interface_release);
	}

	EXPORT void * Button(void * ctx, void * vgst, char * text) {
    		GtkWidget *button;
		//GtkApplication * app = (GtkApplication *)vgst;
		button = gtk_button_new_with_label(text);
    		//gtk_window_set_title(GTK_WINDOW(window), title);
		return vm_create_interface(ctx, vm_select_interface(ctx, "GtkButton"), button, gtklib_interface_release);
	}

	EXPORT void * Application(void * ctx, void * ifx, char * name, void * callback) {
		GtkApplication * app;
		void * handler;
		app = gtk_application_new(name, G_APPLICATION_FLAGS_NONE);
		handler = vm_load_callback(ctx, callback);
		printf("ctx : %lld, callback : %lld\n", ctx, handler);
		g_signal_connect(app, "activate", G_CALLBACK(gtklib_load_callback), handler);
		return vm_create_interface(ctx, vm_select_interface(ctx, "GtkApplication"), app, gtklib_interface_release);
	}

	EXPORT void * BuilderFromFile(void * ctx, void * vgst, char * path) {
    		GtkBuilder *builder;
		builder = gtk_builder_new_from_file(path);
		return vm_create_interface(ctx, vm_select_interface(ctx, "GtkBuilder"), builder, gtklib_interface_release);
	}

	EXPORT void * BuilderFromString(void * ctx, void * vgst, char * content) {
    		GtkBuilder *builder;
		builder = gtk_builder_new_from_string(content, -1);
		return vm_create_interface(ctx, vm_select_interface(ctx, "GtkBuilder"), builder, gtklib_interface_release);
	}

	EXPORT void setup(void * ctx, void * ifx) {
		pk_object_p iface;
		int argc = 0;
		char * argv[] = {NULL, NULL};
		iface = vm_register_interface(ctx, "GtkApplication", NULL);
		//vm_interface_add_method(ctx, iface, "run", VM_INT32, 0, CAST_FUNC(gtklib_application_run));
		//vm_interface_add_method(ctx, iface, "window", VM_VAR, 1, CAST_FUNC(gtklib_application_new_window), VM_STRING);
		//vm_interface_add_method(ctx, iface, "label", VM_VAR, 1, CAST_FUNC(gtklib_application_new_label), VM_STRING);
		//vm_interface_add_method(ctx, iface, "button", VM_VAR, 1, CAST_FUNC(gtklib_application_new_button), VM_STRING);

		iface = vm_register_interface(ctx, "GtkBuilder", NULL);
		vm_interface_add_method(ctx, iface, "find", VM_VAR, 1, CAST_FUNC(gtklib_builder_get_object), VM_STRING);

		iface = vm_register_interface(ctx, "GtkWidget", NULL);
		vm_interface_add_method(ctx, iface, "show", VM_VAR, 0, CAST_FUNC(gtklib_widget_show));
		vm_interface_add_method(ctx, iface, "hide", VM_VAR, 0, CAST_FUNC(gtklib_widget_hide));
		vm_interface_add_method(ctx, iface, "set", VM_VAR, 2, CAST_FUNC(gtklib_widget_set), VM_STRING, VM_STRING);
		vm_interface_add_method(ctx, iface, "get", VM_VAR, 1, CAST_FUNC(gtklib_widget_get), VM_STRING);

		iface = vm_register_interface(ctx, "GtkContainer", vm_select_interface(ctx, "GtkWidget"));
		vm_interface_add_method(ctx, iface, "add", VM_VAR, 1, CAST_FUNC(gtklib_container_add), VM_VAR);
		vm_interface_add_method(ctx, iface, "remove", VM_VAR, 1, CAST_FUNC(gtklib_container_remove), VM_VAR);

		iface = vm_register_interface(ctx, "GtkWindow", vm_select_interface(ctx, "GtkContainer"));
		vm_interface_add_method(ctx, iface, "show", VM_VAR, 0, CAST_FUNC(gtklib_window_show));		//override method
		vm_interface_add_method(ctx, iface, "set_size", VM_VAR, 2, CAST_FUNC(gtklib_window_set_size), VM_INT32, VM_INT32);	

		iface = vm_register_interface(ctx, "GtkButton", vm_select_interface(ctx, "GtkContainer"));
		vm_interface_add_method(ctx, iface, "on_click", VM_VAR, 1, CAST_FUNC(gtklib_button_on_click), VM_FUNCTION);
		vm_interface_add_method(ctx, iface, "set_text", VM_VAR, 1, CAST_FUNC(gtklib_button_set_text), VM_STRING);
		vm_interface_add_method(ctx, iface, "get_text", VM_VAR, 0, CAST_FUNC(gtklib_button_get_text));

		iface = vm_register_interface(ctx, "GtkLabel", vm_select_interface(ctx, "GtkWidget"));

		gtk_init (&argc, NULL);
	}

#ifdef __cplusplus
}
#endif

