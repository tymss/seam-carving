#ifndef ENERGY
#define ENERGY

#include <opencv2\opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class EnergyFunc
{
protected:
	double** energy;
	int n, m;
public:
	EnergyFunc(int, int);
	~EnergyFunc();
	virtual void CalEnergy(Mat data) = 0;
	double GetEnergy(int x, int y)
	{ return energy[x][y]; }
};

class SobelFunc: public EnergyFunc
{
public:
	SobelFunc(int x, int y): EnergyFunc(x, y) {}
	void CalEnergy(Mat data);						//data(CV_64FC3)
};

class ScharrFunc: public EnergyFunc
{
public:
	ScharrFunc(int x, int y): EnergyFunc(x, y) {}
	void CalEnergy(Mat data);
};

class LaplaceFunc: public EnergyFunc
{
public:
	LaplaceFunc(int x, int y): EnergyFunc(x, y) {}
	void CalEnergy(Mat data);
};

#endif