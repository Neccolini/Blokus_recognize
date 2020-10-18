#include <opencv2/opencv.hpp>
#include "BlokusRecognize.hpp"
#include "findOptimalMove.hpp"
using namespace std;

const char *preset_file="./pic2.jpg";//sample picture

int main(int argc, char *argv[]){
    const char *input_file;
    if(argc==2){
        input_file=argv[1];
    }
    else{
        input_file=preset_file;
    }
    //読み込み
    cv::Mat input=cv::imread(input_file,1);
    if(input.empty()){
        fprintf(stderr, "cannot open %s\n", input_file);
        exit(0);
    }
    Board board;
    board=BlokusRecognize(input);
    for(auto z:board){for(auto x:z)cout<<x<<" ";cout<<endl;}
    Block ans;
    Point ans_p;
    ans_p=findOptimalMove(board, 2, ans);
    printBlock(ans);
    cout<<ans_p.y<<" "<<ans_p.x<<endl;
    cv::Point point;
    point.x=ans_p.x,point.y=ans_p.y;
    DisplayAnswer(point,ans,2);
    DisplayAnswerInOriginalPic();
}

/*
todo:
すでに置かれているものを候補から排除する(Done)
答えを盤に表示する(done)
(再度透視変換して戻す)(done)
動画に対応
キーボード操作で帰るほうが良さそう
高速化のために最初に画像サイズを縮小する
認識できなかったときに正常に終了するか確認
*/