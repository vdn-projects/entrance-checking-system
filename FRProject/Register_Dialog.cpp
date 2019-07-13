// Register_Dialog.cpp : implementation file
//

#include "stdafx.h"
#include "FRProject.h"
#include "Register_Dialog.h"
#include "afxdialogex.h"
#define PATH_TO_FACE_COLLECTING  "D:/FaceDemo/"

CFrameGrabber reg_frGrb;
bool reg_CamRun;

IplImage* cropImg;
CvRect* RectFace;
CvHaarClassifierCascade *cascade;
CvMemStorage            *storage;
bool faceDetection(IplImage *pImg);

#define FACE_CROP_WIDTH 70
#define FACE_CROP_HEIGHT 85

// Register_Dialog dialog

IMPLEMENT_DYNAMIC(Register_Dialog, CDialog)

Register_Dialog::Register_Dialog(CWnd* pParent /*=NULL*/)
	: CDialog(Register_Dialog::IDD, pParent)
	, m_strName(_T(""))
	, m_strID(_T(""))
	, m_NumFaces(0)
	, m_bSaveImg(false)
{
	m_thrdReg = NULL;
	m_frame = NULL;
	m_bRegStart = false;
	m_bFacefound = false;
	notfoundFace = cvLoadImage("notfoundFace.bmp");
}

Register_Dialog::~Register_Dialog()
{
	cvReleaseImage(&notfoundFace);
}

void Register_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ID, m_strID);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
}


BOOL Register_Dialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	EmbedCvWindow(GetDlgItem(IDC_STATIC_FACE)->m_hWnd, "face", 130, 150);
	if (notfoundFace) cvShowImage("face", notfoundFace);
	
	cropImg = cvCreateImage(cvSize(FACE_CROP_WIDTH, FACE_CROP_HEIGHT), 8, 3);
	char *filename = "C:/OpenCV2.4.3/data/haarcascades/haarcascade_frontalface_alt.xml";
	cascade = (CvHaarClassifierCascade*)cvLoad(filename, 0, 0, 0);
	storage = cvCreateMemStorage(0);


	//reg_FR->InitFaceFun();
//	m_faceImg8 = cvCreateMat(g_faceFun->faceSize.height, g_faceFun->faceSize.width, CV_8UC1);


	reg_CamRun = reg_frGrb.init(1);
	
		if(reg_CamRun)
	{
		m_frame = cvCreateImage(reg_frGrb.frmSize(), 8, 3);
		m_bRegStart = true;
		m_thrdReg = ::AfxBeginThread(RunRegDialog, this);
	}
	return TRUE;
}


BEGIN_MESSAGE_MAP(Register_Dialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CAPTURE, &Register_Dialog::OnBnClickedButtonCapture)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &Register_Dialog::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_NEWONE, &Register_Dialog::OnBnClickedButtonNewone)
	ON_BN_CLICKED(IDC_BUTTON_DONE, &Register_Dialog::OnBnClickedButtonDone)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &Register_Dialog::OnBnClickedButtonCancel)
END_MESSAGE_MAP()


UINT Register_Dialog::reg_Display()
{
	while(m_bRegStart)
	{
		reg_frGrb.grabImg(m_frame);
		m_bFacefound = faceDetection(m_frame);
		if(m_bFacefound)
		{
			cvShowImage("face", cropImg);
		}
		else cvShowImage("face", notfoundFace);
		
	//	cvShowImage("face", m_frame);
		cvWaitKey(1);
	}
	return 0;
}



UINT RunRegDialog( LPVOID pParam )
{
	Register_Dialog *pDlg = (Register_Dialog *)pParam;
	return pDlg->reg_Display();
}
// Register_Dialog message handlers


void Register_Dialog::OnBnClickedButtonCapture()
{
	// TODO: Add your control notification handler code here

	if(!m_bFacefound) return;
	UpdateData(TRUE);
	if (m_strName.IsEmpty() || m_strID.IsEmpty())
		{
			::AfxMessageBox("Please enter Name & ID!");
			return;
		}
	else
	{
		cvNamedWindow("Face Collecting", 0);
		cvShowImage("Face Collecting", cropImg);

	///	IplImage *save1 = cvCloneImage(cropImg);
	//	m_Face4Store.AddTail(save1);

		IplImage *save2 = cvCloneImage(m_frame);
		m_Face4Train.AddTail(save2);

		m_NumFaces++;
		CString numface;

		numface.Format("%d", m_NumFaces);
		SetDlgItemText(IDC_NUM_FACE, numface);

	}

}


