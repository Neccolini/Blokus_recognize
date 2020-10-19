#include "BlokusRecognize.hpp"
using namespace std;


bool comparey(cv::Point &p1,cv::Point &p2){
    return p1.y<p2.y;
}
bool comparex(cv::Point &p1, cv::Point &p2){
    return p1.x<p2.x;
}
vector<vector<int> > BlokusRecognize(cv::Mat &input){

    /*Blocksの盤の部分を取り出す*/

    //色で絞り込み
    cv::cvtColor(input,hsv,CV_BGR2HSV,3);//hsvに変換
    cv::Scalar s_max=cv::Scalar(H_MAX, S_MAX, V_MAX);
    cv::Scalar s_min=cv::Scalar(H_MIN, S_MIN, V_MIN);
    cv::inRange(hsv, s_min, s_max, mask);
    input.copyTo(output,mask);
    cv::cvtColor(output, gray, CV_RGB2GRAY); //グレイスケール
    cv::threshold(gray,binary,th,255,cv::THRESH_BINARY);//２値化
    cv::namedWindow("mask",1);
    cv::imshow("mask",binary);
    cv::waitKey(2);
    //findContours
    vector<vector<cv::Point> > contours;
    vector<cv::Vec4i> hierarchy;
    vector<cv::Point> hull;
    cv::findContours(binary,contours,hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    //binary.release();
    img2=input.clone();
    img=input.clone();
    int height=img.rows,width=img.cols;
        for(int i=0;i<(int)contours.size(); i++){
        cv::Point minP = minPoint(contours.at(i));
        cv::Point maxP = maxPoint(contours.at(i));
        //認識された領域のうち小さいものを排除
        if(maxP.x-minP.x<width/3){
            continue;
        }
        if(maxP.y-minP.y<height/3){
            continue;
        }
        cv::convexHull(contours[i],hull);
        double epsilon=alpha*cv::arcLength(hull,true);
        cv::approxPolyDP(hull,rect_vertexes,epsilon,true);

        }
        if(rect_vertexes.size()!=4){
            vector<vector<int>> NULL_RETURN_VALUE;
            return NULL_RETURN_VALUE;
        }
    //ソートを実装する
    std::sort(rect_vertexes.begin(),rect_vertexes.end(),comparey);
    std::sort(rect_vertexes.begin(), rect_vertexes.begin()+2,comparex);
    std::sort(rect_vertexes.begin()+2,rect_vertexes.end(),comparex);
    /* 正方形に変換する:*/

    //まず上で求めた盤の角の順番を扱いやすい順番に並べる
    vector<cv::Point2f> before={rect_vertexes[0],rect_vertexes[1],rect_vertexes[2],rect_vertexes[3]};
    dst=cv::Mat::zeros(_size,_size,CV_8UC3);//変換後の行列
    //変換後の座標(転置行列)
    vector<cv::Point2f> after={cv::Point(0,0),cv::Point(_size,0),cv::Point(0,_size),cv::Point(_size,_size)};

    //homography 行列を計算
    const cv::Mat homography_matrix=cv::getPerspectiveTransform(before,after);
    //透視変換
    warpPerspective(img, dst, homography_matrix,dst.size());

    /* 碁盤目以外の余った領域を削除 */
    int pt_x=dst.cols*(1.1/31.8),pt_y=dst.rows*(1.1/31.8);
    int b_width=dst.cols*ratio,b_height=dst.rows*ratio;
    cv::Rect roi(cv::Point(pt_x,pt_y),cv::Size(b_width,b_height));
    board=dst(roi);

    cv::Mat board_clone=board.clone();
    /* 縦横をそれぞれ20分割 */
    int split_size=(int)(board.cols/20);
    vector<vector<cv::Mat> > board_split(20,vector<cv::Mat>(20));
    for(int yi=0;yi<20;yi++)for(int xj=0;xj<20;xj++){
        cv::Rect roi(cv::Point(split_size*xj,split_size*yi),cv::Size(split_size,split_size));
        board_split[yi][xj]=board_clone(roi);
    }
    
    cv::cvtColor(board_clone,board_clone,CV_BGR2HSV,3); //hsvに変換
    vector<vector<int> >blocks_color(20,vector<int>(20,1));
    for(int yi=0;yi<20;++yi)for(int xj=0;xj<20;++xj){
        cv::Mat block=board_split[yi][xj];
        //案1: 中心の色だけ考える, 案2: 平均の色を考える
        //計算量的に前者をとる -> 光の反射で誤りが発生
        //現時点で対処法は撮影時に強い光を入れないようにすること
        cv::Vec3b color=block.at<cv::Vec3b>(split_size/2,split_size/2);//中心の色(hsv)
        blocks_color[yi][xj]=BlockColor(color);
    }
    return blocks_color;
}



void DisplayAnswer(cv::Point point, Block block, int color){
    if(board.empty()){
        return;
    }
    if(point.x<0 || point.y<0 || point.x>=20 || point.y>=20){
        return;
    }
    if(block.size()==0){
        return;
    }
    vector<cv::Point> p_list;
    int y_height=block.size();
    int x_width=block[0].size();
    for(int yi=0;yi<y_height;yi++){
        for(int xj=0;xj<x_width;xj++){
            if(block[yi][xj]!=0){
                cv::Point t;
                t.y=point.y+yi,t.x=point.x+xj;
                p_list.emplace_back(t);
            }
        }
    }

    int b_height=board.rows,b_width=board.cols;

    int add=b_height*(1.1/29.6);//for dst
    for(cv::Point p: p_list){
        for(int i=0;i<b_height/20;i++){
            cv::Vec3b *src=board.ptr<cv::Vec3b>(b_height/20 * p.y+i);
            cv::Vec3b *dst_src=dst.ptr<cv::Vec3b>(b_height/20*p.y+i+add);
            for(int j=0;j<b_height/20;j++){
                if(color==1){
                    src[b_width/20 *p.x + j][0]=40;
                    src[b_width/20 *p.x + j][1]=40;
                    src[b_width/20 *p.x + j][2]=255;

                    dst_src[b_width/20 *p.x + j+add][0]=40;
                    dst_src[b_width/20 *p.x + j+add][1]=40;
                    dst_src[b_width/20 *p.x + j+add][2]=255;
                }
                if(color==2){
                    src[b_width/20 *p.x + j][0]=255;
                    src[b_width/20 *p.x + j][1]=40;
                    src[b_width/20 *p.x + j][2]=40;

                    dst_src[b_width/20 *p.x + j+add][0]=255;
                    dst_src[b_width/20 *p.x + j+add][1]=40;
                    dst_src[b_width/20 *p.x + j+add][2]=40;
                }
            }
        }
    }


}


//認識できてるかテスト
void plotRegion(cv::Mat &img, vector<cv::Point> region){
    for(int i=0;i<(int)region.size();i++){
        cv::Point point=region[i];
        for(int r=-5;r<=5;r++){
            cv::Vec3b *src=img.ptr<cv::Vec3b>(r+point.y);
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
    bool blue=(80<=color[0] && color[0]<=139) &&
               (50<=color[1] && color[1]<=255) && 
                (65<=color[2] && color[2]<=255);
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
    for(int i=0;i<(int)contours.size();i++){
        if(minx>contours.at(i).x)minx=contours.at(i).x;
        if(miny>contours.at(i).y)miny=contours.at(i).y;
    }
    return cv::Point(minx, miny);
}

cv::Point maxPoint(vector<cv::Point> contours){
    double maxx=contours.at(0).x;
    double maxy=contours.at(0).y;
    for(int i=0;i<(int)contours.size();i++){
        if(maxx<contours.at(i).x)maxx=contours.at(i).x;
        if(maxy<contours.at(i).y)maxy=contours.at(i).y;
    }
    return cv::Point(maxx, maxy);
}


cv::Mat DisplayAnswerInOriginalPic(){
    cv::Mat img3;
    if(rect_vertexes.size()!=4){
        //printf("ERROR: NO BOARD WAS READ.\n");
        return img3;
    }
    if(board.empty()){
        //printf("ERROR: NO BOARD WAS READ.\n");
        return img3;
    }
    
    //まず上で求めた盤の角の順番を扱いやすい順番に並べる
    vector<cv::Point2f> before={rect_vertexes[0],rect_vertexes[1],rect_vertexes[2],rect_vertexes[3]};
    //変換後の座標(転置行列)
    vector<cv::Point2f> after={cv::Point(0,0),cv::Point(_size,0),cv::Point(0,_size),cv::Point(_size,_size)};
    //homography 行列を計算
    const cv::Mat homography_matrix=cv::getPerspectiveTransform(after,before);
    

    //透視変換
    warpPerspective(dst,img2, homography_matrix,img2.size());

    cv::Mat mask_answer=img2.clone();
    cv::cvtColor(mask_answer, mask_answer, CV_RGB2GRAY); //グレイスケール
    cv::threshold(mask_answer,mask_answer, 1, 255, cv::THRESH_BINARY);//２値化
    img2.copyTo(img,mask_answer);
    return img;
}

void ReleaseALL(){
    img.release();
    img2.release();
    gray.release();
    binary.release();
    hsv.release();
    mask.release();
    output.release();
    board.release();
    dst.release();
}

