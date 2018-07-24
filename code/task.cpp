#include "task.h"

void copyseam(Point* s, Point* t, int l)
{
	for(int i=0; i<l; i++)
	{
		//cout << s[i] <<endl;
		t[i] = s[i];
	}
}

void BiRemoval(string funcname, string filename, string rname, string cname, int flag, Point* poly, int n_poly)
{
	Pic* src = new Pic(funcname, filename);
	if(flag == 1)
		src->setextra(1, poly, n_poly);
	else if(flag == 2)
		src->setextra(0, poly, n_poly);
	Mat srcmat = src->getdata();
	int n = srcmat.rows, m = srcmat.cols;
	int new_n = n * 0.8, new_m = m * 0.8;
	int r = n - new_n, c = m - new_m;
	Point** seam = new Point*[r+c];
	bool* isByRow = new bool[r+c];
	int num_row = 0, num_col = 0;
	Pic* mat;
	for(int i=0; i<r+c; i++)
	{
		cout<<"generating:i="<<i<<endl;
		if(num_row < r && num_col <c)
		{
			double e_r = src->findRow();
			double e_c = src->findCol();
			if(e_r <= e_c)
			{
				mat = new Pic(src, src->getRowSeam(), 1);
				isByRow[i] = 1;
				seam[i] = new Point[src->getdata().cols];
				copyseam(src->getRowSeam(), seam[i], src->getdata().cols);
				Pic *temp = src;
				src = mat;
				mat = NULL;
				delete temp;
				num_row++;
			}
			else
			{
				mat = new Pic(src, src->getColSeam(), 0);
				isByRow[i] = 0;
				seam[i] = new Point[src->getdata().rows];
				copyseam(src->getColSeam(), seam[i], src->getdata().rows);
				Pic *temp = src;
				src = mat;
				mat = NULL;
				delete temp;
				num_col++;
			}
		}
		else if(num_row == r)
		{
			double e_c = src->findCol();
			mat = new Pic(src, src->getColSeam(), 0);
			isByRow[i] = 0;
			seam[i] = new Point[src->getdata().rows];
			copyseam(src->getColSeam(), seam[i], src->getdata().rows);
			Pic *temp = src;
			src = mat;
			mat = NULL;
			delete temp;
			num_col++;
		}
		else if(num_col == c)
		{
			double e_r = src->findRow();
			mat = new Pic(src, src->getRowSeam(), 1);
			isByRow[i] = 1;
			seam[i] = new Point[src->getdata().cols];
			copyseam(src->getRowSeam(), seam[i], src->getdata().cols);
			Pic *temp = src;
			src = mat;
			mat = NULL;
			delete temp;
			num_row++;
		}
	}
	cout<<"finish creating result"<<endl;
	src->output(rname);
	cout<<"start generating contrast picture"<<endl;
	Mat result = src->getdata();
	for(int t=r+c-1; t>=0; t--)
	{
		cout<<"contrast:t="<<t<<endl;
		if(isByRow[t])
		{
			Mat newmat(result.rows+1, result.cols, CV_64FC3);
			for(int j=0; j<result.cols; j++)
			{
				int b = seam[t][j].x;
				for(int i=0; i<b; i++)
					newmat.at<Vec3d>(i, j) = result.at<Vec3d>(i, j);
				newmat.at<Vec3d>(b, j) = Vec3d(0., 0., 1.);
				for(int i=b+1; i<result.rows+1; i++)
					newmat.at<Vec3d>(i, j) = result.at<Vec3d>(i-1, j);
			}
			result = newmat;
		}
		else
		{
			Mat newmat(result.rows, result.cols+1, CV_64FC3);
			for(int i=0; i<result.rows; i++)
			{
				int b = seam[t][i].y;
				for(int j=0; j<b; j++)
					newmat.at<Vec3d>(i, j) = result.at<Vec3d>(i, j);
				newmat.at<Vec3d>(i, b) = Vec3d(0., 0., 1.);
				for(int j=b+1; j<result.cols+1; j++)
					newmat.at<Vec3d>(i, j) = result.at<Vec3d>(i, j-1);
			}
			result = newmat;
		}
	}
	Mat img;
	result.convertTo(img, CV_8UC3, 255.);
	imwrite(cname, img);
	cout << "finish contrast pic" << endl;
}

void EnlargeCols(string funcname, string filename, string rname, string cname, int n_time)
{
	Pic* src = new Pic(funcname, filename);
	Mat srcmat = src->getdata();
	Mat srccon(srcmat.rows, srcmat.cols, CV_64FC3);
	srcmat.copyTo(srccon);
	int time = 0;
	while(++time <= n_time)
	{
		int n = src->getdata().rows, m = src->getdata().cols;
		int seams_c = m * 0.1;
		Point **col_seams;
		col_seams = new Point*[seams_c];
		for(int i=0; i<seams_c; i++)
		{
			cout << "finish seam:" << i << endl;
			col_seams[i] = new Point[n];
			src->findCol();
			copyseam(src->getColSeam(), col_seams[i], n);
			Pic* newpic = new Pic(src, col_seams[i], 0);
			Pic* temp = src;
			src = newpic;
			delete temp;
			newpic = NULL;
		}
		bool **seammap;
		seammap = new bool*[n];
		for(int i=0; i<n; i++)
			seammap[i] = new bool[m]();
		for(int t=seams_c-1; t>=0; t--)
		{
			for(int i=0; i<n; i++)
			{
				int b = col_seams[t][i].y;
				for(int j=m-t-1; j>b; j--)
					seammap[i][j] = seammap[i][j-1];
				seammap[i][b] = 1;
			}
		}	
		cout << "finish seammap" << endl;
		Mat newmat(n, m+seams_c, CV_64FC3);
		Mat contrast(n, m+seams_c, CV_64FC3);
		for(int i=0; i<n; i++)
		{
			int j = 0, add = 0;
			while(j+add < m+seams_c)
			{//if(i==394) cout<<"j="<<j<<"  add="<<add<<"  map="<<seammap[i][j]<<endl;
				if(!seammap[i][j])
				{
					newmat.at<Vec3d>(i, j+add) = srcmat.at<Vec3d>(i, j);
					contrast.at<Vec3d>(i, j+add) = srccon.at<Vec3d>(i, j);
					j++;
				}
				else
				{
					Vec3d start = (j == 0 ? Vec3d(0, 0, 0) : srcmat.at<Vec3d>(i, j-1));
					Vec3d mid = srcmat.at<Vec3d>(i, j);
					Vec3d end = (j == m-1 ? Vec3d(0, 0, 0) : srcmat.at<Vec3d>(i, j+1));
					newmat.at<Vec3d>(i, j+add) = (mid - start) / 2. + start;
					contrast.at<Vec3d>(i, j+add) = Vec3d(0, 0, 1);
					newmat.at<Vec3d>(i, j+add+1) = end - (end - mid) / 2.;
					contrast.at<Vec3d>(i, j+add+1) = Vec3d(0, 0, 1);
					j++;
					add++;
				}
			}
		}
		for(int i=0; i<n; i++)
			delete []seammap[i];
		for(int i=0; i<seams_c; i++)
			delete col_seams[i];
		delete []seammap;
		delete []col_seams;
		srcmat = newmat;
		srccon = contrast;
		Pic *temp = src;
		src = new Pic(funcname, srcmat);
	}
	Mat im, ic;
	srcmat.convertTo(im, CV_8UC3, 255);
	srccon.convertTo(ic, CV_8UC3, 255);
	imwrite(rname, im);
	imwrite(cname, ic);
}