void Register_Dialog::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here

	CString PathSave = PATH_TO_FACE_COLLECTING;
	::CreateDirectory(PathSave, NULL);

	CString PicSave;
	POSITION pos1 = m_Face4Train.GetHeadPosition();
		//	 pos2 = m_Face4Store.GetHeadPosition();

	for(int i = 0; i < m_NumFaces; i++)
	{
		PicSave.Format("%s_%s_%d.bmp", m_strID, m_strName, i+1);
		cvSaveImage(PathSave + PicSave, m_Face4Train.GetNext(pos1));
	//	cvSaveImage(PathSave + "Store_" + PicSave, m_Face4Store.GetNext(pos2));
	}

	m_bSaveImg = true;
}


void Register_Dialog::OnBnClickedButtonNewone()
{
	// TODO: Add your control notification handler code here
	if(!m_bSaveImg) 
	{
		::AfxMessageBox("Please SAVE before capturing a new one!");
		return;
	}

	m_strName = "";
	m_strID = "";
	SetDlgItemText(IDC_EDIT_NAME, "");
	SetDlgItemText(IDC_EDIT_ID, "");

	POSITION pos1 = m_Face4Train.GetHeadPosition();
		//	 pos2 = m_Face4Store.GetHeadPosition();

	for(int i = 0; i < m_NumFaces; i++)
	{
		cvReleaseImage(&m_Face4Train.GetNext(pos1))	;
	//	cvReleaseImage(&m_Face4Store.GetNext(pos2))	;
	}
	m_Face4Train.RemoveAll();
//	m_Face4Store.RemoveAll();

	m_NumFaces = 0;
	SetDlgItemText(IDC_NUM_FACE, "0");
	cvDestroyWindow("Face Collecting");
	m_bSaveImg = false;
}

bool faceDetection(IplImage *pImg)
{
	bool faceDetected = false;
	CvRect* region = 0;
	int minSize = pImg->width/5;
	
	cvClearMemStorage(storage);
	CvSeq *faces = cvHaarDetectObjects(pImg, cascade, storage, 1.2, 3, 
					CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_DO_ROUGH_SEARCH, 
					cvSize( minSize, minSize)); //one biggest face
	
	//faceDetected flag is on when detected
	if(faces->total == 1) {
		faceDetected = true;
		region = ( CvRect*)cvGetSeqElem(faces, 0);
		RectFace = region;
	//Crop face from photo stream
		cvSetImageROI(pImg, cvRect(region->x, region->y, region->width, region->height));
		IplImage* 	cropImg_ = cvCreateImage(cvSize(region->width, region->height), 8, 3);
		cvCopy(pImg, cropImg_);
		cvResize(cropImg_, cropImg, CV_INTER_CUBIC);
		cvResetImageROI(pImg);

	//Draw rectangle on the face
/*		cvRectangle( pImg, cvPoint( region->x, region->y ), 
			cvPoint( region->x + region->width, region->y + region->height ), 
			CV_RGB( 255, 0, 0 ), 2, 8, 0 );
*/
		cvReleaseImage(&cropImg_);
	}
	
	return faceDetected;
}

void Register_Dialog::OnBnClickedButtonDone()
{
	// TODO: Add your control notification handler code here
		if(!m_bSaveImg) 
	{
		::AfxMessageBox("Please SAVE before quit registering!");
		return;
	}	
	
	m_strName = "";
	m_strID = "";
	m_NumFaces = 0;
	m_bSaveImg = false;
	SetDlgItemText(IDC_EDIT_NAME, "");
	SetDlgItemText(IDC_EDIT_ID, "");
	

	if(reg_CamRun)
	{
		m_bRegStart = false;
		if(m_thrdReg) ::WaitForSingleObject(m_thrdReg->m_hThread, INFINITE);
	}



	cvReleaseImage(&m_frame);
	cvDestroyWindow("Face Collecting");
	cvDestroyWindow("face");
	OnOK();
}


void Register_Dialog::OnBnClickedButtonCancel()
{
	// TODO: Add your control notification handler code here
		if(reg_CamRun)
	{
		m_bRegStart = false;
		if(m_thrdReg) ::WaitForSingleObject(m_thrdReg->m_hThread, INFINITE);
	}

	cvReleaseImage(&m_frame);
	cvDestroyWindow("Face Collecting");
	cvDestroyWindow("face");
	OnCancel();
}
