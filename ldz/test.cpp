#include<iostream>
#include<opencv2/opencv.hpp>
#include<vector>
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
int cut1(Mat &src,Mat &result,Mat &rImg){
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
    dst = src(roi).clone();
    // dst 现在是左右剪切过的，传到cut2函数进行上下剪切

    Rect rest(right,0,src.cols-right,src.rows);
    rImg = src(rest).clone();
    //rImg 剪切后剩余的图像

    cut2(dst,result);
    return 0;
}
vector<Mat> moban(Mat img){
    Mat result,rImg;
    cvtColor(img,img,COLOR_BGR2GRAY);
    threshold(img,img,150,255,THRESH_BINARY);
    int flag = cut1(img,result,rImg);
    vector<Mat> templ;
    int i = 0;
    while(flag == 0){
        cut1(img,result,rImg);
        templ.push_back(result.clone());
        i++;
        img = rImg.clone();
        flag = cut1(img,result,rImg);
    }//存放模板
    return templ;
}
void onchange(int tl,void* img){
    Mat image = (*(Mat*)img).clone();
}
Rect findroi(Mat img){
    int up = 0,left = 0,right = img.cols,w = 50;
    namedWindow("find roi",WINDOW_AUTOSIZE);
    createTrackbar("posision","find roi",&up,img.rows-w);
    createTrackbar("right","find roi",&right,img.cols);
    createTrackbar("left","find roi",&left,right);
    createTrackbar("w","find roi",&w,img.rows-up);
    imshow("origin",img);
    
    while (1)
    {
        Rect roi(Point(left,up),Point(right,up+w));
        Mat dst = img(roi).clone();
        imshow("roi",dst);
        int key;
        key = waitKey(1);
        if (key == 13){
            return roi;
            break;
        }
    }
    waitKey(0);
}
int my_compare(Mat a,Mat b){
    int like = 0;
    for (int i = 0;i<a.rows;i++){
        for(int j = 0;j<a.cols;j++){
            if (a.at<uchar>(i,j) == b.at<uchar>(i,j))
                like++;
        }
    }
    return like;
}
void check(vector<Mat>moban,vector<Mat>target){
    int maxidx,max = 0;
    for (int i = 0;i<target.size();i++)
        resize(target[i],target[i],Size(8,8),0,0);
    for (int i = 0;i<moban.size();i++)
        resize(moban[i],moban[i],Size(8,8),0,0);
    for (int i = 0;i<target.size();i++){
        for(int j = 0;j<moban.size();j++){
            if (my_compare(target[i],moban[j])>max){
                max = my_compare(target[i],moban[j]);
                maxidx = j;
            }
        }
        cout<<"No. "<<i<<"'s result is "<<maxidx<<endl;
        max = 0;
        maxidx = 0;
    }
    
}
int main (){
    string m = "C:/Users/Lenovo/Desktop/project/number.png";
    Mat img = imread(m);
    bitwise_not(img,img);
    vector<Mat> mb = moban(img);//制作模板并且将0-9数字存放到mb变量中
    string cardPath = "C:/Users/Lenovo/Desktop/project/credit_card_03.png";
    Mat card = imread(cardPath);
    Rect rect = findroi(card);
    vector<Mat> temp = moban(card(rect));
    check(mb,temp);
    system("pause");
}