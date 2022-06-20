
#include "gtk_lib.h"
#include "orbweaver.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <gtk/gtk.h>

		
#ifdef __cplusplus
extern "C" {
#endif
	
	void * gtklib_widget_show(void * ctx, void * vgst) {
		GtkWidget * widget = (GtkWidget *)vgst;
		gtk_widget_show (widget);
		return vm_create_object(ctx, 0, NULL);
	}

	void * gtklib_widget_hide(void * ctx, void * vgst) {
		GtkWidget * widget = (GtkWidget *)vgst;
		gtk_widget_hide (widget);
		return vm_create_object(ctx, 0, NULL);
	}

	void * gtklib_widget_set(void * ctx, void * vgst, char * name, char * value) {
		GtkWidget * widget = (GtkWidget *)vgst;
		GValue gval = G_VALUE_INIT;
		g_value_set_string(&gval, (const gchar *)value);
		g_object_set_property ((GObject *)widget,
                       name,
                       &gval);
		return vm_create_object(ctx, 0, NULL);
	}

	void * gtklib_widget_get(void * ctx, void * vgst, char * name) {
		const gchar * value;
    		GtkWidget * widget = (GtkWidget *)vgst;
		GValue gval = G_VALUE_INIT;
		g_object_get_property ((GObject *)widget,
                       name,
                       &gval);
		value = g_value_get_string (&gval);
		return vm_create_object(ctx, strlen(value), (uchar *)value);
	}


#ifdef __cplusplus
}
#endif
