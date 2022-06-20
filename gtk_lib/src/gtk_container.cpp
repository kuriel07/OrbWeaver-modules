
#include "gtk_lib.h"
#include "orbweaver.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <gtk/gtk.h>

		
#ifdef __cplusplus
extern "C" {
#endif
	void * gtklib_container_add(void * ctx, void * vgst, void * obj) {
		GtkContainer * container = (GtkContainer *)vgst;
		printf("container add\n");
		gtk_container_add(GTK_CONTAINER (container), (GtkWidget *)obj);
		return vm_create_object(ctx, 0, NULL);
	}

	void * gtklib_container_remove(void * ctx, void * vgst, void * obj) {
		GtkContainer * container = (GtkContainer *)vgst;
		gtk_container_remove(GTK_CONTAINER (container), (GtkWidget *)obj);
		return vm_create_object(ctx, 0, NULL);
	}


#ifdef __cplusplus
}
#endif
