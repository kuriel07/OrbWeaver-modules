
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/xmlIO.h>
#include <libxml/xinclude.h>
#include <libxml/tree.h>
#include <json-c/json.h>
#include "mustach-json-c.h"
#include "orbweaver.h"
#include "wtk.h"

		
#ifdef __cplusplus
extern "C" {
#endif

	static int wtk_estimate_script_size(wtk_node * root) {
		wtk_node * iterator;
		wtk_script * scr;
		int est_len = 0;
		if(root == NULL) return 0;
		iterator = (wtk_node *)root;
		while(iterator != NULL) {
			scr = (wtk_script *)iterator->init_script;
			if(scr != NULL) est_len += scr->len;
			if(iterator->child != NULL) {
				est_len += wtk_estimate_script_size(iterator->child);
			}
			iterator = iterator->sibling;
		}
		return est_len;
	}

	static int wtk_generate_script_s(wtk_node * root, char * buffer) {
		wtk_node * iterator;
		wtk_script * scr;
		int est_len = 0;
		if(root == NULL) return 0;
		iterator = (wtk_node *)root;
		while(iterator != NULL) {
			scr = (wtk_script *)iterator->init_script;
			if(scr != NULL) {
				memcpy(buffer + est_len, scr->content, scr->len);
				est_len += scr->len;
			}
			if(iterator->child != NULL) {
				est_len += wtk_generate_script_s(iterator->child, buffer + est_len) ;
			}
			iterator = iterator->sibling;
		}
		return est_len;
	}

	char * wtk_generate_script(wtk_node * root) {
		wtk_node * iterator;
		wtk_script * scr;
		char * buffer;
		int est_len = 0;
		if(root == NULL) return NULL;
		est_len = wtk_estimate_script_size(root);
		if(est_len == 0) return NULL;
		buffer = (char *)malloc(est_len + 1);
		wtk_generate_script_s(root, buffer);
		buffer[est_len] = 0;		//null terminated string
		return buffer;
	}

	static int wtk_estimate_header_size(wtk_node * root) {
		wtk_node * iterator;
		wtk_script * scr;
		int est_len = 0;
		if(root == NULL) return 0;
		iterator = (wtk_node *)root;
		while(iterator != NULL) {
			est_len += ((wtk_header *)iterator)->len + 128;
			if(iterator->child != NULL) {
				est_len += wtk_estimate_header_size(iterator->child);
			}
			iterator = iterator->sibling;
		}
		return est_len;
	}

	static int wtk_generate_header_s(wtk_node * root, char * buffer) {
		wtk_node * iterator;
		wtk_script * scr;
		int est_len = 0;
		if(root == NULL) return 0;
		iterator = (wtk_node *)root;
		while(iterator != NULL) {
			//memcpy(buffer + est_len, ((wtk_header *)iterator)->content, ((wtk_header *)iterator)->len);
			//<link href="<?php echo $scriptpath; ?>css/terra.css" rel="stylesheet" type="text/css"/>
			//printf("type : %d\n", ((wtk_node *)iterator)->type);
			switch(((wtk_node *)iterator)->type) {
				case WTK_TYPE_JS_REF:
					sprintf(buffer + est_len, "<script type=\"text/javascript\" src=\"%s\"></script>", ((wtk_header *)iterator)->content);
					//printf("js : %s\n", buffer + est_len); 
					break;
				case WTK_TYPE_CSS_REF:
					sprintf(buffer + est_len, "<link href=\"%s\" rel=\"stylesheet\" type=\"text/css\" />", ((wtk_header *)iterator)->content);
					//printf("css : %s\n", buffer + est_len);
					break;
				case WTK_TYPE_JS:
					sprintf(buffer + est_len, "<script type=\"text/javascript\" >%s</script>", ((wtk_header *)iterator)->content);
					//printf("%s\n", buffer + est_len);
					break;
				case WTK_TYPE_CSS:
					sprintf(buffer + est_len, "<style type=\"text/css\" >%s</style>", ((wtk_header *)iterator)->content);
					//printf("%s\n", buffer + est_len);
					break;
			}
			est_len += strlen(buffer + est_len);
			if(iterator->child != NULL) {
				est_len += wtk_generate_header_s(iterator->child, buffer + est_len) ;
			}
			iterator = iterator->sibling;
		}
		return est_len;
	}

	char * wtk_generate_header(wtk_node * root) {
		wtk_node * iterator;
		wtk_script * scr;
		char * buffer;
		int est_len = 0;
		if(root == NULL) return NULL;
		est_len = wtk_estimate_header_size(root);
		printf("header length : %d\n", est_len);
		if(est_len == 0) return NULL;
		buffer = (char *)malloc(est_len + 1);
		est_len = wtk_generate_header_s(root, buffer);
		buffer[est_len] = 0;		//null terminated string
		return buffer;
	}

	void * wtklib_builder_get_object(void * ctx, void * ifx, char * name) {
		wtk_context * wctx = (wtk_context *)ifx;
		wtk_object * wobj;
		if(wctx == NULL) return vm_create_object(ctx, 0, NULL);
		if(wctx->objects == NULL) return vm_create_object (ctx, 0, NULL);
		wobj = wtk_find_object(wctx->objects, name);
		if(wobj == NULL) return vm_create_object(ctx, 0, NULL);
		return vm_create_interface(ctx, vm_select_interface(ctx, "WtkWidget"), wobj, NULL);
	}

	void * wtklib_builder_generate_html(void * ctx, void * ifx) {
		wtk_context * wctx = (wtk_context *)ifx;
		char * script = NULL;
		char * header = NULL;
		char * buffer = NULL;
		void * ret = NULL;
		int hdr_len = 0;
		int scr_len = 0;
		int est_len = 0, content_len = 0;
		if(wctx == NULL) return vm_create_object(ctx, 0, NULL);
		//generate javascript core

		//generate body content
		printf("generate content\n");
		content_len = strlen(wctx->content);
		
		//generate init script
		printf("generate headers\n");
		header = wtk_generate_header((wtk_node *)wctx->headers);
		printf("header : %s\n", header);
		if(header != NULL) hdr_len = strlen(header);
		printf("generate script\n");
		script = wtk_generate_script((wtk_node *)wctx->objects);
		if(script != NULL) scr_len = strlen(script);
		est_len = content_len + scr_len + hdr_len + 512;
		printf("allocate buffer %d\n", est_len);
		buffer = (char *)malloc(est_len);

		sprintf(buffer, "<html><head>%s<script type=\"text/javascript\" >%s</script></head><body>%s</body></html>", header, script, wctx->content);

		//set return value
		ret = vm_create_object(ctx, strlen(buffer), (uint8 *)buffer);
		//clean up allocated buffer
		if(buffer != NULL) free(buffer);
		if(header != NULL) free(header);
		if(script != NULL) free(script);
		return ret;
	}

	void * wtklib_builder_generate_json(void * ctx, void * ifx) {
		json_object * result;
		char * script = NULL;
		char * str = NULL;
		void * ret = NULL;
		wtk_context * wctx = (wtk_context *)ifx;
		if(wctx == NULL) return vm_create_object(ctx, 0, NULL);
		//generate javascript core
		result = json_object_new_object();
		//generate body content
		json_object_object_add(result, (const char *)"content", json_object_new_string_len(wctx->content, strlen(wctx->content)) );
		//generate init script
		script = wtk_generate_script((wtk_node *)wctx->objects);
		if(script != NULL) {
			json_object_object_add(result, (const char *)"init", json_object_new_string_len(script, strlen(script)) );
			free(script);
		} else {
			json_object_object_add(result, (const char *)"init", json_object_new_string_len("", strlen("")) );
		}
		str = (char *)json_object_to_json_string(result);
		ret = vm_create_object(ctx, strlen((const char *)str), (uint8 *)str);
		
		if(str != NULL) free(str);
		if(result != NULL) json_object_put(result);		//decrement reference counter
		return ret;
	}

#ifdef __cplusplus
}
#endif
