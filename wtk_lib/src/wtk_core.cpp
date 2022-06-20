// dllmain.cpp : Defines the entry point for the DLL application.
//#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "orbweaver.h"
#include "wtk.h"
#include <json-c/json.h>
#include "mustach-json-c.h"
#include <time.h>

//#define EXPORT		
#ifdef __cplusplus
extern "C" {
#endif
	static char * wtk_read_all_file(char * path) {
		int len = 0; 
		char * content = NULL;
		FILE * file = fopen(path, "rb");
		if(file != NULL) {
			fseek(file, 0, SEEK_END);
			len = ftell(file);
			if(len == 0) return NULL;
			content = (char *)malloc(len + 1);
			fseek(file, 0, SEEK_SET);
			fread(content, 1, len, file);	
			content[len] = 0;
			fclose(file);
			printf("content : %s\n", content);
		} else {
			printf("file not found\n");
		}
		return content;
	}

	static const char bin2hexchar[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
	static uint32 wtk_bin2hex(uint8 * bytes, uint32 len, uint8 * hexstring) {
		uint32 j;
		for(j=0;j<len;j++) {
			hexstring[j << 1] = bin2hexchar[bytes[j] >> 4];
			hexstring[(j << 1) + 1] = bin2hexchar[bytes[j] & 0x0F];
		}
		hexstring[len << 1] = 0;
		return len << 1;
	}

	static int wtk_generate_unique_name(char * name, int len) {
		//generate unique null terminated string with specified length
		static int nonce = 0;
		timespec wtime ;
		char temp[64];
		int sz =0;
		memset(name, 0, len);
#if defined(WIN32) || defined(WIN64)
		TIME_ZONE_INFORMATION tz_info;
		time_t t;
		datetime dt;
		tm *tp;
		char bbuf[100];
		timespec_get(&wtime, TIME_UTC);
		time(&t);
		tp = localtime(&t);
		dt.hour = tp->tm_hour;
		dt.minute = tp->tm_min;
		dt.second = tp->tm_sec;
		dt.date = tp->tm_mday;
		dt.month = (tp->tm_mon + 1);
		dt.year = (1900 + tp->tm_year) ;
		dt.days = tp->tm_yday;
		dt.day = tp->tm_wday;
		GetTimeZoneInformation(&tz_info);
		dt.tz = 0 - (tz_info.Bias / 60);
		//dt.tz = 0;
		//util_iso8601_encode(&dt, bbuf);
		wtime.tv_sec = util_iso8601_ffito_filetime(&dt);
		//strftime(bbuf, sizeof(bbuf), "%D %T\n", gmtime(&wtime.tv_sec));
		//printf(bbuf);
#else
		clock_gettime(CLOCK_REALTIME, &wtime);
#endif
		//[random bytes:28][nonce:4]
		memcpy(temp, &wtime, sizeof(timespec));
		memcpy(temp + sizeof(timespec), &nonce, sizeof(int));
		nonce++;
		sz = sizeof(timespec) + sizeof(int);
		wtk_bin2hex((uint8 *)temp, sz, (uint8 *)name);
		return sz;
	}

	void wtklib_interface_release(void *, void * ptr) {
		if (ptr != NULL) {
			void * iface = ((void **)ptr)[0];
			if (iface == NULL) return;
			//printf("gstreamer unref %ld\n", iface);
			//g_object_unref(iface);
			free(iface);
		}
	}

	void wtk_context_interface_release(void *, void * ptr) {
		if (ptr != NULL) {
			wtk_context * iface = ((wtk_context **)ptr)[0];
			if (iface == NULL) return;
			//printf("gstreamer unref %ld\n", iface);
			//g_object_unref(iface);
			//xmlFreeDoc(iface->xdoc);		//free xml tree
			wtk_node_release((wtk_node *)iface->headers);
			wtk_node_release((wtk_node *)iface->objects);
			free(iface->content);
			free(iface);
		}
	}

	void wtk_node_release(wtk_node * node) {
		wtk_node * iterator = node;
		wtk_node * candidate = node;
		if(iterator == NULL) return;
		if(iterator->child != NULL) {
			wtk_node_release(iterator->child);
			iterator->child = NULL;
		}
		while(iterator != NULL) {
			candidate = iterator;			
			iterator = iterator->sibling;
			wtk_node_release((wtk_node *)candidate->init_script);
			free(candidate);
		}
	}

	wtk_node * wtk_create_node(int type, size_t size) {
		wtk_node * node;
		if(size < sizeof(wtk_node)) return NULL;
		node = (wtk_node * )malloc(size);
		node->type = type;
		node->parent = NULL;
		node->sibling = NULL;
		node->child = NULL;
		node->init_script = NULL;
		return node;
	}

	wtk_node * wtk_node_add_sibling(wtk_node * node, wtk_node * sibling) {
		wtk_node * iterator = node;		
		if(iterator == NULL) return NULL;
		while(iterator->sibling != NULL) iterator = iterator->sibling;
		iterator->sibling = sibling;
		sibling->parent = iterator->parent;		//set to the same parent as last node
		return sibling; 
	}

	wtk_node * wtk_node_add_child(wtk_node * node, wtk_node * child) {
		wtk_node * iterator = node;		
		if(iterator == NULL) return NULL;
		if(iterator->child == NULL) {
			iterator->child = child;		//add to child node
			child->parent = iterator;		//set parent
		} else {
			return wtk_node_add_sibling(iterator->child, child);
		}
		return child;
	}

	wtk_header * wtk_create_header(int type, char * content) {
		wtk_header * header = (wtk_header *)wtk_create_node(type, sizeof(wtk_header) + strlen(content) + 1); 
		if(header != NULL) {
			header->len = strlen(content);
			memcpy(header->content, content, header->len);
			header->content[header->len] = 0;		//null string
		}
		return header;
	}

	wtk_object * wtk_create_object(int type, int id, char * tag, char * name, char * ns, char * content) {
		wtk_object * obj = (wtk_object *)wtk_create_node(type, sizeof(wtk_object) + strlen(content)); 
		if(obj != NULL) {
			strncpy(obj->name, name, 127);		//object name
			strncpy(obj->ns, ns, 127);		//object namespace
			strncpy(obj->tag, tag, 47);		//object xml tag
			obj->id = id;
		}
		return obj;
	}

	wtk_script * wtk_create_script(int type, char * content) {
		wtk_script * script = (wtk_script *)wtk_create_node(type, sizeof(wtk_script) + strlen(content) + 1); 
		if(script != NULL) {
			script->len = strlen(content);
			memcpy(script->content, content, script->len);
			script->content[script->len] = 0;		//null string
		}
		return script;
	}

	wtk_context * wtk_create_context(char * content) {
		wtk_context * ctx ;
		int len;
		char ns_name[128];
		//convert content to corresponding xml tree, then convert it to wtk_object tree for future reference
		xmlDocPtr doc = xmlReadMemory(content, strlen(content), "noname.xml", NULL, 0);
	    	if (doc == NULL) {
        		fprintf(stderr, "Failed to parse document\n");
			return NULL;
	    	}
		ctx = (wtk_context *)malloc(sizeof(wtk_context ));
		if(ctx != NULL) {
			len = strlen(content);
			printf("wtk_generate_unique_name\n");
			wtk_generate_unique_name(ns_name, 64);
			printf("wtk_generate_unique_name %s\n", ns_name);
			ctx->content = (char *)malloc(len + 1);
			ctx->headers = NULL;
			printf("wtk_parse_from_xml\n");
			ctx->objects = wtk_parse_from_xml(ns_name, doc);
			memcpy(ctx->content, content, len);
			ctx->content[len] = 0;
			printf("wtk_init_headers\n");
			wtk_init_headers(ctx);
			printf("wtk_init_headers done\n");
			if(ctx->objects != NULL) strncpy(ctx->objects->name, ns_name, 128);		//copy namespace
			else printf("no object\n");
			//wtk_generate_unique_name(ctx->objects->name, 64);
			//generate name for root object, used as reference to the view on the client side
		}
		printf("xmlFreeDoc\n");
		xmlFreeDoc(doc);		//release xml tree (no longer used)	
		return ctx;
	}

	wtk_object * wtk_parse_node(int index, char * ns, xmlNode * node) {
		xmlNode *cur_node = NULL;
		wtk_object * object = NULL;
		wtk_object * childs = NULL;
		wtk_object * prev = NULL;
		wtk_object * root = NULL;
		if(node == NULL) return NULL;
		printf("node : %lx\n", node);
	    	for (cur_node = node; cur_node; cur_node = cur_node->next) {
			if (cur_node->type == XML_ELEMENT_NODE) {
		    		printf("node type: Element, name: %s\n", cur_node->name);
				xmlAttr* attribute = cur_node->properties;
				object = wtk_create_object(WTK_TYPE_WIDGET, index++, (char *)cur_node->name, (char *)"", (char *)ns, (char *)"");
				if(root == NULL) root = object;
				while(attribute)
				{
					if(strcmp((const char *)attribute->name, "id") == 0) {
  						xmlChar* value = xmlNodeListGetString(node->doc, attribute->children, 1);
						strncpy(object->name, (const char *)value, 127);
	  					xmlFree(value); 
					}
  					attribute = attribute->next;
				}
				//if(childs != NULL && object == NULL) {
				//	object = wtk_create_object(WTK_TYPE_WIDGET, index++, (char *)cur_node->name, "", (char *)ns, "");
				//}
				if(cur_node->children != NULL) {
					printf("parse child\n");
					childs = wtk_parse_node(index++, ns, cur_node->children);
					if(object != NULL && childs != NULL) wtk_node_add_child((wtk_node *)object, (wtk_node *)childs);
				}
				if(prev != NULL) wtk_node_add_sibling((wtk_node *)prev, (wtk_node *)object);
				prev = object;
			}
			
	    	}
		return root;
	}

	wtk_object * wtk_find_root(wtk_object * object) {
		wtk_node * iterator = (wtk_node *)object;
		if(iterator == NULL) return NULL;
		while(iterator->parent != NULL) iterator = iterator->parent;
		return (wtk_object *)iterator;
	}

	wtk_object * wtk_find_object(wtk_object * object, char * name) {
		wtk_object * ret ;
		wtk_node * iterator = (wtk_node *)object;
		if(iterator == NULL) return NULL;
		while(iterator != NULL) {
			if(strncmp(((wtk_object *)iterator)->name, name, 128) == 0) {
				return (wtk_object *)iterator;
			}
			//traverse child nodes
			if(iterator->child != NULL) {
				ret = wtk_find_object((wtk_object *)iterator->child, name);
				if(ret != NULL) return ret;
			}
			iterator = iterator->sibling;
		}
		return (wtk_object *)iterator;
	}

	wtk_object * wtk_parse_from_xml(char * ns, xmlDoc * doc) {
		wtk_object * root;    		
		xmlNode *root_element = NULL;
	    	/*Get the root element node */
	    	root_element = xmlDocGetRootElement(doc);
	    	root = wtk_parse_node(0, ns, root_element);
		return root;
	}

	static void wtklib_load_callback(void *app, void * handler) {
#if 0
    		GtkWidget *window;

    		window = gtk_application_window_new(app);
    		gtk_window_set_title(GTK_WINDOW(window), "Hello GNOME");
    		gtk_widget_show_all(window);
#else
		void * ctx = vm_function_get_instance(handler);
		printf("ctx : %lld, callback : %lld\n", ctx, handler);
		printf("gtk_load_callback %lld\n", ctx);
		vm_object_p obj = vm_create_interface(ctx, vm_select_interface(ctx, "WtkApplication"), app, wtklib_interface_release);
		printf("vm_exec_callback\n");
		vm_exec_callback(ctx, handler, 1 , obj);
#endif
	}

	EXPORT void *  Run(void * ctx, void * vgst) {
		//gtk_main();
		return vm_load_bool(ctx, FALSE);
	}

/*
	EXPORT void * BuilderFromTemplate(void * ctx, void * vgst, char * tempstr, dal_result * result) {
		xmlDocPtr doc; 
		int s;
		char * content;
		size_t szl;
		struct json_object * rows = NULL;
		struct json_object * record;
		wtk_context * wctx = NULL;
		dal_array * arr;
		dal_column * column_enum;
		dal_record * row_iterator;
		dal_data * data_iterator;
		arr = (dal_array *)result->records;
		//parse dal_result to json_object
		//json_object* json_object_new_array(void);
		//int json_object_object_add(struct json_object* obj, const char *key, struct json_object *val);
		//int json_object_array_add(struct json_object *obj, struct json_object *val);
		//struct json_object* json_object_new_object(void);
		//struct json_object* json_object_new_boolean(json_bool b);
		//struct json_object* json_object_new_int64(int64_t i)
		//struct json_object* json_object_new_double(double d)
		//struct json_object* json_object_new_string(const char *s)
		if(arr == NULL) { 
			arr = dal_fetch_all(result);		//must fetch all
		}
		if(arr != NULL) {
			rows = json_object_new_array();
			row_iterator = arr;
			while(row_iterator != NULL) {
				column_enum = result->columns;
				data_iterator = (dal_data *)row_iterator;
				record = json_object_new_object();
				while(data_iterator != NULL && column_enum != NULL) {
					json_object_object_add(record, (const char *)column_enum->name, json_object_new_string_len((const char *)data_iterator->data, data_iterator->length) );
					//printf("%s|", column_enum->name);
					//printf("%s|", data_iterator->data, data_iterator->length);
					data_iterator = (dal_data *)((dal_node *)data_iterator)->next;
					column_enum = (dal_column *)((dal_node *)column_enum)->next;
				}
				json_object_array_add(rows, record);
				//printf("\n");
				row_iterator = (dal_record *)((dal_node *)row_iterator)->next_node;
			}
		}
		//start template processing
		s = mustach_json_c(tempstr, rows, &content, &szl);
		if(rows != NULL) json_object_put(rows);		//decrement reference counter json-c
		wctx = wtk_create_context(content);
		return vm_create_interface(ctx, vm_select_interface(ctx, "WtkBuilder"), wctx, wtk_context_interface_release);
	}

	EXPORT void * BuilderFromTemplateFile(void * ctx, void * vgst, char * path, dal_result * result) {
		char * content = wtk_read_all_file(path);
		void * ret ;
		if(content == NULL) return vm_create_object(ctx, 0, NULL);
		ret = BuilderFromTemplate(ctx, vgst, content, result);
		free(content);
		return ret;
	}
*/
	EXPORT void * BuilderFromTemplate(void * ctx, void * vgst, char * tempstr, char * params) {
		xmlDocPtr doc; 
		int s;
		char * content;
		size_t szl;
		struct json_object * rows = NULL;
		wtk_context * wctx = NULL;
		//parse dal_result to json_object
		//json_object* json_object_new_array(void);
		//int json_object_object_add(struct json_object* obj, const char *key, struct json_object *val);
		//int json_object_array_add(struct json_object *obj, struct json_object *val);
		//struct json_object* json_object_new_object(void);
		//struct json_object* json_object_new_boolean(json_bool b);
		//struct json_object* json_object_new_int64(int64_t i)
		//struct json_object* json_object_new_double(double d)
		//struct json_object* json_object_new_string(const char *s)
		printf("BuilderFromTemplate\n");
		//char * jsonstring = vm_object_to_jsonstring(params);
		printf("parse jsonstring : %s\n", params);
		//printf("parse jsonstring : %s\n", "[name:\"hello\"]");
		rows = json_tokener_parse(params);
		//start template processing
		printf("mustach jsonstring\n");
		s = mustach_json_c(tempstr, rows, &content, &szl);
		printf("mustach jsonstring done\n");
		if(rows != NULL) json_object_put(rows);		//decrement reference counter json-c
		printf("wtk_create_context\n");
		wctx = wtk_create_context(content);
		printf("wtk_create_context done\n");
		//printf("release jsonstring\n");
		//vm_jsonstring_release(jsonstring);
		return vm_create_interface(ctx, vm_select_interface(ctx, "WtkBuilder"), wctx, wtk_context_interface_release);
	}

	EXPORT void * BuilderFromTemplateFile(void * ctx, void * vgst, char * path, char * params) {
		char * content = wtk_read_all_file(path);
		void * ret ;
		if(content == NULL) return vm_create_object(ctx, 0, NULL);
		ret = BuilderFromTemplate(ctx, vgst, content, params);
		free(content);
		return ret;
	}


	EXPORT void * BuilderFromString(void * ctx, void * vgst, char * content) {
		wtk_context * wctx = NULL;
		wctx = wtk_create_context(content);
		return vm_create_interface(ctx, vm_select_interface(ctx, "WtkBuilder"), wctx, wtk_context_interface_release);
	}

	EXPORT void * BuilderFromFile(void * ctx, void * vgst, char * path) {
		char * content = wtk_read_all_file(path);
		void * ret ;
		if(content == NULL) return vm_create_object(ctx, 0, NULL);
		ret = BuilderFromString(ctx, vgst, content);
		free(content);
		return ret;
	}

	static int wtk_generate_userdata_s(struct json_object * json, wtk_node * root, wtk_node * node) {
		wtk_node * iterator;
		wtk_script * scr;
		int est_len = 0;
		char temp_scr[238];
		struct json_object * record;
		struct json_object * arr;
		if(node == NULL) return NULL;
		iterator = (wtk_node *)node;
		while(iterator != NULL) {
			record = json_object_new_object();
			if(strcmp(((wtk_object *)iterator)->name, "") != 0) {
				json_object_object_add(record, "name", 
					json_object_new_string_len((const char *)((wtk_object *)iterator)->name, strlen((const char *)((wtk_object *)iterator)->name)) 
				);
				sprintf(temp_scr, "$(\"%s %s\").val()", ((wtk_object *)root)->name, ((wtk_object *)iterator)->name);
				json_object_object_add(record, "value", 
					json_object_new_string_len((const char *)temp_scr, strlen((const char *)temp_scr)) 
				);
			}
			if(iterator->child != NULL) {
				arr = json_object_new_array();
				est_len += wtk_generate_userdata_s(arr, root, iterator->child) ;
				json_object_object_add(record, "childs", arr);
			}
			json_object_array_add(json, record);
			iterator = iterator->sibling;
		}
		return est_len;
	}

	char * wtk_generate_client_userdata(wtk_object * object) {
		struct json_object * record;
		wtk_object * root;
		char * str;
		root = wtk_find_root(object);
		if(root == object || root == NULL || object == NULL) {
			str = (char *)malloc(5);
			sprintf(str, "{}");		//empty object
			return str;
		}
		record = json_object_new_object();
		wtk_generate_userdata_s(record, (wtk_node *)root, ((wtk_node *)root)->child);
		
		str = (char *)json_object_to_json_string(record);
		if(record != NULL) json_object_put(record);		//decrement reference counter
		return str;
	}

	void * wtklib_widget_on_click(void * ctx, void * ifx, void * func) {
		//put init script here
		//char sbuf[512];
		char * sbuf;
		void * callback;
		char * callback_name;
		char * url;
		char * userdata;
		wtk_object * object = (wtk_object *)ifx;
		printf("wtklib_widget_on_click\n");
		callback = vm_install_callback(ctx, func);
		callback_name = vm_get_callback_name(ctx, callback);
		url = vm_get_request_url(ctx);		//TODO : need to check for the url
		userdata = wtk_generate_client_userdata(object);
		sbuf = (char *)malloc(strlen(userdata) + 512);
		printf("vm_get_request_url : %s\n", url);
		//add js ajax callback event to corresponding callback
		sprintf(sbuf, "$(\"%s %s\").click(function(evt) { \
				$.ajax({	\
			  	url: \"%s/%s\",		\
				data: %s,			\
				method: \"POST\",	\
			  	context: $(this),		\
				dataType: \"json\"	\
				}).success(function(data) {	\
  					if(data.init != undefined) eval(data.init);\
				});\
			});", object->ns, object->name, url, callback_name, userdata);
		printf("init_script : %s\n", sbuf);
		if(((wtk_node *)object)->init_script == NULL) {
			((wtk_node *)object)->init_script = wtk_create_script(WTK_TYPE_JS, sbuf);
		}
		sprintf(sbuf, "$(\"%s %s\").attr(\"ns\", );", object->ns, object->name);
		return vm_create_object(ctx, 0, NULL);
	}

	EXPORT void setup(void * ctx, void * ifx) {
		pk_object_p iface;
		int argc = 0;
		char * argv[] = {NULL, NULL};

		iface = vm_register_interface(ctx, "WtkBuilder", NULL);
		vm_interface_add_method(ctx, iface, "find", VM_VAR, 1, CAST_FUNC(wtklib_builder_get_object), VM_STRING);
		vm_interface_add_method(ctx, iface, "html", VM_VAR, 0, CAST_FUNC(wtklib_builder_generate_html));
		vm_interface_add_method(ctx, iface, "json", VM_VAR, 0, CAST_FUNC(wtklib_builder_generate_json));
		iface = vm_register_interface(ctx, "WtkWidget", NULL);
		vm_interface_add_method(ctx, iface, "on_click", VM_VAR, 1, CAST_FUNC(wtklib_widget_on_click), VM_FUNCTION);
		printf("wtklib setup done\n");
	}

#ifdef __cplusplus
}
#endif

