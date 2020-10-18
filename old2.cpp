#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;
using Block=vector<vector<int>>;
using Board=vector<vector<int>>;
struct Point{
    int y,x;
};

const int dx[]={1,0,-1,0},dy[]={0,1,0,-1};//縦横のマス
const int ex[]={1,1,-1,-1},ey[]={-1,1,1,-1};//斜めのマス

//点数計算の仕組みを考える
//ブロックリストを作る
const vector<Block> red_block_list={
    {{1}}, {{1,1}}, {{1,1,1}}, {{1,1}, {0,1}}, //1,2,3
    {{1,1,1,1}}, {{1,1,1},{0,0,1}}, {{1,1,1},{0,1,0}}, {{1,1},{1,1}}, {{1,1,0},{0,1,1}}, //4
    {{1,1,1,1,1}}, {{1,1,1,1},{0,0,0,0,1}}, {{1,1,1,0},{0,0,1,1}}, {{1,1,1},{0,1,1}}, //5
    {{1,1,1},{1,0,1}}, {{1,1,1,1},{0,0,1,0}}, {{1,1,1},{0,1,0},{0,1,0}}, //5
    {{1,0,0},{1,0,0},{1,1,1}},{{1,1,0},{0,1,1},{0,0,1}}, {{1,0,0},{1,1,1},{0,0,1}}, //5
    {{1,0,0},{1,1,1},{0,1,0}}, {{0,1,0},{1,1,1},{0,1,0}} //5
};

const vector<Block> blue_block_list={
    {{2}}, {{2,2}}, {{2,2,2}}, {{2,2}, {0,2}}, //1,2,3
    {{2,2,2,2}}, {{2,2,2},{0,0,2}}, {{2,2,2},{0,2,0}}, {{2,2},{2,2}}, {{2,2,0},{0,2,2}}, //4
    {{2,2,2,2,2}}, {{2,2,2,2},{0,0,0,0,2}}, {{2,2,2,0},{0,0,2,2}}, {{2,2,2},{0,2,2}}, //5
    {{2,2,2},{2,0,2}}, {{2,2,2,2},{0,0,2,0}}, {{2,2,2},{0,2,0},{0,2,0}}, //5
    {{2,0,0},{2,0,0},{2,2,2}},{{2,2,0},{0,2,2},{0,0,2}}, {{2,0,0},{2,2,2},{0,0,2}}, //5
    {{2,0,0},{2,2,2},{0,2,0}}, {{0,2,0},{2,2,2},{0,2,0}} //5
};

//一つのブロックに対し上下反転などの８パターンを計算する
void createEightPatterns(Block block, vector<Block> &created);

//第二引数のblockを置ける最適な場所を計算, 第三引数はblockの色
Point findPlaceToPut(Board board, Block block,int color,Block &ans);
void printBlock(Block b);
int check(const Board &board,const Point point, const Block b,int color, Point &ans);
int main(){
    Board board(20,vector<int>(20));
    for(int i=0;i<20;i++)for(int j=0;j<20;j++)cin>>board[i][j];
    //Block a={{0,1,0,0},{1,1,1,1}};
    vector<Block> res(8);//必ず8個と指定
    Block ans;//どのブロックを使うか（答え）
    Point ans_p;//どこに置くか(ブロックの右上の座標)(答え)
    for(auto a:blue_block_list)ans_p=findPlaceToPut(board,a,2, ans);
    cout<<ans_p.y<<" "<<ans_p.x<<endl;
    printBlock(ans);
}


void createEightPatterns(Block block, vector<Block> &created){
    int height=block.size();
    int width=block[0].size();

    //もとのやつ
    created[0]=block;
    //上下反転
    for(int i=height-1;i>=0;i--){
        created[1].emplace_back(block[i]);
    }
    //左右反転
    created[2]=block;
    for(int i=0;i<height;i++){
        std::reverse(created[2][i].begin(),created[2][i].end());
    }
    //上下左右反転
    created[3]=created[1];
    for(int i=0;i<height;i++)std::reverse(created[3][i].begin(),created[3][i].end());
    
    //90度回転->左右反転
    Block n(width,vector<int>(height));
    for(int i=0;i<width;i++)for(int j=0;j<height;j++){
        n[i][j]=block[j][i];
    }
    created[4]=n;
    //90度回転
    created[5]=n;
    for(int i=0;i<width;i++)std::reverse(created[5][i].begin(),created[5][i].end());

    //90度回転->上下左右回転
    for(int i=width-1;i>=0;i--){
        created[6].emplace_back(n[i]);
    }
    
    //90度回転->上下回転
    for(int i=width-1;i>=0;i--){
        created[7].push_back(created[5][i]);
    }

}

