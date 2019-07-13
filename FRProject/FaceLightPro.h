/*
	Do the light preprocessing for face image.
	The strategy of now is: homographic filtering, then do histogram normalization.
	Usage: InitLight->RunLightPrep...->ReleaseLight
	2 files are needed. See below.
*/

#pragma once
#include "GlobalHeader.h"


#define MASK_FN		"mask.bmp"
#define HISTMD_FN	"histModel.bmp" // the target image for doing histogram normalization
/****************************************************************************
*   https://code.google.com/p/facerecog/
* Free distribution by Yan Ke
* Contact:     	 xjed09@gmail.com
****************************************************************************/

bool InitLight(CvSize imgSz);

// input is 8u, calls HomographicFilter and HistNorm
void RunLightPrep(CvMat *faceImg8);

/*
	return a filter kernel
	filterType: 0:ideal, 1:gaussian, 2:butterworth
	bHighpass: true: highpass;(not supported) false: lowpass
	d0: cut-off freq, independent to kernel size
	order: for butterworth
	h: the output kernel(normalized)
*/
void GenFilterKernel(CvMat *h, int filterType, /*bool bHighpass,*/ double d0, int order = 1);

void ReleaseLight();

