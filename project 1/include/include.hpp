#include<iostream>
#include<opencv2/opencv.hpp>
#include<vector>

//计算cols列上的像素值和
int getcolssum(Mat &src,int cols);

//计算rows行上的像素值和
int getrowssum(Mat &src,int rows);

//上下剪切图像
int cut2(Mat &dst,Mat &result);

//左右剪切图像
int cut1(Mat &src,Mat &result,Mat &rImg);

//第一个在主函数中调用的函数，前面的函数为这个函数做准备。。
vector<Mat> moban(Mat img);

//剪切白边
Rect cutwhite(Mat img);

//确定roi区域，，
Rect findroi(Mat img);

//对比两张图片的像素重合多少，通过like返回到调用者
int my_compare(Mat a,Mat b);

//主要调用my_compare函数来找出最相似的图片，先将模板和待处理图片都resize到统一尺寸方便操作
void check(vector<Mat>moban,vector<Mat>target);

