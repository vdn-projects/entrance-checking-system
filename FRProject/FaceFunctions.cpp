#include "stdafx.h"
#include "FaceFunctions.h"


CFaceFunctions::CFaceFunctions(void)
{
	m_PresentedImg->Empty();
	m_arrModel.empty();
	timg8 = timg32 = tft = tmdl = NULL;
}

CFaceFunctions::~CFaceFunctions(void)
{
	m_PresentedImg->ReleaseBuffer();
	
	cvReleaseMat(&tft);
	cvReleaseMat(&timg32);
	cvReleaseMat(&tmdl);
	cvReleaseMat(&timg8);
	
	ReleaseAlign();
	ReleaseLight();
	ReleaseFeature();
	ReleaseSubspace();
}

bool CFaceFunctions::InitFaceFun()
{
//Init Face Alignment
	static asm_shape faceShape;
	if( !InitAlign(&faceSize, (void*) &faceShape)) return false;
	
//Init Face Light Pre-Processing
	if(!InitLight(faceSize)) return false;

//Init Face Feature
	m_ftSize = InitFeature(faceSize);
	if(m_ftSize == 0) return false;

	timg8 = cvCreateMat(faceSize.height, faceSize.width, CV_8UC1);
	timg32 = cvCreateMat(faceSize.height, faceSize.width, CV_32FC1);
	tft = cvCreateMat(m_ftSize, 1, CV_32FC1);
	
	return true;
}


bool CFaceFunctions::FacePro(CvArr *pic, CvMat *faceImg8)
{
	
	IplImage *tempImg = NULL;

	CvSize imgSz = cvGetSize(pic);
	tempImg = cvCreateImage(imgSz, IPL_DEPTH_8U, 1);
	cvCvtColor(pic, tempImg, CV_BGR2GRAY);

	if(!GetFace(tempImg, faceImg8, false)) 
	{
		cvReleaseImage(&tempImg);
		return false;
	}
//	cvCvtColor(faceImg8, faceImg8, CV_BGR2GRAY);
	RunLightPrep(faceImg8);
	m_faceRec = DrawRecFace();

	cvReleaseImage(&tempImg);
	return true;
}

void CFaceFunctions::FaceFeature(CvArr *faceImg8, CvMat *model)
{
	cvConvertScale(faceImg8, timg32, 1.0/255);
	GetFeature(timg32, tft);
	Project(tft, model);
}

void CFaceFunctions::SaveToModel( CString classId, CString name, CString path, CvMat *model )
{
	SDatabase sd;
	sd.classId = classId;
	sd.name = name;
	sd.picPath = path;
	sd.model = model;
	m_arrModel.push_back(sd);
}


void CFaceFunctions::ClearModelArr()
{
	sdb_iter	iter = m_arrModel.begin();
	for (; iter != m_arrModel.end(); iter++)
	{
		cvReleaseMat(&(iter->model));
	}
	m_arrModel.clear();
}

void CFaceFunctions::FaceTrain(CString path2PicData, bool saveFace)
{
	ClearModelArr();
	
	path2PicData.TrimRight('\\');
	path2PicData += '\\';
	if(saveFace) ::CreateDirectory(path2PicData + "faces\\", NULL);

	vector<SDirInfo> paths;
	ScanDirectory(path2PicData, paths);
	sdi_iter iter = paths.begin();
	

	for (; iter != paths.end(); iter++)
	{
		IplImage *pic = cvLoadImage(iter->picPath, 1);  //CV_LOAD_IMAGE_GRAYSCALE);
		CvMat *ft = cvCreateMat(m_ftSize, 1, CV_32FC1);

		FacePro(pic, timg8);
		if(saveFace) 
		{
			cvSaveImage(path2PicData + "faces\\" + iter->fileName, timg8);
		}

		cvConvertScale(timg8, timg32, 1.0/255);
		GetFeature(timg32, ft);

		SaveToModel( iter->classId, iter->name, iter->picPath, ft);
		cvReleaseImage(&pic);
	}
	paths.clear();


//Start training
	m_totalImg = m_arrModel.size();
	CvMat	*inputs = cvCreateMat(m_ftSize, m_totalImg, CV_32FC1);
	int		*trainIds = new int[m_totalImg];
	
	FormTrainMat(inputs, trainIds);
	m_mdlSize = CalcSubspace(inputs, trainIds);

	TrainResSave2Model(); //project all face features to subspace & save into gallary
	
	delete []trainIds;
	cvReleaseMat(&inputs);

	tmdl = cvCreateMat(m_mdlSize, 1, CV_64FC1);

}

void CFaceFunctions::FormTrainMat( CvMat *inputs, int *trainIds )
{
	sdb_iter	iter = m_arrModel.begin();
	CvMat	sub, *src;
	int i = 0;
	m_totalId = 0;

	for (; iter != m_arrModel.end(); iter++)
	{
		src = iter->model;
		cvGetCol(inputs, &sub, i);
		cvCopy(src, &sub);

		bool flag = false;
		for (int j = 0; j < i; j++)
		{
			if (trainIds[j] == atoi(iter->classId))
			{
				flag = true;
				break;
			}
		}
		if (!flag) m_totalId++;
		trainIds[i++] = atoi(iter->classId);
	}
}


