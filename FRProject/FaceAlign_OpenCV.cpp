#include "stdafx.h"
#include "FaceAlign.h"

CvSize	g_faceSz;
CvRect	g_faceRc;
CvPoint2D32f	g_margin;
CvSize2D32f		g_cutoff;
const int	nFiltLevel = 3;

CvHaarClassifierCascade	*g_FDcascade;
CvMemStorage			*g_FDstorage;
#define FDFN	"haarcascade_frontalface_alt.xml"
#define ASMFN	"AsmModel.amf"


bool InitAlign( CvSize *faceSz, void *ignore )
{
	*faceSz = cvSize(70, 80);
	g_faceSz = *faceSz;
	float a = .011, b = .75; //  see below for the meaning of a and b
	g_cutoff = cvSize2D32f(g_faceSz.width*a,g_faceSz.height*a);
	g_margin = cvPoint2D32f((1-g_cutoff.width)/2,(1-g_cutoff.height)*b);

	g_FDcascade = (CvHaarClassifierCascade*)cvLoad(FDFN, 0, 0, 0);


	g_FDstorage = cvCreateMemStorage(0);

	return true;
}

bool GetFace( IplImage *pic8, CvMat *faceImg8, bool useBuf )
{

	int minSize = pic8->width/5;

	CvSeq *pFaces = cvHaarDetectObjects(
		pic8, g_FDcascade, g_FDstorage,
		1.2,
		3,
		0 |
	/*	CV_HAAR_DO_CANNY_PRUNING	|*/
		CV_HAAR_FIND_BIGGEST_OBJECT	|
		CV_HAAR_DO_ROUGH_SEARCH		|
		0,
		cvSize( minSize, minSize));

	if (!pFaces || pFaces->total == 0)
		return false;
	g_faceRc = *(CvRect *)cvGetSeqElem(pFaces, 0);
	
	//static CvRect	*history = new CvRect[nFiltLevel];
	static CvRect history[nFiltLevel];
	static int idx;
	if (useBuf)
	{
		history[idx++] = g_faceRc;
		g_faceRc = cvRect(0,0,0,0);
		idx %= nFiltLevel;
		for (int i = 0; i < nFiltLevel; i++)
		{
			g_faceRc.x += history[i].x/nFiltLevel;
			g_faceRc.y += history[i].y/nFiltLevel;
			g_faceRc.width += history[i].width/nFiltLevel;
			g_faceRc.height += history[i].height/nFiltLevel;
		}
	}

	float a = g_faceRc.width;
	g_faceRc.height = int(g_cutoff.height*g_faceRc.height);
	g_faceRc.width = int(g_cutoff.width*g_faceRc.width);
	g_faceRc.x += int(g_margin.x*a);
	g_faceRc.y += int(g_margin.y*a);
	

	CvMat sub;
	cvGetSubRect(pic8, &sub, g_faceRc);
	cvResize(&sub, faceImg8);
	return true;
}

SFacePose GetFacePose()
{
	SFacePose fp;
	fp.eyeAngle = 0;
	fp.eyeDis = g_faceRc.width*3/7;
	fp.rc = g_faceRc;
	return fp;
}

void ReleaseAlign()
{
	cvReleaseHaarClassifierCascade(&g_FDcascade);
	cvReleaseMemStorage(&g_FDstorage);
}

CvRect DrawRecFace()
{
	return g_faceRc;
}