int getcolssum(Mat &src,int cols){
    int result = 0;
    for (int i = 0;i<src.rows;i++){
        result += src.at<uchar>(i,cols);
    }
    return result;
}

int getrowssum(Mat &src,int rows){
    int result = 0;
    for (int i = 0;i<src.cols;i++){
        result += src.at<uchar>(rows,i);
    }
    return result;
}

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
    int gray = 0;
    for (int i = left;i<src.cols;i++){
        cols_value = getcolssum(src,i);
        if (cols_value == 0){
            gray++;
            if (gray<2)
                continue;
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
    add(img,Scalar(5,5,5),img);
    cvtColor(img,img,COLOR_BGR2GRAY);
    threshold(img,img,150,255,THRESH_BINARY);
    int flag = cut1(img,result,rImg);
    vector<Mat> templ;
    int i = 0;
    Mat kernal = getStructuringElement(MORPH_RECT,Size(3,3));
    while(flag == 0){
        cut1(img,result,rImg);
        dilate(result,result,kernal);
        templ.push_back(result.clone());
        i++;
        img = rImg.clone();
        flag = cut1(img,result,rImg);
    }//存放模板
    return templ;
}

//剪切白边
Rect cutwhite(Mat img){
	cvtColor(img,img,COLOR_BGR2GRAY);
	threshold(img,img,150,255,THRESH_BINARY);
	int rvalue = getrowssum(img,0);
	int cvalue = getcolssum(img,0);
	int up = 0,left = 0,right = img.cols-1,down = img.rows-1;
	if (cvalue>10){
		while(getcolssum(img,left) > (cvalue-100)){
			left++;
		}
	}
	cvalue = getcolssum(img,right);
	if (cvalue>100){
		while (getcolssum(img,right) > (cvalue-100)){
			right--;
		}
	}
	if (rvalue>10){
		while(getrowssum(img,up) > (rvalue - 100)){
			up++;
		}
	}
	rvalue = getrowssum(img,down);
	if (rvalue>10){
		while (getrowssum(img,down) > (rvalue-100)){
			down--;
		}
	}
	return Rect(Point(left,up),Point(right,down));
}
//确定roi区域，，
Rect findroi(Mat img){
    subtract(img,Scalar(16,16,16),img);
	cvtColor(img,img,COLOR_BGR2GRAY);
	threshold(img,img,150,255,THRESH_BINARY);
	int flag,lenth = 0,gray = 0,white = 0;
	int rowidx,colsidx,maxcolsidx,max=0;
	double rate;
	for (int i = 0;i<img.rows;i++){
		bool first = true;
		for (int j = 0;j<img.cols;j++){
			flag = img.at<uchar>(i,j);
			if (flag > 0&&first){
				colsidx = j;
				first = false;
				lenth++;
			}else if (flag>0&&first == false){
				lenth++;
				white++;
				gray = 0;
				if (white > 60)
					break;
			}else if (flag == 0&&first == false){
				white = 0;
				lenth++;
				gray++;
				if (gray>75)
					break;
			}
		}
		if (lenth>max){
			max = lenth;
			rowidx = i;
			rate = max*1.0/img.cols*1.0;
			
			maxcolsidx = colsidx;
			if (max > 400||rate>0.85)
				break;
		}
		lenth = 0;
		gray = 0;
	}
	int value = getrowssum(img,rowidx);
	int i = rowidx,w = 0;
	while (value>300){
        if (w>45)
            break;
		value = getrowssum(img,i);
		i++;
		w++;
	}
	Rect rect(maxcolsidx-5,rowidx-2,max-20,w+2);
    imshow("roi",img(rect));
	return rect;
}

//对比两张图片的像素重合多少，通过like返回到调用者
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

//主要调用my_compare函数来找出最相似的图片，先将模板和待处理图片都resize到统一尺寸方便操作
void check(vector<Mat>moban,vector<Mat>target){
    char result[20];
    string ret;
    int maxidx,max = 0;
    for (int i = 0;i<target.size();i++)
        resize(target[i],target[i],Size(12,15),0,0);
    for (int i = 0;i<moban.size();i++)
        resize(moban[i],moban[i],Size(12,15),0,0);
    for (int i = 0;i<target.size();i++){
        for(int j = 0;j<moban.size();j++){
            if (my_compare(target[i],moban[j])>max){
                max = my_compare(target[i],moban[j]);
                maxidx = j;
            }
        }
        cout<<"No. "<<i<<"'s result is "<<maxidx<<endl;
        cout<<"max = "<<max<<endl;
        char c = maxidx+'0';
		text.push_back(c);
		if (i!=0 && (i+1)%4 == 0)
			for (int i = 0;i<1;i++)
				text.push_back(' ');
        max = 0;
        maxidx = 0;
    }
}