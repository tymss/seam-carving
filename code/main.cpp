#include "task.h"

//funcname:sobel, scharr, laplace

void task1(string funcname, string filename)
{
	string fname = ".\\" + filename;
	string rname = ".\\result" + filename;
	string cname = ".\\contrast" +filename;
	BiRemoval(funcname, fname, rname, cname);
}

void task2(string funcname, string filename, int rate)
{
	string fname = ".\\" + filename;
	string rname = ".\\result" + filename;
	string cname = ".\\contrast" +filename;
	EnlargeCols(funcname, fname, rname, cname, rate);
}

void task3(string funcname, string filename, int flag, Point* poly, int n_poly) //flag = 0: no area removal or prevent(=task1);  
{                                                                               //flag = 1: area removal;
	string fname = ".\\" + filename;											//flag = 2: area prevent
	string rname = ".\\result" + filename;										//point.x means col and point.y means row
	string cname = ".\\contrast" +filename;
	BiRemoval(funcname, fname, rname, cname, flag, poly, n_poly);
}

int main()
{
	Point* p = new Point[4];
	p[0] = Point(401, 97);
	p[1] = Point(402, 119);
	p[2] = Point(490, 55);
	p[3] = Point(490, 9);
	task3("sobel", "9.jpg", 2, p, 4);
	return 0;
}