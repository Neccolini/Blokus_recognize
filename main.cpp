#include <opencv2/opencv.hpp>
#include "BlokusRecognize.hpp"
#include "findOptimalMove.hpp"
using namespace std;

const char *preset_file="pic2.jpg";//sample picture


//debug
void printboard(Board &board){
    cout<<"begin"<<endl;
    for(auto v:board){for(auto b:v)cout<<b<<" ";cout<<endl;}
    cout<<"end"<<endl;
}
int main(int argc, char *argv[]){
    cv::Mat input=cv::imread(preset_file,1);
    cv::VideoCapture cap;
    std::string input_index;
    cv::namedWindow("result",1);
    if(argc>1){
        input_index=argv[1];
        cap.open(input_index);
    }else{
        cap.open(0);
    }
    cv::Mat frame,result;
    if(!cap.isOpened()){
        printf("no input video\n");
        return 0;
    }
    bool loop_flag=true; 
    int now=1;
    int i=0;
    while(loop_flag){
        i++;
        if(i%4==0)cap>>frame;

        if(frame.empty()){
            frame.release();
            continue;
        }
        cv::resize(frame, frame,cv::Size(), 800.0/frame.cols, 800.0/frame.cols);
        int k=cv::waitKey(27);
        switch(k){
            case 'q':
            loop_flag=false;
            break;
            case 'c':
            now=3-now;
            break;
        }
        Board board=BlokusRecognize(frame);
        Block ans;
        Point ans_p;
        ans_p=findOptimalMove(board,now,ans);
        cv::Point point;
        point.x=ans_p.x,point.y=ans_p.y;
        DisplayAnswer(point,ans,now);
        result=DisplayAnswerInOriginalPic();
        if(result.empty())cv::imshow("result",frame);
        else cv::imshow("result",result);
        ReleaseALL();
        frame.release();
    }
    return 0;
}

/*
todo:

*/

