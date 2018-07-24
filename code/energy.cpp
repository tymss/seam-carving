#include "energy.h"

EnergyFunc::EnergyFunc(int _n, int _m):
	n(_n), m(_m)
{
	energy = new double*[n];
	for(int i=0; i<n; i++)
		energy[i] = new double[m];
}

EnergyFunc::~EnergyFunc()
{
	for(int i=0; i<n; i++)
		delete []energy[i];
	delete []energy;
}

void SobelFunc::CalEnergy(Mat data)
{
	Mat graX, graY;
	Sobel(data, graX, CV_64F, 1, 0, 3);
	Sobel(data, graY, CV_64F, 0, 1, 3);
	for(int i=0; i<n; i++)
		for(int j=0; j<m; j++)
		{
			energy[i][j] = fabs(graX.at<Vec3d>(i, j)[0]) + fabs(graX.at<Vec3d>(i, j)[1]) + fabs(graX.at<Vec3d>(i, j)[2]) +
						   fabs(graY.at<Vec3d>(i, j)[0]) + fabs(graY.at<Vec3d>(i, j)[1]) + fabs(graY.at<Vec3d>(i, j)[2]);
		}
}

void ScharrFunc::CalEnergy(Mat data)
{
	Mat graX, graY;
	Scharr(data, graX, CV_64F, 1, 0);
	Scharr(data, graY, CV_64F, 0, 1);
	for(int i=0; i<n; i++)
		for(int j=0; j<m; j++)
		{
			energy[i][j] = fabs(graX.at<Vec3d>(i, j)[0]) + fabs(graX.at<Vec3d>(i, j)[1]) + fabs(graX.at<Vec3d>(i, j)[2]) +
						   fabs(graY.at<Vec3d>(i, j)[0]) + fabs(graY.at<Vec3d>(i, j)[1]) + fabs(graY.at<Vec3d>(i, j)[2]);
		}
}

void LaplaceFunc::CalEnergy(Mat data)
{
	Mat grad;
	Laplacian(data, grad, CV_64F, 3);
	for(int i=0; i<n; i++)
		for(int j=0; j<m; j++)
			energy[i][j] = fabs(grad.at<Vec3d>(i, j)[0]) + fabs(grad.at<Vec3d>(i, j)[1]) + fabs(grad.at<Vec3d>(i, j)[2]);
}