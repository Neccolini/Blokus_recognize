#ifndef INCLUDED_BLOKUS_RECOGNIZE_h

#define INCLUDED_BLOKUS_RECOGNIZE_h

#ifndef opencv2_opencv
#include "opencv2/opencv.hpp"
#endif

#ifndef vector
#include "vector"
#endif

#ifndef iostream
#include "iostream"
#endif

#ifndef string
#include "string"
#endif

//ブロックスの盤領域を取り出すためのhsvの範囲
#define H_MAX 200
#define H_MIN 0
#define S_MAX 120
#define S_MIN 0
#define V_MAX 255
#define V_MIN 120
const int th=122;//２値化の際の閾値
const int _size=861;//透視変換後の大きさ
const double ratio=29.6/31.8;//碁盤目の以外の橋の部分を削除するときの割合(長さを測りました)
static cv::Mat img,img2,gray,binary,hsv, mask,output, board,dst;
struct hsvColor{
    int hmax,hmin,smax,smin,vmax,vmin;
};

using Block=std::vector<std::vector<int>>;
using Board=std::vector<std::vector<int>>;
static std::vector<cv::Point> rect_vertexes;//盤の角の位置を格納
bool comparex(cv::Point &p1,cv::Point &p2);
bool comparey(cv::Point &p1,cv::Point &p2);

std::vector<std::vector<int> > BlokusRecognize(cv::Mat &input);
cv::Point minPoint(std::vector<cv::Point> contours);
cv::Point maxPoint(std::vector<cv::Point> contours);
void plotRegion(cv::Mat &img, std::vector<cv::Point> region);//デバッグ関数
int BlockColor(cv::Vec3b color);
void DisplayAnswer(cv::Point point, Block block, int color);
void saveBoardPicture(char *filename);
cv::Mat DisplayAnswerInOriginalPic();
void ReleaseALL();
#endif