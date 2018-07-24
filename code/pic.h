#ifndef PIC
#define PIC

#include <string>
#include "energy.h"
#include <random>

class Pic
{
	Mat data;
	Mat extradata;
	bool isprevent;
	bool isremoval;
	int n, m;
	Point* RowSeam;
	Point* ColSeam;
	EnergyFunc* func;
	string funcName;
	void InitEnergyFunc();						//to add energyfunc in this function
	double getEnergy(int i, int j);
public:
	Pic(string funcname, string filename);		//creat from picture file
	Pic(Pic* p, Point* s, bool row);			//creat by Pic*p removing s by row/col
	Pic(string funcname, Mat mat);
	~Pic();
	void setextra(bool re, Point* poly, int n_poly);		//to set isremoval(1) or isprevent(0)
	Mat getdata() { return data; }
	Point* getRowSeam() { return RowSeam; }
	Point* getColSeam() { return ColSeam; }
	void output(string filename);
	double findRow();							//return min total energy of seam by row, at the same time restore this seam 
	double findCol();
};

#endif