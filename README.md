# 基于seam carving的图像缩放
## 环境
* windows10
* VS2012
* OpenCV 2.4.13

## 基本功能
参考论文 Seam Carving for Content-Aware Image Resizing(Shai Avidan,Mitsubishi Electric Research Labs,Ariel Shamir,The Interdisciplinary Center & MERL)

基于seam carving实现的图像缩放及区域的保护和移除

## 文件及代码说明
### main.cpp

入口文件，可执行task函数完成对应任务。

	void task1(string funcname, string filename);	图像双向缩小20%，输入文件名filename，输出文件为contrast+filename和result+filename。 funcname可选sobel、scharr、laplace，也可自己派生新的EnergyFunc(下同)。

	void task2(string funcname, string filename, int rate);		图像水平放大，rate为放大次数，每次10%。

	void task3(string funcname, string filename, int flag, Point* poly, int n_poly); 	区域保护或填充后双向缩小20%，flag=0是无区域移除或保护效果，flag=1区域移除，flag=2区域保护。

### energy.h energy.cpp

class EnergyFunc(能量函数类的基类，可派生，需要在继承时实现CalEnergy函数，并且在pic.cpp的InitEnergyFunc函数中添加相应的情况)

	virtual void CalEnergy(Mat data) = 0;  输入图片data后计算每个像素点的能量。

class SobelFunc \ ScharrFunc \ LaplaceFunc(均继承自Energyfunc类，分别对应sobel算子、scharr算子、laplace算子) 

### pic.h pic.cpp

class pic(图像类)
	
	Pic(string funcname, string filename);		从文件中读取图像。		
	
	Pic(Pic* p, Point* s, bool row);		从原图像删除行seam或列seam构造新的图像。
	
	Pic(string funcname, Mat mat);			从mat数据构造图像。
	
	void setextra(bool re, Point* poly, int n_poly);		设置区域保护或移除。
	
	Point* getRowSeam();		返回最小能量的水平seam。
	
	Point* getColSeam();		返回最小能量的竖直seam。
	
	void output(string filename);		将图像输出到文件。
	
	double findRow();		寻找最低能量的水平seam并储存，返回该seam的能量。
	
	double findCol();		寻找最低能量的竖直seam并储存，返回该seam的能量。
	
### task.h task.cpp
	
	void BiRemoval(string funcname, string filename, string resultname, string contrastname, int flag=0, Point* poly=NULL, int n_poly=0);	双向缩小20%，可以选择区域移除或填充。

	void EnlargeCols(string funcname, string filename, string rname, string cname, int n_time);		水平放大10%，放大n_time次。