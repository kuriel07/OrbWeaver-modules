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

//#define EXPORT		
#ifdef __cplusplus
extern "C" {
#endif

void wtk_init_headers(wtk_context * ctx) {
	wtk_header *  header;
	if(ctx == NULL) return;
	//TODO change to absolute address with servername and current protocol (http/https)
	//char * url = vm_get_request_url(ctx);		//TODO : use vm_get_request_url
	if(ctx->headers == NULL) ctx->headers = wtk_create_header(WTK_TYPE_CSS_REF, "css/terra.css");
	else wtk_node_add_sibling((wtk_node *)ctx->headers, (wtk_node *)wtk_create_header(WTK_TYPE_CSS_REF, "css/terra.css"));
	wtk_node_add_sibling((wtk_node *)ctx->headers, (wtk_node *)wtk_create_header(WTK_TYPE_JS_REF, "js/terra.js"));
	wtk_node_add_sibling((wtk_node *)ctx->headers, (wtk_node *)wtk_create_header(WTK_TYPE_JS_REF, "js/jquery.base64.min.js"));
	wtk_node_add_sibling((wtk_node *)ctx->headers, (wtk_node *)wtk_create_header(WTK_TYPE_JS_REF, "js/jquery.min.js"));
	wtk_node_add_sibling((wtk_node *)ctx->headers, (wtk_node *)wtk_create_header(WTK_TYPE_JS_REF, "js/jquery-ui.min.js"));
	wtk_node_add_sibling((wtk_node *)ctx->headers, (wtk_node *)wtk_create_header(WTK_TYPE_JS_REF, "js/json2.js"));
	wtk_node_add_sibling((wtk_node *)ctx->headers, (wtk_node *)wtk_create_header(WTK_TYPE_JS_REF, "js/mustache.min.js"));
	wtk_node_add_sibling((wtk_node *)ctx->headers, (wtk_node *)wtk_create_header(WTK_TYPE_JS_REF, "js/hash-min.js"));
}

#ifdef __cplusplus
}
#endif

