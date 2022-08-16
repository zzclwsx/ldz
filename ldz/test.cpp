#include<iostream>
#include<opencv2/opencv.hpp>
using namespace cv;
using namespace std;

//计算cols列上的像素值和
int getcolssum(Mat &src,int cols){
    int result = 0;
    for (int i = 0;i<src.rows;i++){
        result += src.at<uchar>(i,cols);
    }
    return result;
}
//计算rows行上的像素值和
int getrowssum(Mat &src,int rows){
    int result = 0;
    for (int i = 0;i<src.cols;i++){
        result += src.at<uchar>(rows,i);
    }
    return result;
}

//上下剪切图像
int cut2(Mat &dst,Mat &result){
    int up = 0,down;
    for (int i = 0;i<dst.rows;i++){
        int rows_value = getrowssum(dst,i);
        if (rows_value > 0){
            up = i;
            break;
        }
    }
    if (up == 0){
        return 1;
    }
    for (int i = up;i<dst.rows;i++){
        int rows_value = getrowssum(dst,i);
        if (rows_value == 0){
            down = i;
            break;
        }
    }
    int height = down - up;
    Rect roi(0,up,dst.cols,height);
    result = dst(roi).clone();
    return 0;
}

//左右剪切图像
int cut1(Mat &src,Mat &result){
    int left = 0,right;
    int cols_value;

    //单个数字的左界限
    for (int i = 0;i<src.cols;i++){
        cols_value = getcolssum(src,i);
        if (cols_value > 0){
            left = i;
            break;
        }
    }
    if (left == 0){
        return 1;
    }
    //右界限
    for (int i = left;i<src.cols;i++){
        cols_value = getcolssum(src,i);
        if (cols_value == 0){
            right = i;
            break;
        }
    }
    int w = right-left;
    Mat dst;
    Rect roi(left,0,w,src.rows);
    //cout<<"left "<<left<<" w "<<w<<" src.rows "<<src.rows<<endl;
    //rectangle(src,roi,Scalar(255,255,166),1,8);
    //imshow("hey!!dst",src);
    //waitKey(0);
    dst = src(roi).clone();
    
    cut2(dst,result);
    return 0;
}
int main (){
    string path = "C:/Users/Lenovo/Desktop/project/number.png";
    Mat img = imread(path);
    Mat dst;
    cvtColor(img,dst,COLOR_BGR2GRAY);
    threshold(dst,dst,200,255,THRESH_BINARY_INV);
    Mat result;
    cut1(dst,result);
    imshow ("hey!",result);
    imshow ("origin",img);
    waitKey(0);
}