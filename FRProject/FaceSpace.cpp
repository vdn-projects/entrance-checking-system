/****************************************************************************
* Free distribution by Yan Ke
* Contact:     	 xjed09@gmail.com
* Last modification (2013) by Nguyen Van Duc, email: vanduc.dn@gmail.com
****************************************************************************/

#include "stdafx.h"
#include "FaceSpace.h"

#pragma comment(lib, "dggev.lib") // solving generalized eigen problem using CLAPACK

static CvMat	*mu_total;
static CvMat	*W_pcaT, *W_fldT, *W_prjT;
static int		inputDim, postPcaDim, postLdaDim;
static int		trainNum, classNum; // sample number and class number of training samples

/*
	Do PCA for inputs, the results is saved in mu_total and W_pcaT.
	inputs:			feature matrix, each column is a feature vector.
	postPcaDimCoef:	If it's a integer, it is the dimension of the PCA subspace;
					If it's a decimal between 0~1, it is the ratio of the reserved eigenvalues
*/
void Pca(CvMat *inputs, double postPcaDimCoef);


/*
	inputs:		M-by-N, similar with Pca, but the average of each row should be 0. inputs will be changed.
	trainIds:	ID tags of train samples. should be of the same length with N.
	postLdaDimCoef:	if = 0, it will be automatically chosen as (classnumber - 1)
				( See P. N. Belhumeur£¬Eigenfaces vs. fisherfaces: recognition using 
				class specific linear projection)
				If so, if (classnumber - 1) < 3, in case the dim is too small, it will be min(N, 3)
	return:		the number of classes used for training(How many diff people).

	The eigenvector and eigenvalue calculated by CLAPACK is a little diff from MATLAB.
	Calls CalcSwSb and CalcLdaSpace, save result to W_fldT.
*/
void FisherLda(CvMat *inputs, int *trainIds, int postLdaDimCoef = 0);

void CalcSwSb(CvMat *inputs, int *trainIds, CvMat *Sw, CvMat *Sb);

void CalcLdaSpace(CvMat *Sw64, CvMat *Sb64, int postLdaDimCoef);


int CalcSubspace( CvMat *inputs, int *trainIds )
{
	ReleaseSubspace();
	inputDim = inputs->rows;
	trainNum = inputs->cols;
	Pca(inputs, .97);

	CvMat *pcaFace = cvCreateMat(postPcaDim, trainNum, CV_COEF_FC1);
	cvProjectPCA(inputs, mu_total, W_pcaT, pcaFace);

	// same w/ cvProjectPCA
	//CvMat	sub;
	//CvRect	rc = cvRect(0,0, 1, inputDim);
	//for (int i = 0; i < trainNum; i++)
	//{
	//	rc.x = i;
	//	cvGetSubRect(inputs, &sub, rc);
	//	cvSub(&sub, mu_total, &sub);
	//}	
	//cvmMul(W_pcaT, inputs, pcaFace);

	FisherLda(pcaFace, trainIds);

	//cvReleaseMat(&W_prjT);
	W_prjT = cvCreateMat(postLdaDim, inputDim, CV_COEF_FC1);
	cvmMul(W_fldT, W_pcaT, W_prjT);

	//DispCvArr(W_pcaT, "W_p");
	//DispCvArr(W_fldT, "W_f");
	//DispCvArr(W_prjT, "W_opt");
	cvReleaseMat(&pcaFace);
	return postLdaDim;
}

