// FRProjectDlg.cpp : implementation file
//
#include "stdafx.h"
#include "FRProject.h"
#include "FRProjectDlg.h"
#include "afxdialogex.h"
#include "CMSComm.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <time.h>
#include <windows.h>
#include "mmsystem.h"

#define PATH_FOR_TRAIN "D:\\FaceDatabase"
#define MAX_NUM_OF_ID 100

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//variables declaration
CFrameGrabber g_frGrb;
bool g_CamRun;
CFaceFunctions *g_faceFun = new CFaceFunctions;
double g_timeStart;

//RFID reader variables
int m_count=0;
CString g_ID_Num[14], g_StoreID[MAX_NUM_OF_ID];
int g_ID;
std::stringstream ss; 

//Convert ID number
void readIDnum(string filename, CString ID_Array[MAX_NUM_OF_ID], char separator = ' ')
{
	std::ifstream file(filename.c_str(), ifstream::in);
	    if (!file) {
				::AfxMessageBox("Cannot read the file", MB_OK | MB_ICONINFORMATION);
		}
	 string line, idconvert, idreal;
	while (getline(file, line)) {
		stringstream liness(line);
		getline(liness, idconvert, separator);
		getline(liness, idreal);
		if(!idconvert.empty() && !idreal.empty()) {
			ID_Array[atoi(idconvert.c_str())] = idreal.c_str();
		}
		}

}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CFRProjectDlg dialog


CFRProjectDlg::CFRProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFRProjectDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_frame = NULL;
	m_matchImg = NULL;
	m_thrdFR = NULL;
	m_notify = NULL;
	m_bthrdStart = false;
	m_faceDectected = false;
	m_bPortConnected = false;
	m_flagID = false;
	m_unknownPerson = cvLoadImage("unknown_person.jpg");
	m_noface = cvLoadImage("Noface.jpg");
}

void CFRProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSCOMM1, m_mscomm);
}


BEGIN_MESSAGE_MAP(CFRProjectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_TRAIN, &CFRProjectDlg::OnBnClickedButtonTrain)
	ON_BN_CLICKED(IDC_BUTTON_REGISTER, &CFRProjectDlg::OnBnClickedButtonRegister)
	ON_BN_CLICKED(ID_FRPROJ_EXIT, &CFRProjectDlg::OnBnClickedFrprojExit)
	ON_BN_CLICKED(IDC_BUTTON_SETUP, &CFRProjectDlg::OnBnClickedButtonSetup)
END_MESSAGE_MAP()


// CFRProjectDlg message handlers

BOOL CFRProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	g_faceFun->InitFaceFun();
	faceImg8 = cvCreateMat(g_faceFun->faceSize.height, g_faceFun->faceSize.width, CV_8UC1);
	g_faceFun->LoadDatabase("database.xml");
	g_ID = 0;
	readIDnum("id card.txt", g_StoreID);


	g_CamRun = g_frGrb.init(1);
	if(g_CamRun){
		
		CWnd *pWnd = GetDlgItem(IDC_VIDEO);
		EmbedCvWindow(pWnd->m_hWnd, "Camera", 576, 432);

		CWnd	*pWnd1 = GetDlgItem(IDC_MATCH_PIC);
		EmbedCvWindow(pWnd1->m_hWnd, "match", 150, 200); 
	
		CWnd *pWnd2 = GetDlgItem(IDC_STATIC_NOTIFY);
		EmbedCvWindow(pWnd2->m_hWnd, "notify", 70, 70);

		m_frame = cvCreateImage(g_frGrb.frmSize(), 8, 3);
		m_bthrdStart = true;
		m_thrdFR = ::AfxBeginThread(RunDialog, this);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}


