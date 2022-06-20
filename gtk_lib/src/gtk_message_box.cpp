
#include "gtk_lib.h"
#include "orbweaver.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <gtk/gtk.h>

		
#ifdef __cplusplus
extern "C" {
#endif

EXPORT int MessageBox(void * ctx, void * vgst, char* text, char* caption, int type)
{
    GtkWidget *dialog ;

    /* Instead of 0, use GTK_DIALOG_MODAL to get a modal dialog box */

    if (type & 0x01)
        dialog = gtk_message_dialog_new(GTK_WINDOW(vgst), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, text );
    else
        dialog = gtk_message_dialog_new(GTK_WINDOW(vgst), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, text );


    gtk_window_set_title(GTK_WINDOW(dialog), caption);
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy( GTK_WIDGET(dialog) );

    if (type & 0x01)
    {
        switch (result)
        {
        default:
        case GTK_RESPONSE_DELETE_EVENT:
        case GTK_RESPONSE_NO:
            return 0;
        case GTK_RESPONSE_YES:
            return 1;
        }
        return 1;
    } 
}

#ifdef __cplusplus
}
#endif
