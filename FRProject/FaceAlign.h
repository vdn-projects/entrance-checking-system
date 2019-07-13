/****************************************************************************
*   https://code.google.com/p/facerecog/
* Free distribution by Yan Ke
* Contact:     	 xjed09@gmail.com
****************************************************************************/

#pragma once
#include "GlobalHeader.h"
#include "asmfitting.h"
//#pragma comment(lib, "asmlibrary.lib")


struct SFacePose
{
	CvRect	rc; // Rect of the face in the picture
	float	eyeDis; // distance between eyes
	float	eyeAngle; // rotation of the face
};

// face size is an output. To change other align coefs, change the code of this funtion.
// The input DETSHAPE is a tradeoff. See the comments at the top.
// I'm searching for a alignment algorithm which could replace asmlibrary.
bool InitAlign(CvSize *faceSz, void *detshape);

// if useBuf = true, eye position will be smoothed among frames
bool EyeCoord2FaceCrop( IplImage * pic8, CvMat * faceImg, 
							  CvPoint2D32f leftEye, CvPoint2D32f rightEye, bool useBuf );

bool GetFace(IplImage *pic8, CvMat *faceImg, bool useBuf);

CvRect DrawRecFace();

SFacePose GetFacePose();

void ReleaseAlign();