UINT CFRProjectDlg::Display()
{
	long frame_delay = 0;
	bool flagAccepted = false, flagNotAccepted = false;
	CString matchName, matchPath, matchID;

	//TEst
	//m_flagID = true;
	//g_ID = 5;
	time_t start, end;
	time (&start);
	int counter = 0;
	double sec, fps;

	while(m_bthrdStart)
	{
		g_frGrb.grabImg(m_frame);
		m_faceDectected = g_faceFun->FacePro(m_frame, faceImg8);

		if(m_faceDectected)
		{
			frame_delay++;
			g_faceFun->FaceRecognize(m_frame, &matchInfo);
		
			if( frame_delay > 4 && m_flagID)
			{
				if(g_ID == atoi(matchInfo.classId))
				{
					matchName.Format("Name: %s", matchInfo.name);
					matchID.Format("ID: %s", matchInfo.classId);
					SetDlgItemText(IDC_MATCH_ID, matchID);
					SetDlgItemText(IDC_MATCH_NAME, matchName);
					

					m_matchImg = cvLoadImage(matchInfo.picPath);
					cvShowImage("match", m_matchImg);

					NotifyInfo("\nACCESS ACCEPTED", "Accept");
					flagAccepted = true;

				}
				else
				{
					PlaySound(TEXT("alarm1.wav"), NULL, SND_SYNC);
					SetDefaultItems();
					NotifyInfo("\nACCESS DENIED!", "NotAccept");
					flagNotAccepted = true;
				}
				
				g_ID = 0;
				m_flagID = false;	
			}
			else if((!flagAccepted) && (!flagNotAccepted))
			{
					cvShowImage("match", m_unknownPerson);
					SetDefaultItems();
					NotifyInfo("You need an ID card and stand in font of the camera within 1 meter.", "ID card");
			}

			cvRectangle( m_frame, cvPoint( g_faceFun->m_faceRec.x, g_faceFun->m_faceRec.y ), 
						cvPoint( g_faceFun->m_faceRec.x + g_faceFun->m_faceRec.width, g_faceFun->m_faceRec.y + g_faceFun->m_faceRec.height ), 
						CV_RGB( 0, 255, 0 ), 2, 8, 0 );
		}
		else
		{
			flagAccepted = false;
			flagNotAccepted = false;
			frame_delay = 0;
			cvShowImage("match", m_noface);
			SetDefaultItems();
			NotifyInfo("\nIDLE...", "idle");
		}
		
		cvShowImage("Camera", m_frame);
		cvWaitKey(3);

		time(&end);
		++counter;
		sec = difftime(end, start);
		fps = counter/sec;
		CString showFPS;
		showFPS.Format("FPS: %.2f", fps);
		SetDlgItemText(IDC_STATIC_FPS, showFPS);
	}
	
	return 0;
}


void CFRProjectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFRProjectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFRProjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT RunDialog( LPVOID pParam )
{
	CFRProjectDlg *pDlg = (CFRProjectDlg *)pParam;
	return pDlg->Display();
}

void CFRProjectDlg::SetDefaultItems(void)
{
	SetDlgItemText(IDC_MATCH_NAME, "Name:");
	SetDlgItemText(IDC_MATCH_ID, "ID:");
}

void CFRProjectDlg::NotifyInfo(CString Status, CString icon)
{
	SetDlgItemText(IDC_TRAINING_STATUS, Status);
	m_notify = cvLoadImage(icon + ".jpg");
	cvShowImage("notify", m_notify);
}

void CFRProjectDlg::OnBnClickedButtonTrain()
{
	// TODO: Add your control notification handler code here
	NotifyInfo("\nFACE TRAINING", "traning");

	m_thrdFR->SuspendThread();
	int ret = m_train.DoModal();

	if(ret == IDOK)
	{
		g_faceFun->InitFaceFun();
		
		SetDlgItemText(IDC_TRAINING_STATUS, "\nTRAINING...");
		
		g_timeStart = (double)cvGetTickCount();

		g_faceFun->FaceTrain(m_train.m_strFaceDir, m_train.m_SaveFace);
		
		double timeEnd = ((double)cvGetTickCount() - g_timeStart) / ((double)cvGetTickFrequency()*1e6);

		if(m_train.m_SaveXml) g_faceFun->Store2Database();

		SetDlgItemText(IDC_TRAINING_STATUS, "");

		CString msg;
			msg.Format("Done Training! \nTotal IDs: %d Total images: %d, Feature size: %d Model Size: %d\nTraining time: %lf seconds",
				g_faceFun->m_totalId, g_faceFun->m_totalImg,
				g_faceFun->m_ftSize, g_faceFun->m_mdlSize, timeEnd);
		::AfxMessageBox(msg, MB_OK | MB_ICONINFORMATION);
	}
	m_thrdFR->ResumeThread();
	SetDefaultItems();
}


