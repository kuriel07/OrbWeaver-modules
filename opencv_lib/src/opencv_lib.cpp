// dllmain.cpp : Defines the entry point for the DLL application.
//#include "stdafx.h"
#include "opencv_lib.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video.hpp"
#include "opencv2/imgcodecs.hpp"
#include "orbweaver.h"

//#define EXPORT		
#ifdef __cplusplus
extern "C" {
#endif

	
	cv_array * cv_new_base_class(void * ptr, void * base, cv_array * elem_base, size_t elem_size, int current, int count) {
		cv_array * array = (cv_array * )malloc(sizeof(cv_array));
		array->ptr = ptr;
		array->base = base;
		array->elem_base = elem_base;
		array->elem_size = elem_size;
		array->current = current;
		array->count = count;
		return array;
	}

	cv_array * cv_array_clone(cv_array * array) {
		cv_array * candidate;
		cv_array * arr;
		std::vector<void *> * ptr;	// = new std::vector<void *>(*((std::vector<void *> *)array->ptr));	//recreate duplicate
		cv_array * iter;	// = array->elem_base;
		cv_array * prev;	// = arr;
		if(array == NULL) return NULL;
		arr = cv_new_base_class(array->ptr, array->base, NULL, array->elem_size, array->current, array->count);
		ptr = new std::vector<void *>(*((std::vector<void *> *)array->ptr));	//recreate duplicate;
		iter = array->elem_base;
		prev = arr;
		while(iter != NULL) {
			candidate = cv_new_base_class(iter->ptr, iter->base, NULL, iter->elem_size, iter->current, iter->count);
			prev->elem_base = candidate;
			iter = iter->elem_base;
			prev = candidate;
		}
		arr->ptr = ptr;
		return arr;
	}

	void cv_interface_array_release(void *, void * ptr) {
		cv_array * candidate;
		if (ptr != NULL) {
			cv_array * iface = ((cv_array **)ptr)[0];
			if (iface == NULL) return;
			cv_array * iter = iface->elem_base;
			while(iter != NULL) {
				candidate = iter;
				iter = iter->elem_base;
				free(candidate);
			}
			//free(iface);
			delete iface->ptr;
			free(iface);
		}
	}

	void cv_interface_release(void *, void * ptr) {
		if (ptr != NULL) {
			void * iface = ((void **)ptr)[0];
			if (iface == NULL) return;
			//printf("release :%ld => 0x%x\n", (long)iface, *(long *)iface);
			//free(iface->ptr);
			delete iface;
		}
	}

	void cv_interface_mat_sta_release(void *, void * ptr) {
		if (ptr != NULL) {
			void * iface = ((void **)ptr)[0];
			if (iface == NULL) return;
			//printf("release :%lld => 0x%x\n", (long int)iface, *(long *)iface);
			cv::Mat * mat = (cv::Mat *)iface;
			mat->release();
			delete mat;
		}
	}

	void cv_interface_mat_release(void *, void * ptr) {
		if (ptr != NULL) {
			void * iface = ((void **)ptr)[0];
			if (iface == NULL) return;
			//printf("release :%lld => 0x%x\n", (long)iface, *(long *)iface);
			cv::Mat * mat = (cv::Mat *)iface;
			free(mat->data);
			//free(iface->ptr);
			mat->release();
			delete mat;
		}
	}

	EXPORT int imshow(void * ctx, void * iface, char * name, void * matp) {
		//printf("imshow : %s :%ld => 0x%x\n", name, (long)matp, *(long *)matp);
		cv::Mat * mat = (cv::Mat *)matp;
		//printf("imshow with data %x\n", ((int *)mat->data)[0]);
		cv::imshow((const cv::String &)name, ((cv::InputArray)mat[0]));
		return 0;
	}

	EXPORT void * imread(void * ctx, void * iface, char * name) {
		void * ptr = new cv::Mat(cv::imread((const cv::String &)name));
		//printf("imread : %s :%ld => 0x%x\n", name, (long)ptr, *(long *)ptr);
		//cv::imshow("debug", cv::imread((const cv::String &)name));
		//return ptr;
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvMat"), ptr, cv_interface_mat_sta_release);
	}

	int str_is_numeric(char * str) {
		char c;
		while((c = *str++) != 0) {
			if(c < 0x30 || c > 0x39) return 0;
		}
		return 1;
	}

	EXPORT void * VideoCapture(void * ctx, void * iface, char * name) {
		cv::String cstr = cv::String(name);
		cv::VideoCapture * cap ;
		if(str_is_numeric(name)) cap =  new cv::VideoCapture(atoi(name));
		else cap =  new cv::VideoCapture(cstr);
		printf("VideoCapture : %s, %x\n", name, cap);
		//return (void *)cap;
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvVideoCapture"), cap, cv_interface_release);
	}

	EXPORT void * VideoWriter(void * ctx, void * iface, char * name, char * fourcc, int fps, cv::Size * sz) {
		cv::String cstr = cv::String(name);
		cv::VideoWriter * cap = new cv::VideoWriter(cstr, cv::VideoWriter::fourcc(fourcc[0], fourcc[1], fourcc[2], fourcc[3]), fps, sz[0]);
		printf("VideoWriter : %s, %x\n", name, cap);
		//return (void *)cap;
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvVideoWriter"), cap, cv_interface_release);
	}

	void * opencv_vicap_grab(void * ctx, void * capp) {
		//printf("vicap grab\n");
		cv::Mat mat;
		cv::VideoCapture * cap = (cv::VideoCapture *)capp;
		//printf("grab capp : %x\n", cap);
		cap->read(mat);
		cv::Mat * pmat = new cv::Mat(mat);
		//printf("grab : %x, pmat : %x\n", cap, pmat);
		mat.release();
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvMat"), pmat, cv_interface_mat_sta_release);
	}

	EXPORT void * Mat(void * ctx, void * iface, int w, int h, int c, int emsz, void * data) {
		cv::Mat * mat;
		int datsz;
		void *pdata;
		//printf("cv data size : [%dx%d:%d:%d] %d %ld\n", w, h, c, emsz, vm_array_get_count(ctx, data), data);
		if (data != NULL) {
			datsz = vm_array_get_count(ctx, data);
			pdata = malloc(datsz);
			memcpy(pdata, data, datsz);
			data = pdata;
		}
		switch (emsz) {
		case 1: if (data != NULL) {
					mat = new cv::Mat(cv::Size(w, h), CV_8UC(c), data);
				} else {
					mat = new cv::Mat(cv::Size(w, h), CV_8UC(c)); 
				}
			break;
		case 2: if (data != NULL) {
					mat = new cv::Mat(cv::Size(w, h), CV_16UC(c), data);
				} else {
					mat = new cv::Mat(cv::Size(w, h), CV_16UC(c));
				}
			break;
		case 4: if (data != NULL) {
					mat = new cv::Mat(cv::Size(w, h), CV_32FC(c), data);
				} else {
					mat = new cv::Mat(cv::Size(w, h), CV_32FC(c));
				}
			break;
		default:
			break;
		}
		//return vm_create_interface(ctx, sizeof(cv::Mat *), (uint8 *)mat);
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvMat"), mat, cv_interface_mat_release);
	}

	EXPORT void * Rect(void * ctx, void * iface, int x, int y, int w, int h) {
		cv::Rect * rect = new cv::Rect(x, y, w, h);
		//printf("rect@%lx : %d %d %d %d\n", rect, rect->x, rect->y, rect->width, rect->height);
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvRect"), rect, cv_interface_release);
	}

	EXPORT void * Color(void * ctx, void * iface, int r, int g, int b) {
		//cv::Scalar * color = new CV_RGB(b, g, r);
		cv::Scalar * color = new cv::Scalar((b), (g), (r), 0);

		return vm_create_interface(ctx, vm_select_interface(ctx, "CvScalar"), color, cv_interface_release);
	}

	EXPORT void * Size(void * ctx, void * iface, int w, int h) {
		cv::Size * sz = new cv::Size(w, h);
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvSize"), sz, cv_interface_release);
	}

	EXPORT int waitKey(void * ctx, void * iface, int ms) {
		//printf("waitKey ctx : %x\n",  ctx);
		return cv::waitKey(ms);
	}
	EXPORT void destroyAllWindows(void * ctx, void * iface) {
		cv::destroyAllWindows();
	}

	EXPORT void test_callback(void * ctx, void * iface, void * func, char * str) {
		//vm_push_argument(ctx, strlen(str), str);
		vm_exec_callback(ctx, func, 1 , vm_create_object(ctx, strlen(str), (uchar *)str));
	}

	void * opencv_mat_add(void * ctx, void * vmat, float f) {
		cv::Mat * mat = (cv::Mat *)vmat;
		cv::Mat omat = *mat + f;
		cv::Mat * res = new cv::Mat(omat);
		omat.release();
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvMat"), res, cv_interface_mat_sta_release);
	}

	void * opencv_mat_sub(void * ctx, void * vmat, float f) {
		cv::Mat * mat = (cv::Mat *)vmat;
		cv::Mat omat = *mat - f;
		cv::Mat * res = new cv::Mat(omat);
		omat.release();
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvMat"), res, cv_interface_mat_sta_release);
	}

	void * opencv_mat_mul(void * ctx, void * vmat, float f) {
		cv::Mat * mat = (cv::Mat *)vmat;
		cv::Mat omat = *mat * f;
		cv::Mat * res = new cv::Mat(omat);
		omat.release();
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvMat"), res, cv_interface_mat_sta_release);
	}

	void * opencv_mat_div(void * ctx, void * vmat, float f) {
		cv::Mat * mat = (cv::Mat *)vmat;
		cv::Mat omat = *mat / f;
		cv::Mat * res = new cv::Mat(omat);
		omat.release();
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvMat"), res, cv_interface_mat_sta_release);
	}

	void * opencv_mat_and(void * ctx, void * vmat, int f) {
		cv::Mat * mat = (cv::Mat *)vmat;
		cv::Mat omat = *mat & f;
		cv::Mat * res = new cv::Mat(omat);
		omat.release();
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvMat"), res, cv_interface_mat_sta_release);
	}

	void * opencv_mat_or(void * ctx, void * vmat, int f) {
		cv::Mat * mat = (cv::Mat *)vmat;
		cv::Mat omat = *mat | f;
		cv::Mat * res = new cv::Mat(omat);
		omat.release();
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvMat"), res, cv_interface_mat_sta_release);
	}
	void * opencv_mat_xor(void * ctx, void * vmat, int f) {
		cv::Mat * mat = (cv::Mat *)vmat;
		cv::Mat omat = *mat ^ f;
		cv::Mat * res = new cv::Mat(omat);
		omat.release();
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvMat"), res, cv_interface_mat_sta_release);
	}
	void * opencv_mat_not(void * ctx, void * vmat) {
		cv::Mat * mat = (cv::Mat *)vmat;
		cv::Mat omat = ~*mat;
		cv::Mat * res = new cv::Mat(omat);
		omat.release();
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvMat"), res, cv_interface_mat_sta_release);
	}

	void * opencv_mat_convert(void * ctx, void * vmat, uint32 mode) {
		void * ret;
		cv::Mat * mat = (cv::Mat *)vmat;
		cv::Mat omat;
		mat->convertTo(omat, mode);
		ret = vm_create_interface(ctx, vm_select_interface(ctx, "CvMat"), new cv::Mat(omat), cv_interface_mat_sta_release);
		omat.release();
		return ret;
	}

	void * opencv_mat_clone(void * ctx, void * vmat) {
		cv::Mat * mat = (cv::Mat *)vmat;
		cv::Mat omat = mat->clone();
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvMat"), new cv::Mat(omat), cv_interface_mat_sta_release);
	}

	void * opencv_mat_resize(void * ctx, void * vmat, void * vsize) {
		void * ret;
		cv::Mat * mat = (cv::Mat *)vmat;
		cv::Size * size = (cv::Size *)vsize;
		cv::Mat omat;
		cv::resize(mat[0], omat, size[0]);
		ret = vm_create_interface(ctx, vm_select_interface(ctx, "CvMat"), new cv::Mat(omat), cv_interface_mat_sta_release);
		omat.release();
		return ret;
	}

	void * opencv_mat_crop(void * ctx, void * vmat, void * vrect) {
		void * ret;
		cv::Mat * mat = (cv::Mat *)vmat;
		cv::Rect * rect = (cv::Rect *)vrect;
		printf("crop@%lx : %d %d %d %d\n", vrect, rect->x, rect->y, rect->width, rect->height);
		cv::Mat omat = mat[0](rect[0]);
		ret = vm_create_interface(ctx, vm_select_interface(ctx, "CvMat"), new cv::Mat(omat), cv_interface_mat_sta_release);
		omat.release();
		return ret;
	}

	int opencv_mat_width(void * ctx, void * vmat) {
		cv::Mat * mat = (cv::Mat *)vmat;
		return mat->cols;
	}

	int opencv_mat_height(void * ctx, void * vmat) {
		cv::Mat * mat = (cv::Mat *)vmat;
		return mat->rows;
	}

	void opencv_mat_zeros(void * ctx, void * vmat) {
		cv::Mat * mat = (cv::Mat *)vmat;
		mat->zeros(mat->size(), mat->type());
	}

	int opencv_mat_type(void * ctx, void * vmat) {
		cv::Mat * mat = (cv::Mat *)vmat;
		return mat->type();
	}

	void * opencv_mat_size(void * ctx, void * vmat) {
		cv::Mat * mat = (cv::Mat *)vmat;
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvSize"), new cv::Size(mat->size()), cv_interface_release);
	}

	void * opencv_mat_rectangle(void * ctx, void * vmat, void * vrect, void * vcolor, int thickness, int type) {
		cv::Mat * mat = (cv::Mat *)vmat;
		cv::Rect * rect = (cv::Rect *)vrect;
		cv::Scalar * color = (cv::Scalar *)vcolor;
		//cv::Mat omat = mat[0](rect[0]);
		//printf("rectangle [%d, %d, %d, %d]\n", rect->x, rect->y, rect->width, rect->height);
		//printf("color [%d, %d, %d]\n", color[0], color[1], color[2]);
		
		cv::rectangle(mat[0], *rect, *color, thickness, type);
		return vm_create_object(ctx, 0, NULL);
	}

	void * opencv_mat_polylines(void * ctx, void * vmat, void * vpoints, void * vcolor, int thickness, int type) {
		cv::Mat * mat = (cv::Mat *)vmat;
		//cv::Rect * rect = (cv::Rect *)vrect;
		cv::Scalar * color = (cv::Scalar *)vcolor;
		//vector<Point2f> * color = (cv::Scalar *)vcolor;
		cv_array * points = (cv_array *)vpoints;
		std::vector<cv::Point> pts = ((std::vector<cv::Point> *)points->ptr)[0];
		//printf("total points : %d\n", pts.size());
		//for(int i=0;i<pts.size();i++) {
		//	printf("point[%d] : %d,%d\n", i, pts[i].x, pts[i].y);
		///}
    		cv::polylines(mat[0], ((std::vector<cv::Point> *)points->ptr)[0], type, *color, thickness, 16);
		return vm_create_object(ctx, 0, NULL);
	}

	int opencv_array_count(void * ctx, void * vvec) {
		cv_array * vec = (cv_array * )vvec;
		if(vec == NULL) return 0;
		if(vec->ptr == NULL) return 0;
		//std::vector<void *> * ptr = (std::vector<void *> * )vec->ptr;
		return vec->count;
	}

	void * opencv_array_first(void * ctx, void * vvec) {
		cv_array * vec = (cv_array *)vvec;
		vec = cv_array_clone((cv_array *)vvec);
		std::vector<void *> * ptr = (std::vector<void *> * )vec->ptr;
		vec->current = -1;
		vec->count = ptr->size();
		return vm_create_interface(ctx, vec->base, (void *)vec, NULL);
	}

	void * opencv_array_next(void * ctx, void * vvec) {
		cv_array * vec = (cv_array *)vvec;
		vec->current++;
		if(vec->current >= vec->count) return vm_create_object(ctx, 0, NULL);		//end of enumeration
		char * ptr = (char *)vec->ptr + (vec->current * vec->elem_size);
		return vm_create_interface(ctx, vec->elem_base->base, (void *)&ptr, NULL);
	}

	void * opencv_array_at(void * ctx, void * vvec, int index) {
		cv_array * vec = (cv_array *)vvec;
		if(vec->base == NULL) return vm_create_object(ctx, 0, NULL);
		if(index >= vec->count) return vm_create_object(ctx, 0, NULL);		//out of bounds
		if(vec->elem_base == NULL) {					//leaf base
			std::vector<char> * ptr = (std::vector<char> * )vec->ptr;
			//printf("get dptr\n");
			char * dptr = (char *)ptr->data() + (index * vec->elem_size);
			//printf("leaf base\n");
			return vm_create_interface(ctx, vec->base, (void *)&dptr, NULL);
		} else {
			std::vector< std::vector<char> > * ptr = (std::vector< std::vector<char> > * )vec->ptr;
			//printf("get dptr list\n");
			std::vector<char> dptr = ptr->at(index);
			//printf("create new vector\n");
			std::vector<char> * new_ptr = new std::vector<char>(dptr.begin(), dptr.end());
			//printf("base array clone\n");
			vec->elem_base->ptr = new_ptr;
			cv_array * base_clone = cv_array_clone(vec->elem_base);
			//printf("new cv_array\n");
			void * ret = cv_new_base_class(new_ptr, vec->base, 
			base_clone, 
			vec->elem_base->elem_size, 
			0, 
			(dptr.end() - dptr.begin()) / vec->elem_base->elem_size);
			//printf("done cv_array\n");
			return vm_create_interface(ctx, vm_select_interface(ctx, "CvArray"), ret, cv_interface_array_release);
		}
	}

	void * opencv_array_float_2_integer(void * ctx, void * vvec, int index, int length) {
		cv_array * vec = (cv_array *)vvec;
		if(vec == NULL) return vm_create_object(ctx, 0, NULL);
		if(vec->base == NULL) return vm_create_object(ctx, 0, NULL);
		if(index >= vec->count) return vm_create_object(ctx, 0, NULL);		//out of bounds
		if((vec->elem_size % sizeof(float)) != 0) return vm_create_object(ctx, 0, NULL);	
		if(length == 0) length = (vec->count - index);
		std::vector<float> * ptr = (std::vector<float> * )vec->ptr;
		std::vector<int> * new_ptr = new std::vector<int>();
		for(int i=index;i<(index+length);i++) {
			float * dptr = (float *)((char *)ptr->data() + (i * vec->elem_size));
			//printf("params per-element : %d\n", vec->elem_size / sizeof(float));
			for(int j=0;j<(vec->elem_size / sizeof(float));j++) {
				new_ptr->push_back( (int)dptr[j] );
			}
		}
		vec->elem_base->ptr = new_ptr;
		cv_array * base_clone = cv_array_clone(vec->elem_base);
		void * ret = cv_new_base_class( new_ptr, 
				vm_select_interface(ctx, "CvArray"), 
				base_clone, 
				(vec->elem_size * sizeof(int)) /  sizeof(float), 0, length );

		return vm_create_interface(ctx, vm_select_interface(ctx, "CvArray"), ret, cv_interface_array_release);
	}

	void * opencv_array_integer_2_float(void * ctx, void * vvec, int index, int length) {
		cv_array * vec = (cv_array *)vvec;
		if(vec == NULL) return vm_create_object(ctx, 0, NULL);
		if(vec->base == NULL) return vm_create_object(ctx, 0, NULL);
		if(index >= vec->count) return vm_create_object(ctx, 0, NULL);		//out of bounds
		if((vec->elem_size % sizeof(int)) != 0) return vm_create_object(ctx, 0, NULL);	
		if(length == 0) length = (vec->count - index);
		std::vector<int> * ptr = (std::vector<int> * )vec->ptr;
		std::vector<float> * new_ptr = new std::vector<float>();
		for(int i=index;i<(index+length);i++) {
			int * dptr = (int *)((char *)ptr->data() + (i * vec->elem_size));
			//printf("params per-element : %d\n", vec->elem_size / sizeof(float));
			for(int j=0;j<(vec->elem_size / sizeof(float));j++) {
				new_ptr->push_back( (float)dptr[j] );
			}
		}
		vec->elem_base->ptr = new_ptr;
		cv_array * base_clone = cv_array_clone(vec->elem_base);
		void * ret = cv_new_base_class( new_ptr, 
				vm_select_interface(ctx, "CvArray"), 
				base_clone, 
				(vec->elem_size * sizeof(float)) /  sizeof(int), 0, length );
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvArray"), ret, cv_interface_array_release);
	}

	EXPORT void setup(void * ctx, void * ifx) {
		pk_object_p iface;
		printf("setup\n");
		iface = vm_register_interface(ctx, "CvSize", NULL);
		//printf("size registered\n");
		iface = vm_register_interface(ctx, "CvRect", NULL);
		//printf("rect registered\n");
		iface = vm_register_interface(ctx, "CvScalar", NULL);
		//iface = vm_register_interface(ctx, "CvPoint2F", NULL);
		iface = vm_register_interface(ctx, "CvPoint", NULL);
		//printf("scalar registered\n");
		iface = vm_register_interface(ctx, "CvMat", NULL);
		//printf("mat registered\n");
		vm_interface_add_method(ctx, iface, "+", VM_VAR, 1, CAST_FUNC(opencv_mat_add), VM_FLOAT);
		//printf("mat->add registered\n");
		vm_interface_add_method(ctx, iface, "-", VM_VAR, 1, CAST_FUNC(opencv_mat_sub), VM_FLOAT);
		vm_interface_add_method(ctx, iface, "*", VM_VAR, 1, CAST_FUNC(opencv_mat_mul), VM_FLOAT);
		vm_interface_add_method(ctx, iface, "/", VM_VAR, 1, CAST_FUNC(opencv_mat_div), VM_FLOAT);
		vm_interface_add_method(ctx, iface, "&", VM_VAR, 1, CAST_FUNC(opencv_mat_and), VM_INT32);
		vm_interface_add_method(ctx, iface, "|", VM_VAR, 1, CAST_FUNC(opencv_mat_or), VM_INT32);
		vm_interface_add_method(ctx, iface, "^", VM_VAR, 1, CAST_FUNC(opencv_mat_xor), VM_INT32);
		vm_interface_add_method(ctx, iface, "!", VM_VAR, 0, CAST_FUNC(opencv_mat_not));
		vm_interface_add_method(ctx, iface, "convert", VM_VAR, 1, CAST_FUNC(opencv_mat_convert), VM_INT32);
		vm_interface_add_method(ctx, iface, "clone", VM_VAR, 0, CAST_FUNC(opencv_mat_clone));
		vm_interface_add_method(ctx, iface, "resize", VM_VAR, 1, CAST_FUNC(opencv_mat_resize), VM_VAR);
		vm_interface_add_method(ctx, iface, "crop", VM_VAR, 1, CAST_FUNC(opencv_mat_crop), VM_VAR);
		vm_interface_add_method(ctx, iface, "width", VM_INT32, 0, CAST_FUNC(opencv_mat_width));
		vm_interface_add_method(ctx, iface, "height", VM_INT32, 0, CAST_FUNC(opencv_mat_height));
		vm_interface_add_method(ctx, iface, "zeros", VM_VAR, 0, CAST_FUNC(opencv_mat_zeros));
		vm_interface_add_method(ctx, iface, "type", VM_INT32, 0, CAST_FUNC(opencv_mat_type));
		vm_interface_add_method(ctx, iface, "size", VM_VAR, 0, CAST_FUNC(opencv_mat_size));
		vm_interface_add_method(ctx, iface, "rectangle", VM_VAR, 4, CAST_FUNC(opencv_mat_rectangle), VM_VAR, VM_VAR, VM_INT32, VM_INT32);
		vm_interface_add_method(ctx, iface, "polylines", VM_VAR, 4, CAST_FUNC(opencv_mat_polylines), VM_VAR, VM_VAR, VM_INT32, VM_INT32);

		iface = vm_register_interface(ctx, "CvVideoCapture", NULL);
		vm_interface_add_method(ctx, iface, "grab", VM_VAR, 0, CAST_FUNC(opencv_vicap_grab));
		//printf("video capture registered\n");

		iface = vm_register_interface(ctx, "CvArray", NULL);
		vm_interface_add_method(ctx, iface, "count", VM_INT32, 0, CAST_FUNC(opencv_array_count));
		vm_interface_add_method(ctx, iface, "first", VM_VAR, 0, CAST_FUNC(opencv_array_first));
		vm_interface_add_method(ctx, iface, "next", VM_VAR, 0, CAST_FUNC(opencv_array_next));
		vm_interface_add_method(ctx, iface, "at", VM_VAR, 1, CAST_FUNC(opencv_array_at), VM_INT32);
		vm_interface_add_method(ctx, iface, "to_int", VM_VAR, 2, CAST_FUNC(opencv_array_float_2_integer), VM_INT32, VM_INT32);
		vm_interface_add_method(ctx, iface, "to_float", VM_VAR, 2, CAST_FUNC(opencv_array_integer_2_float), VM_INT32, VM_INT32);
		//vm_install_interface(ctx, iface);
		//printf("setup successful\n");
		opencv_bio_setup(ctx, ifx);
	}

#ifdef __cplusplus
}
#endif