void CFaceFunctions::TrainResSave2Model()
{
	int		mdSz = GetSubspaceDim();
	sdb_iter	iter = m_arrModel.begin();
	for (; iter != m_arrModel.end(); iter++)
	{
		// project the feature vectors of the training samples
		CvMat *model = cvCreateMat(mdSz, 1, CV_64FC1);
		Project(iter->model, model);
		cvReleaseMat(&(iter->model));
		iter->model = model;
	}
}


void CFaceFunctions::Store2Database()
{
	CvFileStorage * database;
	
	database = cvOpenFileStorage( "database.xml", 0, CV_STORAGE_WRITE );
	cvWriteInt( database, "ModelSize", m_mdlSize);
	cvWriteInt( database, "TotalImages", m_totalImg);
	cvWriteInt( database, "TotalIds", m_totalId);

	sdb_iter	iter = m_arrModel.begin();
	for (int i = 0; iter != m_arrModel.end(); iter++)
	{
		CString mdlAdding, idAdding, pathAdding, nameAdding;
				
				idAdding.Format("ID_%d", i);
				nameAdding.Format("Name_%d",i);
				pathAdding.Format("Path_%d", i);
				mdlAdding.Format("ModelVector_%d", i++ );

		
		cvWriteString(database, idAdding, iter->classId);
		cvWriteString(database, nameAdding, iter->name);
		cvWriteString(database, pathAdding, iter->picPath);
		cvWrite(database, mdlAdding, iter->model, cvAttrList(0,0));
	}
	SaveSpace(database);

	cvReleaseFileStorage( &database );
}

void CFaceFunctions::LoadDatabase(CString database)
{
	ClearModelArr();
	
	CString preId, postId = "xxx";
	CvFileStorage * fileStorage;
	fileStorage = cvOpenFileStorage( database, 0, CV_STORAGE_READ );
	
	m_mdlSize = cvReadIntByName(fileStorage, 0, "ModelSize", 0);
	tmdl = cvCreateMat(m_mdlSize, 1, CV_64FC1);


	m_totalImg = cvReadIntByName(fileStorage, 0, "TotalImages", 0);
	m_totalId = cvReadIntByName(fileStorage, 0, "TotalIds", 0);
	for(int i = 0; i < m_totalImg; i++)
	{
		CString mdlAdding, idAdding, pathAdding, nameAdding;
		idAdding.Format("ID_%d", i);
		nameAdding.Format("Name_%d",i);
		pathAdding.Format("Path_%d", i);
		mdlAdding.Format("ModelVector_%d", i );

		SaveToModel(cvReadStringByName(fileStorage, 0, idAdding, 0),
					cvReadStringByName(fileStorage, 0, nameAdding, 0),
					cvReadStringByName(fileStorage, 0, pathAdding, 0),
					(CvMat*)cvReadByName(fileStorage, 0, mdlAdding, 0));

		preId = cvReadStringByName(fileStorage, 0, idAdding, 0);
		if(preId != postId)
		{
			postId = preId;
			m_PresentedImg[atoi(postId)] = cvReadStringByName(fileStorage, 0, pathAdding, 0);
		}
	}

	cvReleaseFileStorage( &fileStorage );
	LoadSpace(database);
}

void CFaceFunctions::FaceRecognize( CvArr *pic, SMatch *info )
{
	FacePro(pic, timg8);
	FaceFeature(timg8, tmdl);

	SDatabase	*minpm = NULL;
	sdb_iter	iter = m_arrModel.begin();
	double	minDist = 1e9, curVal; 
	for (; iter != m_arrModel.end(); iter++)
	{
		curVal = CalcVectorDist(iter->model, tmdl);
		if (curVal < minDist)
		{
			minDist = curVal;
			minpm = &(*iter);
		}
	}

	info->classId = minpm->classId;
	info->name = minpm->name;
	info->dist = minDist;
//	info->picPath = minpm->picPath;

	info->picPath = m_PresentedImg[atoi(minpm->classId)];

}

CString CFaceFunctions::FindName( CString fn )
{
	int a = fn.Find('_'), b = fn.ReverseFind('_');
	//return p > 0 ? fn.Left(p) : (q > 0 ? fn.Left(q) : fn);
	return fn.Mid(a + 1, b - a - 1);
}

CString CFaceFunctions::FindId( CString fn )
{
	int a = fn.Find('_');
	return fn.Left(a);
}

void CFaceFunctions::ScanDirectory( CString Dir, vector<SDirInfo> &DirInfo )
{
	Dir.TrimRight('\\');
	Dir += '\\';
	CFileFind search;
	BOOL fileSearch = search.FindFile(Dir + "*.*");
	while(fileSearch)
	{
		fileSearch = search.FindNextFile(); 
		if (search.IsDirectory()) continue;
		CString info = search.GetFileName();
		DirInfo.push_back(SDirInfo(FindId(info), FindName(info), Dir + info, info));
	}
}


