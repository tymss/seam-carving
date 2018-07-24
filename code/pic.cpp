#include "pic.h"

Pic::Pic(string funcname, string filename):
	RowSeam(NULL), ColSeam(NULL), funcName(funcname), isremoval(0), isprevent(0)
{
	Mat img = imread(filename);
	if(img.empty())
		cout << "fail to read picture" << endl;
	else
		cout << "read picture success" << endl;
	img.convertTo(data, CV_64FC3, 1./255.);
	n = data.rows;
	m = data.cols;
	InitEnergyFunc();
}

Pic::Pic(string funcname, Mat mat):
	RowSeam(NULL), ColSeam(NULL), funcName(funcname), data(mat), isremoval(0), isprevent(0)
{
	n = data.rows;
	m = data.cols;
	InitEnergyFunc();
}

Pic::Pic(Pic* p, Point* s, bool row):
	RowSeam(NULL), ColSeam(NULL), funcName(p->funcName)
{
	isprevent = p->isprevent;
	isremoval = p->isremoval;
	//TODO
	if(row)
	{
		n = p->n - 1;
		m = p->m;
	}
	else
	{
		n = p->n;
		m = p->m - 1;
	}
	Mat newdata(n, m, CV_64FC3);
	Mat newextra(n, m, CV_8U);
	if(row)
	{
		for(int j=0; j<m; j++)
		{
			int i = 0;
			while(i < s[j].x)
			{
				if(isremoval || isprevent)
					newextra.at<uchar>(i, j) = p->extradata.at<uchar>(i, j);
				newdata.at<Vec3d>(i, j) = p->data.at<Vec3d>(i, j);
				i++;
			}
			while(i < n)
			{
				if(isremoval || isprevent)
					newextra.at<uchar>(i, j) = p->extradata.at<uchar>(i+1, j);
				newdata.at<Vec3d>(i, j) = p->data.at<Vec3d>(i+1, j);
				i++;
			}
		}
	}
	else
	{
		for(int i=0; i<n; i++)
		{
			int j = 0;
			while(j < s[i].y)
			{
				if(isremoval || isprevent)
					newextra.at<uchar>(i, j) = p->extradata.at<uchar>(i, j);
				newdata.at<Vec3d>(i, j) = p->data.at<Vec3d>(i, j);
				j++;
			}
			while(j < m)
			{
				if(isremoval || isprevent)
					newextra.at<uchar>(i, j) = p->extradata.at<uchar>(i, j+1);
				newdata.at<Vec3d>(i, j) = p->data.at<Vec3d>(i, j+1);
				j++;
			}
		}
	}
	data = newdata;
	if(isremoval || isprevent)
		extradata = newextra;
	InitEnergyFunc();
}

Pic::~Pic()
{
	if(RowSeam != NULL)
		delete RowSeam;
	if(ColSeam != NULL)
		delete ColSeam;
	if(func != NULL)
		delete func;
}

void Pic::setextra(bool re, Point* poly, int n_poly)
{
	if(re)
		isremoval = 1;
	else
		isprevent = 1;
	Mat mat(n, m, CV_8U, Scalar(0));
	fillConvexPoly(mat, poly, n_poly, Scalar(1));
	cout << "finish set extra" << endl;
	extradata = mat;
}

void Pic::InitEnergyFunc()
{
	if(funcName == "sobel")
		func = new SobelFunc(n, m);
	else if(funcName == "scharr")
		func = new ScharrFunc(n, m);
	else if(funcName == "laplace")
		func = new LaplaceFunc(n, m);
	func->CalEnergy(data);
}

void Pic::output(string filename)
{
	Mat src;
	data.convertTo(src, CV_8UC3, 255.0);
	imwrite(filename, src);
}

double Pic::getEnergy(int i, int j)
{
	if(isremoval)
	{
		if(extradata.at<uchar>(i, j) == 1)
			return -1000.0;
		else
			return func->GetEnergy(i, j);
	}
	else if(isprevent)
	{
		if(extradata.at<uchar>(i, j) == 1)
			return 1000.0;
		else
			return func->GetEnergy(i, j);
	}
	else
		return func->GetEnergy(i, j);
}

