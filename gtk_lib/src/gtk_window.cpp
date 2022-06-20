
#include "gtk_lib.h"
#include "orbweaver.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <gtk/gtk.h>

		
#ifdef __cplusplus
extern "C" {
#endif

	void * gtklib_window_show(void * ctx, void * vgst) {
    		GtkWidget *window = (GtkWidget *)vgst;
    		gtk_widget_show_all(window);
		return vm_create_object(ctx, 0, NULL);
	}

	void * gtklib_window_set_size(void * ctx, void * vgst, int width, int height) {
    		GtkWidget *window = (GtkWidget *)vgst;
    		gtk_window_set_default_size(GTK_WINDOW(window), width, height);
		return vm_create_object(ctx, 0, NULL);
	}

#ifdef __cplusplus
}
#endif
