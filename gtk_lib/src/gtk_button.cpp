
#include "gtk_lib.h"
#include "orbweaver.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <gtk/gtk.h>

		
#ifdef __cplusplus
extern "C" {
#endif
	
	static void gtklib_button_click(GtkButton *button, gpointer handler) {
		void * ctx = vm_function_get_instance(handler);
		printf("button clicked\n");
		vm_object_p obj = vm_create_interface(ctx, vm_select_interface(ctx, "GtkButton"), button, gtklib_interface_release);
		vm_exec_callback(ctx, handler, 1 , obj);
	}

	void * gtklib_button_on_click(void * ctx, void * ifx, void * callback) {
		GtkWidget * button;
		void * handler;
		button = (GtkWidget * )ifx;
		handler = vm_load_callback(ctx, callback);
		g_signal_connect(button, "clicked", G_CALLBACK(gtklib_button_click), handler);
		//return vm_create_interface(ctx, vm_select_interface(ctx, "GtkApplication"), app, gtklib_interface_release);
		return vm_create_object(ctx, 0, NULL);
	}

	void * gtklib_button_set_text(void * ctx, void * ifx, char * text) {
		GtkWidget * button = (GtkWidget *)ifx;
		gtk_button_set_label(GTK_BUTTON(button), text);
		return vm_create_object(ctx, 0, NULL);
	}

	void * gtklib_button_get_text(void * ctx, void * ifx) {
		GtkWidget * button = (GtkWidget *)ifx;
		const gchar * text = gtk_button_get_label(GTK_BUTTON(button));
		return vm_create_object(ctx, strlen(text), (uchar *)text);
	}


#ifdef __cplusplus
}
#endif