double Pic::findRow()
{
	double minE = 1000000.0;
	int lasti =-1;
	double** M = new double*[n];
	int** lastrow = new int*[n];
	for(int i=0; i<n; i++)
	{
		lastrow[i] = new int[m];
		M[i] = new double[m];
		M[i][0] = getEnergy(i, 0);
	}
	for(int j=1; j<m; j++)
	{
		double e1, e2, e3;
		e2 = M[0][j-1];
		e3 = M[1][j-1];
		M[0][j] = getEnergy(0, j) + ((e3<e2)?e3:e2);
		lastrow[0][j] = ((e3<e2)?1:0);
		for(int i=1; i<n-1; i++)
		{
			e1 = M[i-1][j-1];
			e2 = M[i][j-1];
			e3 = M[i+1][j-1];
			if(e1 <= e2 && e1 <= e3)
			{
				M[i][j] = getEnergy(i, j) + e1;
				lastrow[i][j]= i - 1;
			}
			else if(e2 < e1 && e2 <= e3)
			{
				M[i][j] = getEnergy(i, j) + e2;
				lastrow[i][j]= i;
			}
			else
			{
				M[i][j] = getEnergy(i, j) + e3;
				lastrow[i][j]= i + 1;
			}
		}
		e1 = M[n-2][j-1];
		e2 = M[n-1][j-1];
		M[n-1][j] = getEnergy(n-1, j) + ((e2<e1)?e2:e1);
		lastrow[n-1][j] = ((e2<e1)?n-1:n-2);
	}
	for(int i=0; i<n; i++)
	{
		if(M[i][m-1] < minE)
		{
			minE = M[i][m-1];
			lasti = i;
		}
	}
	if(lasti == -1)
		cout << "fail to find seam by row" << endl;
	RowSeam = new Point[m];
	RowSeam[m-1] = Point(lasti, m-1);
	for(int j=m-2; j>=0; j--)
		RowSeam[j] = Point(lastrow[RowSeam[j+1].x][j+1], j);
	for(int i=0; i<n; i++)
	{
		delete []M[i];
		delete []lastrow[i];
	}
	delete []M;
	delete []lastrow;
	return minE;
}

double Pic::findCol()
{
	double minE = 1000000.0;
	int lastj =-1;
	double** M = new double*[n];
	int** lastline = new int*[n];
	for(int i=0; i<n; i++)
	{
		lastline[i] = new int[m];
		M[i] = new double[m];
	}
	for(int j=0; j<m; j++)
		M[0][j] = getEnergy(0, j);

	for(int i=1; i<n; i++)
	{
		double e1, e2, e3;
		e2 = M[i-1][0];
		e3 = M[i-1][1];
		M[i][0] = getEnergy(i, 0) + ((e3<e2)?e3:e2);
		lastline[i][0] = ((e3<e2)?1:0);
		for(int j=1; j<m-1; j++)
		{
			e1 = M[i-1][j-1];
			e2 = M[i-1][j];
			e3 = M[i-1][j+1];
			if(e1 <= e2 && e1 <= e3)
			{
				M[i][j] = getEnergy(i, j) + e1;
				lastline[i][j]= j - 1;
			}
			else if(e2 < e1 && e2 <= e3)
			{
				M[i][j] = getEnergy(i, j) + e2;
				lastline[i][j]= j;
			}
			else
			{
				M[i][j] = getEnergy(i, j) + e3;
				lastline[i][j]= j + 1;
			}
		}
		e1 = M[i-1][m-2];
		e2 = M[i-1][m-1];
		M[i][m-1] = getEnergy(i, m-1) + ((e2<e1)?e2:e1);
		lastline[i][m-1] = ((e2<e1)?m-1:m-2);
	}
	for(int j=0; j<m; j++)
	{
		if(M[n-1][j] < minE)
		{
			minE = M[n-1][j];
			lastj = j;
		}
		//cout << M[n-1][j]<< endl;
	}
	if(lastj == -1)
		cout << "fail to find seam by col" << endl;
	ColSeam = new Point[n];
	ColSeam[n-1] = Point(n-1, lastj);
	for(int i=n-2; i>=0; i--)
		ColSeam[i] = Point(i, lastline[i+1][ColSeam[i+1].y]);
	for(int i=0; i<n; i++)
	{
		delete []M[i];
		delete []lastline[i];
	}
	delete []M;
	delete []lastline;
	return minE;
}
