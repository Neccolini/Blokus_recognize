#include <opencv2/opencv.hpp>
#include<stdio.h>
#include<iostream>
#include<fstream>
#include<vector>
using namespace std;

//ブロックスの盤領域を取り出すためのhsvの範囲
#define H_MAX 200
#define H_MIN 0
#define S_MAX 120
#define S_MIN 0
#define V_MAX 255
#define V_MIN 120
const int th=108;//２値化の際の閾値
const int _size=860;//透視変換後の大きさ
const double ratio=29.6/31.8;//碁盤目の以外の橋の部分を削除するときの割合(長さを測りました)
const char *preset_file="pic2.jpg";//sample picture
cv::Mat img,img2,gray,binary,input,hsv, mask,output;

cv::Point minPoint(vector<cv::Point> contours);
cv::Point maxPoint(vector<cv::Point> contours);
void plotRegion(cv::Mat &img, vector<cv::Point> region);//デバッグ関数
int BlockColor(cv::Vec3b color);
struct hsvColor{
    int hmax,hmin,smax,smin,vmax,vmin;
};



//main
int main(int argc, char *argv[]){
    const char *input_file;
    if(argc==2){
        input_file=argv[1];
    }
    else{
        input_file=preset_file;
    }
    //読み込み
    input=cv::imread(input_file,1);
    if(input.empty()){
        fprintf(stderr, "cannot open %s\n", input_file);
        exit(0);
    }

    /*Blocksの盤の部分を取り出す*/

    //色で絞り込み
    cv::cvtColor(input,hsv,CV_BGR2HSV,3);//hsvに変換
    cv::Scalar s_max=cv::Scalar(H_MAX, S_MAX, V_MAX);
    cv::Scalar s_min=cv::Scalar(H_MIN, S_MIN, V_MIN);
    cv::inRange(hsv, s_min, s_max, mask);
    input.copyTo(output,mask);
    cv::cvtColor(output, gray, CV_RGB2GRAY); //グレイスケール
    cv::threshold(gray,binary,th,255,cv::THRESH_BINARY);//２値化

    //findContours
    vector<vector<cv::Point> > contours;
    vector<cv::Vec4i> hierarchy;
    vector<cv::Point> hull;
    cv::findContours(binary,contours,hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    img=input.clone();
    int height=img.rows,width=img.cols;
    vector<cv::Point> rect_vertexes;//盤の角の位置を格納
        for(int i=0;i<contours.size(); i++){
        cv::Point minP = minPoint(contours.at(i));
        cv::Point maxP = maxPoint(contours.at(i));

        //認識された領域のうち小さいものを排除
        if(maxP.x-minP.x<width/3){
            continue;
        }
        if(maxP.y-minP.y<height/3){
            continue;
        }
        //plotRegion(img,contours.at(i));//認識に成功しているか確認
        cv::convexHull(contours[i],hull);
        double epsilon=0.005*cv::arcLength(hull,true);
        
        cv::approxPolyDP(hull,rect_vertexes,epsilon,true);
        std::cout<<rect_vertexes.size()<<std::endl;
        if(rect_vertexes.size()!=4){
            //どうにかする
        }
        //plotRegion(img,rect_vertexes);
    }
    //for(auto v:rect_vertexes)std::cout<<v.x<<" "<<v.y<<std::endl;

    /* 正方形に変換する:*/

    //まず上で求めた盤の角の順番を扱いやすい順番に並べる
    vector<cv::Point2f> before={rect_vertexes[2],rect_vertexes[1],rect_vertexes[3],rect_vertexes[0]};
    cv::Mat dst=cv::Mat::zeros(_size,_size,CV_8UC3);//変換後の行列
    //変換後の座標(転置行列)
    vector<cv::Point2f> after={cv::Point(0,0),cv::Point(0,_size),cv::Point(_size,0),cv::Point(_size,_size)};

    //homography 行列を計算
    const cv::Mat homography_matrix=cv::getPerspectiveTransform(before,after);

    //透視変換
    warpPerspective(img, dst, homography_matrix,dst.size());
    cv::imwrite("./Blocks/res.jpg",dst);

    /* 碁盤目以外の余った領域を削除 */
    int pt_x=dst.cols*(1.1/31.8),pt_y=dst.rows*(1.1/31.8);
    int b_width=dst.cols*ratio,b_height=dst.rows*ratio;
    cv::Rect roi(cv::Point(pt_x,pt_y),cv::Size(b_width,b_height));
    cv::Mat board=dst(roi);
    //cout<<board.cols<<" "<<board.rows<<endl;

    cv::Mat board_clone=board.clone();
    /* 縦横をそれぞれ20分割 */
    int split_size=(int)(board.cols/20);
    vector<vector<cv::Mat> > board_split(20,vector<cv::Mat>(20));
    for(int yi=0;yi<20;yi++)for(int xj=0;xj<20;xj++){
        cout<<yi<<" "<<xj*split_size<<" "<<split_size<<endl;
        cv::Rect roi(cv::Point(split_size*xj,split_size*yi),cv::Size(split_size,split_size));
        board_split[yi][xj]=board_clone(roi);
    }


    /* それぞれのマスの色を求める */
    //ブロックが置かれていない -> 0, 赤 -> 1, 青 -> 2
    
    cv::cvtColor(board_clone,board_clone,CV_BGR2HSV,3);//hsvに変換
    vector<vector<int> >blocks_color(20,vector<int>(20,1));
    for(int yi=0;yi<20;++yi)for(int xj=0;xj<20;++xj){
        cv::Mat block=board_split[yi][xj];
        //案1: 中心の色だけ考える, 案2: 平均の色を考える
        //計算量的に前者をとる -> 光の反射で誤りが発生
        //現時点で対処法は撮影時に強い光を入れないようにすること
        cv::Vec3b color=block.at<cv::Vec3b>(split_size/2,split_size/2);//中心の色(hsv)
        blocks_color[yi][xj]=BlockColor(color);
    }
    
     cv::Vec3b color=board_split[12][9].at<cv::Vec3b>(split_size/2,split_size/2);

    //printf("(%d %d): (%d %d %d)\n",12,9,color[0],color[1],color[2]);
    for(int i=0;i<20;i++){
        for(int j=0;j<20;j++){
            cout<<blocks_color[i][j]<<" ";
        }
        cout<<endl;
    }
    cv::imwrite("./Blocks/test/67.jpg",board_split[0][1]);
    cv::imwrite("./Blocks/test/68.jpg",board_split[6][9]);
    cv::imwrite("./Blocks/test/board.jpg",board);
    cv::imwrite("./Blocks/test/res.jpg",dst);
    cv::imwrite("./Blocks/test/test.jpg",img);
    cv::imwrite("./Blocks/test/output.jpg", output);
    cv::imwrite("./Blocks/test/binary.jpg",binary);

    return 0;
}




//認識できてるかテスト
void plotRegion(cv::Mat &img, vector<cv::Point> region){
    for(int i=0;i<region.size();i++){
        cv::Point point=region[i];
        for(int i=-5;i<=5;i++){
            cv::Vec3b *src=img.ptr<cv::Vec3b>(i+point.y);
            for(int j=0;j<=5;j++){
                for(int k=0;k<3;k++)src[j+point.x][k]=0;
            }
        }
    }
}


//hsv -> 0:なし, 1:赤, 2:青
int BlockColor(cv::Vec3b color){
    bool none=H_MIN<=color[0] && color[0]<=H_MAX && 
        S_MIN<=color[1] && color[1]<=S_MAX &&
         V_MIN<=color[2] && color[2]<=V_MAX;

    bool redh=(0<=color[0] && color[0]<=20)||(140<=color[0] && color[0]<=180);
    bool reds= (50<=color[1] && color[1]<=255);
    bool redv= (80<=color[2] && color[2]<=255);
    bool red=redh && reds && redv;
    bool blue=(83<=color[0] && color[0]<=139) &&
               (50<=color[1] && color[1]<=255) && 
                (70<=color[2] && color[2]<=255);
    if(none){
        return 0;
    }
    if(red){
        return 1;
    }
    if(blue){
        return 2;
    }
    return 0;
}

cv::Point minPoint(vector<cv::Point> contours){
    double minx=contours.at(0).x;
    double miny=contours.at(0).y;
    for(int i=0;i<contours.size();i++){
        if(minx>contours.at(i).x)minx=contours.at(i).x;
        if(miny>contours.at(i).y)miny=contours.at(i).y;
    }
    return cv::Point(minx, miny);
}

cv::Point maxPoint(vector<cv::Point> contours){
    double maxx=contours.at(0).x;
    double maxy=contours.at(0).y;
    for(int i=0;i<contours.size();i++){
        if(maxx<contours.at(i).x)maxx=contours.at(i).x;
        if(maxy<contours.at(i).y)maxy=contours.at(i).y;
    }
    return cv::Point(maxx, maxy);
}