void CFRProjectDlg::OnBnClickedButtonRegister()
{
	// TODO: Add your control notification handler code here
	NotifyInfo("\nREGISTER", "register");

	m_thrdFR->SuspendThread();
	m_register.DoModal();
	m_thrdFR->ResumeThread();
	SetDefaultItems();
}

BEGIN_EVENTSINK_MAP(CFRProjectDlg, CDialogEx)
	ON_EVENT(CFRProjectDlg, IDC_MSCOMM1, 1, CFRProjectDlg::OnCommMscomm, VTS_NONE)
END_EVENTSINK_MAP()

void CFRProjectDlg::PortSettings(void)
{
	// if port is already opened then close port.
		if( m_mscomm.get_PortOpen()) 
				m_mscomm.put_PortOpen(false);
		// Setting comport
		m_mscomm.put_CommPort(m_setup.m_portnum);

		// Setting Handshaking
		m_mscomm.put_Handshaking(0);
	
		m_mscomm.put_Settings("9600,N,8,1");  //default setup (baudrate, parity, Data bit, Stop bit)
		m_mscomm.put_RThreshold(1);  // read data right after it appears in buffer
		// set for input direction
		m_mscomm.put_InputLen(1); // Read every charater each time
		m_mscomm.put_InBufferSize(1024);
		m_mscomm.put_InputMode(0); //  0- text mode, 1- binary mode
		m_mscomm.put_OutBufferSize(1024);
		m_mscomm.put_PortOpen(true); // oprt open
}

void CFRProjectDlg::OnCommMscomm()
{
	// TODO: Add your message handler code here
	UpdateData(TRUE);
	VARIANT data;
	CString UserID;

	switch (m_mscomm.get_CommEvent()){
	case 1: // comEvSend
		break;
	case 2: // comEvReceive
		data = m_mscomm.get_Input();
		g_ID_Num[m_count]= (CString)data.bstrVal;
		ss << (CString)data.bstrVal;
		m_count++;
		if(m_count == 14)
		{
		//	SetDlgItemText(IDC_STATIC_IDNUMBER, ss.str().c_str());
			for(int i=6; i<=10; i++)
			{
				UserID += g_ID_Num[i];	
			}	 	

			for(int j = 1; j< MAX_NUM_OF_ID; j++)
			{
				if(g_StoreID[j] == UserID) 
				{	
					g_ID = j;
					m_flagID = true;
					break;
				}
			}


			m_count = 0;
	//		ID_num->ReleaseBuffer();

		}
			//UpdateData(FALSE);		
		break;
	case 3:	// comEvCTS

		break;
	case 4:	// ComEvDSR
		break;
	case 5: // comEvCD
		break;
	case 6: // comEvRing
		break;
	case 7: // comEvEOF
		break;
	default:	break;
	};
	
	UpdateData(false);
}


void CFRProjectDlg::OnBnClickedButtonSetup()
{
	// TODO: Add your control notification handler code here
	NotifyInfo("\nSETUP", "setup");

	m_thrdFR->SuspendThread();
	int ret = m_setup.DoModal();
	if(ret == IDOK)
	{
			g_faceFun->InitFaceFun();
			g_faceFun->LoadDatabase(m_setup.m_strDatabasePath);
			PortSettings();
		/*	m_bPortConnected = true;
			CString msg;
			msg.Format("%d", m_setup.m_portnum);
			::AfxMessageBox(msg, MB_OK | MB_ICONINFORMATION);
			*/
	}

	m_thrdFR->ResumeThread();
	SetDefaultItems();	
}


void CFRProjectDlg::OnBnClickedFrprojExit()
{
	// TODO: Add your control notification handler code here

	if(g_CamRun)
	{
		m_bthrdStart = false;
		m_thrdFR->SuspendThread();
		cvReleaseImage(&m_unknownPerson);
		cvReleaseImage(&m_noface);
		cvReleaseImage(&m_frame);
		cvReleaseImage(&m_notify);
		cvReleaseImage(&m_matchImg);
		cvDestroyWindow("Camera");
	}
	delete g_faceFun;
	OnCancel();
}