void printBlock(Block b){
    for(auto h:b){
        for(auto w:h)cout<<w<<" ";
        cout<<endl;
    }
}

int mx=0;
Point ans_p;//置かれるブロックの右端の座標
Block ans_block;//ブロックの形（向き）
//ans_Bは当てはめるときのブロックの形 
Point findPlaceToPut(Board board, Block block, int color,Block &ans_B){

    vector<Point>temp_list;//おける場所のリスト（仮）
    for(int yi=0;yi<20;yi++){
        for(int xj=0;xj<20;xj++){
            if(board[yi][xj]!=0)continue;
            bool able_put=true;
            //縦横のマスに自分と同じ色のブロックが置かれていないか
            for(int k=0;k<4;k++){
                int nx=xj+dx[k],ny=yi+dy[k];
                if(nx<0 || ny<0 || nx>=20 || ny>=20)continue;
                if(board[ny][nx]==color){
                    able_put=false;
                    break;
                }
            }
            if(!able_put)continue;
            able_put=false;
            //斜めのマスに自分と同じ色のブロックが置かれているかどうか
            for(int k=0;k<4;k++){
                int nx=xj+ex[k],ny=yi+dy[k];
                if(nx<0 || ny<0 || nx>=20 || ny>=20)continue;
                if(board[ny][nx]==color)able_put=true;
            }
            if(able_put){
                Point t;
                t.x=xj,t.y=yi;
                temp_list.emplace_back(t);
            }
        }
    }

    vector<Block>res(8);
    createEightPatterns(block,res);
    Point temp;

    for(auto point:temp_list){
        for(auto b:res){
            int score=check(board,point,b,color,temp);
            cout<<"2 score:mx"<<score<<" "<<mx<<endl;
            if(score>=mx){
                ans_p=temp;
                ans_block=b;
                mx=score;
                cout<<"mx="<<mx<<endl;
            }
            cout<<"3 score:mx"<<score<<" "<<mx<<endl;
            }
        }
    ans_B=ans_block;
    return ans_p;
}


int check(const Board &board, const Point point, const Block b, int color,Point &ans){
    int height=b.size(),width=b[0].size();
    int res=0;//相手のブロックと接する回数を最大化
    //(yi,xj)を(point.y,point.x)に合わせる
    for(int yi=0;yi<height;yi++)for(int xj=0;xj<width;xj++){
        if(b[yi][xj]==0)continue;
        //(y,x)はboardでの、置きたいブロックの右上の座標
        int y=point.y-yi,x=point.x-xj;
        if(y<0 || x<0)continue;
        int cnt=0;//相手のブロックと接する回数
        bool able=true;
        for(int i=0;i<height;i++)for(int j=0;j<width;j++){
            if(b[i][j]==0){
                //block_listの17, 18について考える必要がある(未)
                if((0<=y+i && y+i<20 && 0<=x+j && x+j<20) && board[y+i][x+j]==color)able=false;
                else if((0<=y+i && y+i<20 && 0<=x+j && x+j<20) && board[y+i][x+j]!=0)cnt++;
                continue;
            }
            if(y+i>=20 || x+j>=20)able=false;
            if(board[y+i][x+j]!=0)able=false;
            if(i==0){
                if(y!=0 && board[y+i-1][x+j]==color)able=false;//上のますが同じ色
                else if(y!=0 && board[y+i-1][x+j]!=0)cnt++;
            }
            if(i==height-1){
                if(y!=19 && board[y+i+1][x+j]==color)able=false;//下のマスが同じ色
                else if(y!=19 && board[y+i+1][x+j]!=0)cnt++;

            }
            if(j==0){
                if(x!=0 && board[y+i][x+j-1]==color)able=false;//左のマスが同じ色
                else if(x!=0 && board[y+i][x+j-1]!=0)cnt++;
            }
            if(j==width-1){
                if(x!=19 && board[y+i][x+j+1]==color)able=false;//右のマスが同じ色
                else if(x!=19 && board[y+i][x+j-1]!=0)cnt++;
            }

        }
        if(able){
            printf("point:(%d %d) score:%d\n",y,x,cnt);
            cout<<"Block"<<endl;
            printBlock(b);
            cout<<endl;
            if(cnt>=res){
                Point t;
                t.x=x,t.y=y;
                ans=t;
                res=cnt;
            }
        }
    }
    
    return res;
}   
