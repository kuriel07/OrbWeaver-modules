
#include "gtk_lib.h"
#include "orbweaver.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <gtk/gtk.h>

		
#ifdef __cplusplus
extern "C" {
#endif

	void * gtklib_builder_get_object(void * ctx, void * ifx, char * name) {
		GtkBuilder * builder = (GtkBuilder *)ifx;
		GtkWidget * widget = (GtkWidget *)gtk_builder_get_object(builder, name);
		return vm_create_interface(ctx, vm_select_interface(ctx, "GtkWidget"), widget, gtklib_interface_release);
	}


#ifdef __cplusplus
}
#endif
