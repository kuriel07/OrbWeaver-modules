// dllmain.cpp : Defines the entry point for the DLL application.
//#include "stdafx.h"
#include "opencv_lib.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video.hpp"
#include "opencv2/imgcodecs.hpp"
#include "orbweaver.h"
#include <opencv2/face.hpp>


using namespace std;
using namespace cv;
using namespace cv::face;

//#define EXPORT		
#ifdef __cplusplus
extern "C" {
#endif

	
	EXPORT void * CascadeClassifier(void * ctx, void * iface, char * xml_path) {
		cv::CascadeClassifier * classifier = new cv::CascadeClassifier (xml_path);
		//return classifier
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvCascadeClassifier"), classifier, cv_interface_release);
	}

	void * opencv_classifier_detect(void * ctx, void * iface, void * vmat) {
		cv::Mat gray;
		std::vector<cv::Rect> faces;// = new std::vector<cv::Rect>();
		cv::CascadeClassifier * classifier = (cv::CascadeClassifier * )iface;
		if(classifier == NULL) return vm_create_object(ctx, 0, NULL);
		cv::Mat * frame = (cv::Mat * )vmat;
      		cvtColor(frame[0], gray, COLOR_BGR2GRAY);
      		classifier->detectMultiScale(gray, faces);
		//printf("return classifier detect\n");
		//return vm_create_object(ctx, 0, NULL);
		void * ret = cv_new_base_class(new std::vector<cv::Rect>(faces),  vm_select_interface(ctx, "CvArray"), 
			cv_new_base_class(NULL, vm_select_interface(ctx, "CvRect"), NULL, 0, 0, 0), 
			sizeof(cv::Rect), 
			0, 
			faces.size());
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvArray"), ret, cv_interface_array_release);
	}

	EXPORT void * FacemarkLBF(void * ctx, void * iface, char * lbf_path) {
		Ptr<Facemark> facemark = FacemarkLBF::create();
    		facemark->loadModel(lbf_path);
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvFacemarkLBF"), new Ptr<Facemark>(facemark), cv_interface_release);
	}

	void * opencv_facemark_fit(void * ctx, void * iface, void * vmat, void * varrfaces) {
		vector< vector<Point2f> > landmarks;
		cv::Mat * frame = (cv::Mat * )vmat;
		cv_array * array = (cv_array *)varrfaces;
		std::vector<cv::Rect> * faces = (std::vector<cv::Rect> *)array->ptr;
		Ptr<Facemark> * facemark = (Ptr<Facemark> *)iface;
      		bool success = facemark[0]->fit(frame[0], faces[0], landmarks);
		if(!success) return vm_create_object(ctx, 0, NULL);
		void * ret = cv_new_base_class(new std::vector< std::vector<cv::Point2f> >(landmarks),  vm_select_interface(ctx, "CvArray"), 
			cv_new_base_class( NULL, 
				vm_select_interface(ctx, "CvArray"), 
				cv_new_base_class(NULL, vm_select_interface(ctx, "CvPoint"), NULL, 0, 0, 0), 
				sizeof(Point2f), 0, 0 ), 
			sizeof(vector<Point2f>), 
			0, 
			landmarks.size());
		//printf("opencv_bio landmarks : %d\n", landmarks.size());
		return vm_create_interface(ctx, vm_select_interface(ctx, "CvArray"), ret, cv_interface_array_release);
	}

	
	void opencv_bio_setup(void * ctx, void * ifx) {
		pk_object_p iface;
		iface = vm_register_interface(ctx, "CvCascadeClassifier", NULL);
		vm_interface_add_method(ctx, iface, "detect", VM_VAR, 1, CAST_FUNC(opencv_classifier_detect), VM_VAR);
		
		iface = vm_register_interface(ctx, "CvFacemarkLBF", NULL);
		vm_interface_add_method(ctx, iface, "fit", VM_VAR, 2, CAST_FUNC(opencv_facemark_fit), VM_VAR, VM_VAR);
	}

	

#ifdef __cplusplus
}
#endif

