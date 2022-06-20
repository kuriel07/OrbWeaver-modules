#include <gst/gst.h>
#include <dlfcn.h>
#include <stdio.h>
extern int
host_main (int argc, char *argv[]);
int
main (int argc, char *argv[])
{
  
	#define RTLD_DEEPBIND	0x00008	/* Use deep binding.  */
	void * dll = dlopen("/home/kouryuu/opencv_lib/lib/libopencv_lib.so", RTLD_NOW | RTLD_GLOBAL | RTLD_DEEPBIND );
	//dll = dlopen("/home/kouryuu/opencv_lib/lib/liborbweaver.so", RTLD_NOW | RTLD_GLOBAL | RTLD_DEEPBIND );
	dll = dlopen("/home/kouryuu/gstreamer_lib/lib/libgstreamer_lib.so", RTLD_NOW | RTLD_GLOBAL | RTLD_DEEPBIND );
	int (*host)(int argc, char * argv[]) = (int (* )(int, char * []))dlsym(dll, "host_main");
	if(host != 0) {
		return host(argc, argv);
	}	
	printf("not found\n");	
	return -1;
}