void Pca( CvMat *inputs, double postPcaDimCoef )
{
	int		oriDim = inputs->rows, sampleNum = inputs->cols;
	CvMat	*eigenVecs = cvCreateMat(sampleNum, oriDim, CV_COEF_FC1),
		*eigenVals = cvCreateMat(sampleNum, 1, CV_COEF_FC1);
	//cvReleaseMat(&mu_total);
	mu_total = cvCreateMat(oriDim, 1, CV_COEF_FC1);

	//CvMat *cov_mat = cvCreateMat(sampleNum, sampleNum, CV_COEF_FC1);
	//cvCalcCovarMatrix(inputs, sampleNum, cov_mat, mu_total, CV_COVAR_SCRAMBLED);
	//CvMat *U = cvCreateMat(sampleNum, sampleNum, CV_COEF_FC1),
	//	*S = cvCreateMat(sampleNum, 1, CV_COEF_FC1);
	//W_pcaT = cvCreateMat(postPcaDim, oriDim, CV_COEF_FC1);
	//cvSVD(cov_mat, S, U, NULL, CV_SVD_MODIFY_A | CV_SVD_U_T);

	cvCalcPCA(inputs, mu_total, eigenVals, eigenVecs, CV_PCA_DATA_AS_COL); 
	// the eigenvalue seems to range in descending order
	// eigenvectors are row vectors, and seems to have been normalized

	postPcaDim = int(postPcaDimCoef);
	if (postPcaDim == 0) // postPcaDimCoef is between 0~1, decide the dim according to ratio
	{
		//DispCvArr(eigenVals,"ev",true,"%2.3f ");
		double sum = cvSum(eigenVals).val[0], sum1 = 0;
		int i = 0;
		do 
		{
			sum1 += cvmGet(eigenVals, i++, 0);
		} while (sum1 < sum*postPcaDimCoef);
		postPcaDim = i;
		if (postPcaDim < 8) postPcaDim = min(sampleNum, 8); // postPcaDim shouldn't be too small
	}

	CvMat sub;
	//cvReleaseMat(&W_pcaT); // if training has done before
	W_pcaT = cvCreateMat(postPcaDim, oriDim, CV_COEF_FC1);
	cvGetSubRect(eigenVecs, &sub, cvRect(0,0, oriDim, postPcaDim));
	cvCopy(&sub, W_pcaT);
	//DispCvArr(W_pcaT, "Wpca");

	cvReleaseMat(&eigenVals);
	cvReleaseMat(&eigenVecs);
}

void FisherLda( CvMat *inputs, int *trainIds, int postLdaDimCoef /*= 0*/ )
{
	int	oriDim = inputs->rows;
	CvMat *Sw = cvCreateMat(oriDim, oriDim, CV_COEF_FC1),
		*Sb = cvCreateMat(oriDim, oriDim, CV_COEF_FC1); 

	CalcSwSb(inputs, trainIds, Sw, Sb);

	if (CV_COEF_FC1 == CV_64FC1)
		CalcLdaSpace(Sw, Sb, postLdaDimCoef);
	else
	{
		CvMat	*Sw64 = cvCreateMat(oriDim, oriDim, CV_64FC1),
				*Sb64 = cvCreateMat(oriDim, oriDim, CV_64FC1);
		cvConvert(Sw, Sw64);
		cvConvert(Sb, Sb64);
		CalcLdaSpace(Sw64, Sb64, postLdaDimCoef);

		CvMat *W_fldT64 = cvCloneMat(W_fldT);
		cvReleaseMat(&W_fldT);
		W_fldT = cvCreateMat(postLdaDim, oriDim, CV_COEF_FC1);
		cvConvert(W_fldT64, W_fldT);

		cvReleaseMat(&Sw64);
		cvReleaseMat(&Sb64);
		cvReleaseMat(&W_fldT64);
	}

	cvReleaseMat(&Sw);
	cvReleaseMat(&Sb);
}

void CalcSwSb( CvMat *inputs, int *trainIds, CvMat *Sw, CvMat *Sb )
{
	int		oriDim = inputs->rows, sampleNum = inputs->cols;
	int		*id2idx = new int[sampleNum], *smpNumEachClass = new int[sampleNum];
	CvMat	**muPerClass = new CvMat *[sampleNum], sub;
	classNum = 0;

	// within-class average
	for (int i = 0; i < sampleNum; i++)
	{
		cvGetCol(inputs, &sub, i);

		int j;
		for (j = 0; j < classNum; j++)
		{
			if (id2idx[j] == trainIds[i]) 
			{
				break;
			}
		}
		if (j == classNum)
		{
			muPerClass[j] = cvCloneMat(&sub);
			id2idx[j] = trainIds[i];
			smpNumEachClass[j] = 1;
			classNum++;
		}
		else
		{
			cvAdd(muPerClass[j], &sub, muPerClass[j]);
			smpNumEachClass[j] ++;
		}
	}

	for (int i = 0; i < classNum; i++)
		cvScale(muPerClass[i], muPerClass[i], 1.0/(double(smpNumEachClass[i])));

	// reduce within-class average
	for (int i = 0; i < sampleNum; i++) 
	{
		cvGetCol(inputs, &sub, i);
		int j;
		for (j = 0; j < classNum; j++)
			if (id2idx[j] == trainIds[i]) break;
		cvSub(&sub, muPerClass[j], &sub);
	}

	// within-class scatter matrix
	cvMulTransposed(inputs, Sw, 0);

	// between-class scatter matrix
	cvSetZero(Sb);
	CvMat *Sb1 = cvCreateMat(oriDim, oriDim, CV_COEF_FC1);
	for (int i = 0; i < classNum; i++)
	{
		cvMulTransposed(muPerClass[i], Sb1, 0);
		cvAddWeighted(Sb, 1, Sb1, smpNumEachClass[i], 0, Sb);
	}

	//DispCvArr(Sw, "Sw");
	//DispCvArr(Sb, "Sb");
	cvReleaseMat(&Sb1);
	delete []id2idx;
	delete []smpNumEachClass;
	for (int i = 0; i < classNum; i++)
		cvReleaseMat(& muPerClass[i]);
	delete []muPerClass;
}

