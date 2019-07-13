#include "stdafx.h"
#include "FrameGrabber.h"


CFrameGrabber::CFrameGrabber(void)
{
	m_capture = NULL;
}


CFrameGrabber::~CFrameGrabber(void)
{
	release();
}

bool CFrameGrabber::init(int index)
{
	m_capture = cvCaptureFromCAM(index);
	return (m_capture != 0);
}

IplImage* CFrameGrabber::grabImg(IplImage* frame)
{
	assert(m_capture);
	m_prvtCnft.Lock();
	IplImage* temp = cvQueryFrame(m_capture);
	if(temp) cvCopy(temp, frame);
	else frame = temp;
	m_prvtCnft.Unlock();
	
	return frame;
}

CvSize CFrameGrabber::frmSize()
{
	assert(m_capture);
	CvSize size;
	size.width = (int)cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_WIDTH);
	size.height = (int)cvGetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_HEIGHT);
	return size;
}

void CFrameGrabber::release()
{
	cvReleaseCapture(&m_capture);
}