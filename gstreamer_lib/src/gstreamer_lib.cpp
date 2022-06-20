// dllmain.cpp : Defines the entry point for the DLL application.
//#include "stdafx.h"
#include "gstreamer_lib.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video.hpp"
#include "opencv2/imgcodecs.hpp"
#include "orbweaver.h"
#include <gst/gst.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


//#define EXPORT		
#ifdef __cplusplus
extern "C" {
#endif

	void gst_interface_release(void *, void * ptr) {
		if (ptr != NULL) {
			void * iface = ((void **)ptr)[0];
			if (iface == NULL) return;
			printf("gstreamer unref %ld\n", iface);
			gst_object_unref(iface);
		}
	}

	void * gst_elem_set_state(void * ctx, void * vgst, int state) {
		GstElement * pipeline = (GstElement *)vgst;
		gst_element_set_state(pipeline, (GstState)state);
		return vm_create_object(ctx, 0, NULL);
	}

	void * gst_elem_set(void * ctx, void * vgst, char * name, char * value) {
		GstElement * pipeline = (GstElement *)vgst;
		g_object_set(pipeline, name, value);
		return vm_create_object(ctx, 0, NULL);
	}

	void * gst_elem_get_bus(void * ctx, void * vgst, int state) {
		GstBus * bus;
		GstElement * pipeline = (GstElement *)vgst;
		bus = gst_element_get_bus (pipeline);
		return vm_create_interface(ctx, vm_select_interface(ctx, "GstBus"), bus, gst_interface_release);
	}

	void * gst_pipeline_add(void * ctx, void * vgst, void * elem) {
		GstElement * pipeline = (GstElement *)vgst;
		gst_bin_add (GST_BIN(pipeline), (GstElement *)elem);
		return vm_create_object(ctx, 0, NULL);
	}

	void * gst_pipeline_remove(void * ctx, void * vgst, void * elem) {
		GstElement * pipeline = (GstElement *)vgst;
		gst_bin_remove (GST_BIN(pipeline), (GstElement *)elem);
		return vm_create_object(ctx, 0, NULL);
	}

	void * gst_pipeline_find(void * ctx, void * vgst, char * name) {
		GstElement * elem;
		GstElement * pipeline = (GstElement *)vgst;
		elem = gst_bin_get_by_name (GST_BIN(pipeline), name);
		return vm_create_interface(ctx, vm_select_interface(ctx, "GstElement"), elem, gst_interface_release);
	}

	int gst_pipeline_start(void * ctx, void * vgst) {
  		GstBus *bus;
  		GstMessage *msg;
  		GstStateChangeReturn ret;
  		gboolean terminate = FALSE;
		GstElement * pipeline = (GstElement *)vgst;
		/* Start playing */
		//while(1);
		printf("gst_element_set_state %ld\n", pipeline);
		ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
		printf("gst_element_set_state result : %d\n", ret);
		
		if (ret == GST_STATE_CHANGE_FAILURE) {
		    g_printerr ("Unable to set the pipeline to the playing state.\n");
		    gst_object_unref (pipeline);
		    return -1;
		}
		/* Listen to the bus */
		printf("gst_element_get_bus\n");
		bus = gst_element_get_bus (pipeline);
		printf("gst_bus_timed_pop_filtered %ld\n", bus);
#if 1
		//wait async state change
		if (ret == GST_STATE_CHANGE_ASYNC) {
			printf("wait async state changed %ld\n", bus);
			
			msg = gst_bus_timed_pop_filtered (bus, 10 * GST_SECOND,
     				(GstMessageType)(GST_MESSAGE_ASYNC_DONE | GST_MESSAGE_ERROR));
			if (msg != NULL)
    				gst_message_unref (msg);
		}

		printf("wait eos %ld\n", bus);
		msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
			GstMessageType(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
		if (msg != NULL)
    			gst_message_unref (msg);
#else
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
#endif
		printf("gst_object_unref\n");
  		/* Free resources */
  		gst_object_unref (bus);
  		gst_element_set_state (pipeline, GST_STATE_NULL);
  		//gst_object_unref (pipeline);
		return 0;
	}

	EXPORT void * Plugin(void * ctx, void * ifx, char * name, char * type) {
		GstElement * element;
		element = gst_element_factory_make(name, type);
		return vm_create_interface(ctx, vm_select_interface(ctx, "GstElement"), element, gst_interface_release);
	}

	EXPORT void * Pipeline(void * ctx, void * ifx, char * name) {
		GstElement * pipeline;
		pipeline = gst_pipeline_new(name);
		return vm_create_interface(ctx, vm_select_interface(ctx, "GstElement"), pipeline, gst_interface_release);
	}

	EXPORT void * PipelineFromString(void * ctx, void * ifx, char * params) {
		GstElement * pipeline;
		GError * err;
		printf("PipelineFromString(%s)\n", params);
		//pipeline = gst_parse_launch(params, NULL);
		pipeline = gst_parse_launch_full (params,
                       gst_parse_context_new (),
                       (GstParseFlags )GST_PARSE_FLAG_NONE,
                       NULL);
		if(pipeline == NULL) {
			return vm_create_object(ctx, 0, NULL);
		}
		printf("gstreamer new pipeline %ld\n", pipeline);
		return vm_create_interface(ctx, vm_select_interface(ctx, "GstElement"), pipeline, gst_interface_release);
	}

EXPORT int
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
	EXPORT void setup(void * ctx, void * ifx) {
		pk_object_p iface;
		//printf("setup\n");
		int argc = 0;
		char * argv[] = {NULL, NULL};
		//main1(0, NULL);
		gst_init (0, NULL);
		iface = vm_register_interface(ctx, "GstElement", NULL);
		vm_interface_add_method(ctx, iface, "set", VM_VAR, 2, CAST_FUNC(gst_elem_set_state), VM_STRING, VM_STRING);
		vm_interface_add_method(ctx, iface, "set_state", VM_VAR, 1, CAST_FUNC(gst_elem_set), VM_INT32);
		vm_interface_add_method(ctx, iface, "get_bus", VM_VAR, 0, CAST_FUNC(gst_elem_get_bus));
		vm_interface_add_method(ctx, iface, "add", VM_VAR, 1, CAST_FUNC(gst_pipeline_add), VM_VAR);
		vm_interface_add_method(ctx, iface, "remove", VM_VAR, 1, CAST_FUNC(gst_pipeline_remove), VM_VAR);
		vm_interface_add_method(ctx, iface, "find", VM_VAR, 1, CAST_FUNC(gst_pipeline_find), VM_STRING);
		vm_interface_add_method(ctx, iface, "start", VM_INT32, 0, CAST_FUNC(gst_pipeline_start));


		//iface = vm_register_interface(ctx, "GstBus");



		//vm_install_interface(ctx, iface);
		//printf("setup successful\n");
	}

#ifdef __cplusplus
}
#endif