void CalcLdaSpace( CvMat *Sw64, CvMat *Sb64, int postLdaDimCoef )
{
	int oriDim = Sw64->rows;
	CvMat	*eigenVecs = cvCreateMat(oriDim, oriDim, CV_64FC1);
	double	*ar = new double[oriDim], *ai = new double[oriDim], 
		*be = new double[oriDim];
	double	*A, *B, *E;

	// GeneralEig
	A = Sb64->data.db;
	B = Sw64->data.db;
	E = eigenVecs->data.db;
	GeneralEig(A, B, oriDim, E, ar, ai, be); 
	//DispCvArr(eigenVecs,"vecs", false, "%1.10f\t");


	// choose subspace
	CvMat arm = cvMat(1, oriDim, CV_64FC1, ar);
	CvMat bem = cvMat(1, oriDim, CV_64FC1, be);
	CvMat *idxm = cvCreateMat(1, oriDim, CV_32SC1);
	cvAbs(&arm, &arm); // are all the value positive? is abs needed?(positive definite?)
	cvAbs(&bem, &bem);
	cvAddS(&bem, cvScalar(.001), &bem); // be is always near zero
	cvDiv(&arm, &bem, &arm);
	//DispCvArr(&arm, "arm");
	cvSort(&arm, &arm, idxm, CV_SORT_DESCENDING | CV_SORT_EVERY_ROW);

	if (postLdaDimCoef == 0)
	{
		postLdaDim = classNum-1;
		if (postLdaDim < 1) postLdaDim = 1;
		//if (postLdaDim < 3) postLdaDim = min(sampleNum, 3);
	}

	//cvReleaseMat(&W_fldT);
	W_fldT = cvCreateMat(postLdaDim, oriDim, CV_64FC1);
	CvMat subSrc, subDst;
	for (int i = 0; i < postLdaDim; i++)
	{
		cvGetRow(eigenVecs, &subSrc, int(cvGetReal1D(idxm, i)));
		cvGetRow(W_fldT, &subDst, i);
		cvCopy(&subSrc, &subDst); // normalize?
	}

	cvReleaseMat(&eigenVecs);
	delete []ar;
	delete []ai;
	delete []be;
	cvReleaseMat(&idxm);
}

void Project( CvMat *inputs, CvMat *results )
{
	cvProjectPCA(inputs, mu_total, W_prjT, results);
}

int GetSubspaceDim(){return postLdaDim;}

int GetFtDim(){return inputDim;}

double CalcVectorDist( CvMat *target, CvMat *query )
{
	// use normalized cosine metric
	// other alternative metrics: L1, L2, Mahalanobis ...
	if (target->rows > 1)
		return -(cvDotProduct(target, query) / cvNorm(target) / cvNorm(query));
	else
		return cvNorm(target, query, CV_L2);
}

void ReleaseSubspace()
{
	cvReleaseMat(&mu_total);
	cvReleaseMat(&W_pcaT);
	cvReleaseMat(&W_fldT);
	cvReleaseMat(&W_prjT);
}

void SaveSpace(CvFileStorage * database)
{
//	cvWriteInt( database, "dataBytes", W_prjT->step / W_prjT->cols );
//	cvWriteInt( database, "inputDim", inputDim );
//	cvWriteInt( database, "postLdaDim", postLdaDim );
	cvWrite( database, "mu_total", mu_total, cvAttrList(0,0));
	cvWrite( database, "W_prjT", W_prjT, cvAttrList(0,0));
}

void LoadSpace(CString database)
{
	CvFileStorage * fileStorage;
	fileStorage = cvOpenFileStorage( database, 0, CV_STORAGE_READ );
	
//	cvReleaseMat(&mu_total);
//	cvReleaseMat(&W_prjT);

//	inputDim = cvReadIntByName(fileStorage, 0, "inputDim", 0);
//	postLdaDim = cvReadIntByName(fileStorage, 0, "postLdaDim", 0);
	mu_total = (CvMat *)cvReadByName(fileStorage, 0, "mu_total", 0);
	W_prjT = (CvMat *)cvReadByName(fileStorage, 0, "W_prjT", 0);
	
	cvReleaseFileStorage( &fileStorage );
}