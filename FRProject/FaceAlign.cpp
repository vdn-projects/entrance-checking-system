/****************************************************************************
* Free distribution by Yan Ke
* Contact:     	 xjed09@gmail.com
* Last modification (2013) by Nguyen Van Duc, email: vanduc.dn@gmail.com
****************************************************************************/

#include "stdafx.h"
#include "FaceAlign.h"

#define FDFN	"haarcascade_frontalface_alt.xml"
#define ASMFN	"AsmModel.amf"

#pragma comment(lib, "asmlibrary.lib")

CvSize	g_faceSz;
int		g_normRow, g_normDis;	// the eye's position of the aligned face
int		g_nFiltLevel;			// to remove the noise in the detected eye position, use average position of g_nFiltLevel frames

CvPoint2D32f	*g_lefta, *g_righta;
float			g_angle, g_dis; // angle and distance between 2 eyes
CvRect			g_faceRc;

asmfitting		g_AsmFit;
asm_vjfacedetect	g_FDinAsm;
asm_shape		g_shape, g_detshape;
asm_shape		*detshapes = 0;

CvHaarClassifierCascade	*FA_cascade;
CvMemStorage			*FA_storage;



// declaration of functions which are not the interface of the dll(DLLEXP)
bool InitAsm();
bool AsmDetectFeaturePoints(IplImage *pic8);


bool InitAlign( CvSize *faceSz, void *detshape )
{
	*faceSz = cvSize(63, 78); // set to return the size of the aligned face
	g_faceSz = *faceSz;
	detshapes = (asm_shape *)detshape;
	g_normRow = 24;
	g_normDis = 30;
	g_nFiltLevel = 3;

	g_lefta = new CvPoint2D32f[g_nFiltLevel];
	g_righta = new CvPoint2D32f[g_nFiltLevel];
	return InitAsm();
}


bool InitAsm()
{
	g_AsmFit.Read(ASMFN);

	FA_cascade = (CvHaarClassifierCascade*)cvLoad(FDFN, 0, 0, 0);
	FA_storage = cvCreateMemStorage(0);
	
	return true;
}

bool GetFace( IplImage *pic8, CvMat *faceImg8, bool useBuf)
{
	if (! AsmDetectFeaturePoints(pic8)) return false;
	
	CvPoint2D32f leftEye, rightEye;
	leftEye = g_shape[31];	rightEye = g_shape[36]; // see ASM.tif for indices of feature points
	return EyeCoord2FaceCrop(pic8, faceImg8, leftEye, rightEye, useBuf);
	return true;
}

CvRect DrawRecFace()
{
	return g_faceRc;
}

SFacePose GetFacePose()
{
	SFacePose fp;
	fp.eyeAngle = g_angle;
	fp.eyeDis = g_dis;
	fp.rc = g_faceRc;
	return fp;
}

void ReleaseAlign()
{
	delete []g_lefta;
	delete []g_righta;
	cvReleaseHaarClassifierCascade(&FA_cascade);
	cvReleaseMemStorage(&FA_storage);
}

bool AsmDetectFeaturePoints( IplImage *pic8 )
{
	int nFaces;
	CvSeq *pFaces;
	IplImage *picCopy = pic8;
	int minSize = pic8->width/5;

	pFaces = cvHaarDetectObjects(
		pic8, FA_cascade, FA_storage,
		1.2,
		3,
		0 |
		//CV_HAAR_DO_CANNY_PRUNING	|
		CV_HAAR_FIND_BIGGEST_OBJECT	|
		CV_HAAR_DO_ROUGH_SEARCH	|
		0,
		cvSize( minSize, minSize));
	nFaces = pFaces->total;
	if (nFaces == 0) return false;
	g_detshape.Resize(2);
	g_faceRc = *(CvRect*)cvGetSeqElem(pFaces, 0);

	g_detshape[0].x = float(g_faceRc.x);
	g_detshape[0].y = float(g_faceRc.y);
	g_detshape[1].x = float(g_faceRc.x+g_faceRc.width);
	g_detshape[1].y = float(g_faceRc.y+g_faceRc.height);

	InitShapeFromDetBox(g_shape, g_detshape,
						g_AsmFit.GetMappingDetShape(), g_AsmFit.GetMeanFaceWidth());

	g_AsmFit.Fitting(g_shape, picCopy); // fit ASM model
	return true;
}



bool EyeCoord2FaceCrop( IplImage * pic8, CvMat * faceImg8,  CvPoint2D32f leftEye, 
					   CvPoint2D32f rightEye, bool useBuf )
{
	static int idx = 0;
	CvPoint2D32f l1 = cvPoint2D32f(0,0), r1 = cvPoint2D32f(0,0);
	if (useBuf)
	{
		g_lefta[idx] = leftEye;
		g_righta[idx++] = rightEye;
		idx %= g_nFiltLevel;
		for (int i = 0; i < g_nFiltLevel; i++)
		{
			l1.x += g_lefta[i].x/g_nFiltLevel;
			l1.y += g_lefta[i].y/g_nFiltLevel;
			r1.x += g_righta[i].x/g_nFiltLevel;
			r1.y += g_righta[i].y/g_nFiltLevel;
		}
	}
	else
	{
		l1 = leftEye;
		r1 = rightEye;
	}

	float	xDis = r1.x - l1.x,
			yDis = r1.y - l1.y;

	g_angle = cvFastArctan(yDis, xDis);
	g_dis = sqrt(xDis*xDis + yDis*yDis);


	CvMat *map = cvCreateMat(2, 3, CV_32FC1);
	CvMat *tmpDst = cvCreateMat(pic8->height, pic8->width, CV_8UC1);
	cv2DRotationMatrix(l1, g_angle, g_normDis/g_dis, map); // similar transformation
	//DispCvArr(map, "map");
	cvWarpAffine(pic8, tmpDst, map);

	int		leftEyeXNew = cvRound((g_faceSz.width - g_normDis)/2);
	int		left = cvRound(l1.x - leftEyeXNew),
			top  = cvRound(l1.y - g_normRow);
	CvMat	tmpHeader, *sub = 0;

	if (left >= 0 && top >= 0 &&
		left + g_faceSz.width <= tmpDst->width &&
		top + g_faceSz.height <= tmpDst->height)
	{	
		sub = cvGetSubRect(tmpDst, &tmpHeader, cvRect(left, top, g_faceSz.width, g_faceSz.height));
		cvCopy(sub, faceImg8);
	}

	cvReleaseMat(&map);
	cvReleaseMat(&tmpDst);
	return (sub != 0);
}