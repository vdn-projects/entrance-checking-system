#pragma once
#include "GlobalHeader.h"

class CFrameGrabber
{
public:
	CFrameGrabber(void);
	~CFrameGrabber(void);
private:
	CvCapture *m_capture;
	CCriticalSection	m_prvtCnft; //prevent conflict
public:
	bool init(int index = 0);
	IplImage* grabImg(IplImage* frame = NULL);
	CvSize frmSize();
	void release();
};
