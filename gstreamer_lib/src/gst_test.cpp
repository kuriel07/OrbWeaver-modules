#include <gst/gst.h>

#if defined(__GNUC__)	//  GCC
#define LIB_API __attribute__((visibility("default")))
#endif

#if defined(__cplusplus)
extern "C" {
#endif

__attribute__((visibility("default"))) int
host_main (int argc, char *argv[])
{
  GstElement *pipeline;
  GstBus *bus;
  GstMessage *msg;
  gboolean terminate = FALSE;

  /* Initialize GStreamer */
  gst_init (0, NULL);
  gst_init (0, NULL);

  /* Build the pipeline */
  pipeline =
      gst_parse_launch
      ("playbin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm",
      NULL);

  /* Start playing */
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  /* Wait until error or EOS */
  bus = gst_element_get_bus (pipeline);
  do {
    msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
	(GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
	/* Parse message */
	if (msg != NULL) {
      		GError *err;
     		gchar *debug_info;

		switch (GST_MESSAGE_TYPE (msg)) {
			case GST_MESSAGE_ERROR:
	  		gst_message_parse_error (msg, &err, &debug_info);
	  		g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
	  		g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
	  		g_clear_error (&err);
	  		g_free (debug_info);
	  		terminate = TRUE;
	  		break;
			case GST_MESSAGE_EOS:
	  		g_print ("End-Of-Stream reached.\n");
	  		terminate = TRUE;
	  		break;
			case GST_MESSAGE_STATE_CHANGED:
	  		/* We are only interested in state-changed messages from the pipeline */
	  		if (GST_MESSAGE_SRC (msg) == GST_OBJECT (pipeline)) {
	    			GstState old_state, new_state, pending_state;
	    			gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
	    			g_print ("Pipeline state changed from %s to %s:\n",
				gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
	  		}
	  		break;
			default:
	  		/* We should not reach here */
	  		g_printerr ("Unexpected message received.\n");
	  		break;
		}
		gst_message_unref (msg);
	}
  } while (!terminate);

  /* Free resources */
  if (msg != NULL)
    gst_message_unref (msg);
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  return 0;
}


#if defined(__cplusplus)
}
#endif

