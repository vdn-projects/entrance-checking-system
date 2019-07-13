/****************************************************************************
*   https://code.google.com/p/facerecog/
* Free distribution by Yan Ke
* Contact:     	 xjed09@gmail.com
******************************************************************************/

/*
	Calculate the subsapce, and project a feature vector on it.
	Now we are using Fisherface method. NPE may be added later.
	Usage: CalcSubspace / LoadDataFromFile -> Project -> CalcVectorDist
*/

#pragma once

#include "GlobalHeader.h"
#include "dggev.h"

// define the precision of subspace base vectors, depth = 32 or 64
#define CV_COEF_FC1 CV_64FC1 // 64 seems to be better

// Calls Pca and FisherLda, saves result to W_prjT
// Return the dim of the subspace
int CalcSubspace(CvMat *inputs, int *trainIds);

// inputs can be either a vector or a matrix of column vectors
void Project(CvMat *inputs, CvMat *results);

int GetSubspaceDim();
int GetFtDim(); // dim of feature space

// If the dim is larger than 1, use -(cvDotProduct(target, query) / cvNorm(target) / cvNorm(query));
// Else, use cvNorm(target, query, CV_L2).
// The smaller the dist is, the more similar.
double CalcVectorDist(CvMat *target, CvMat *query);

void SaveSpace(CvFileStorage * database);
void LoadSpace(CString database);

void ReleaseSubspace();