#pragma once

#include <fstream>
#include <vector>
#include <atlstr.h>

#include "C:\OpenCV2.4.3\build\include\opencv\cv.h"
#include "C:\OpenCV2.4.3\build\include\opencv\highgui.h"
#include "C:\OpenCV2.4.3\build\include\opencv\cvaux.h"
#include "C:\OpenCV2.4.3\build\include\opencv2\opencv.hpp"

using namespace std;
using namespace cv;

//#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
	#pragma comment(lib,"C:/OpenCV2.4.3/build/x86/vc10/lib/opencv_core243d.lib")
	#pragma comment(lib,"C:/OpenCV2.4.3/build/x86/vc10/lib/opencv_imgproc243d.lib")
	#pragma comment(lib,"C:/OpenCV2.4.3/build/x86/vc10/lib/opencv_highgui243d.lib")
	#pragma comment(lib,"C:/OpenCV2.4.3/build/x86/vc10/lib/opencv_objdetect243d.lib")
	#pragma comment(lib,"C:/OpenCV2.4.3/build/x86/vc10/lib/opencv_ml243d.lib")
	#pragma comment(lib,"C:/OpenCV2.4.3/build/x86/vc10/lib/opencv_video243d.lib")
	#pragma comment(lib,"C:/OpenCV2.4.3/build/x86/vc10/lib/opencv_features2d243d.lib")

#else
	#pragma comment(lib,"C:/OpenCV2.4.3/build/x86/vc10/lib/opencv_core243.lib")
	#pragma comment(lib,"C:/OpenCV2.4.3/build/x86/vc10/lib/opencv_imgproc243.lib")
	#pragma comment(lib,"C:/OpenCV2.4.3/build/x86/vc10/lib/opencv_highgui243.lib")
	#pragma comment(lib,"C:/OpenCV2.4.3/build/x86/vc10/lib/opencv_objdetect243.lib")
//	#pragma comment(lib,"C:/OpenCV2.4.3/build/x86/vc10/lib/opencv_ml243.lib")
//	#pragma comment(lib,"C:/OpenCV2.4.3/build/x86/vc10/lib/opencv_video243.lib")
//	#pragma comment(lib,"C:/OpenCV2.4.3/build/x86/vc10/lib/opencv_features2d243.lib")

#endif

