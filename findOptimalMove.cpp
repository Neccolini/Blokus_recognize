#include "findOptimalMove.hpp"

using namespace std;


Point findOptimalMove(const Board &board, const int color, Block &ans_B){
    Point res;
    if(board.empty()){
        res.x=-1,res.y=-1;
        return res;
    }
    vector<int> used_list=findUsedBlock(board, color);
    if(used_list.size()==0){
        return res;//初期状態(盤の４つの角のうちどれが青、赤のスタート地点かわからないのでスルー)
    }
    if(used_list.size()!=0 && used_list[0]<0){//異常ステータスが帰ってきたとき
        return res;
    }
    sort(used_list.begin(),used_list.end());
    mx.border=-1;
    for(int i=0;i<(int)block_list[color-1].size();i++){
        if(binary_search(used_list.begin(),used_list.end(),i))continue;
        res=findPlaceToPut(board, block_list[color-1][i], color, ans_B);
    }
    return res;
}


void printBlock(Block b){
    for(auto h:b){
        for(auto w:h)cout<<w<<" ";
        cout<<endl;
    }
}



//ブロックを置く場所を探す
Point findPlaceToPut(const Board& board, Block block, int color,Block &ans_B){

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
                int nx=xj+ex[k],ny=yi+ey[k];
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
            Evaluation evl=check(board,point,b,color,temp);
            if(evl.border>mx.border){
                ans_p=temp;
                ans_block=b;
                mx=evl;
            }
            else if(evl.border==mx.border){
                if(evl.distance<=mx.distance){
                ans_p=temp;
                ans_block=b;
                mx=evl;
                }
            }
            }
        }
    ans_B=ans_block;
    return ans_p;
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

Evaluation check(const Board &board, const Point point, const Block b, int color,Point &ans){
    const int height=b.size(),width=b[0].size();
    Evaluation res;//相手のブロックと接する回数を最大化
    //(yi,xj)を(point.y,point.x)に合わせる
    res.border=-1;
    res.distance=100;
    for(int yi=0;yi<height;yi++)for(int xj=0;xj<width;xj++){
        if(b[yi][xj]==0)continue;
        //(y,x)はboardでの、置きたいブロックの右上の座標
        int y=point.y-yi,x=point.x-xj;
        if(y<0 || x<0)continue;
        Evaluation cnt;//相手のブロックと接する回数
        bool able=true;
        for(int i=0;i<height;i++)for(int j=0;j<width;j++){
            if(b[i][j]==0){
                if((0<=y+i && y+i<20 && 0<=x+j && x+j<20) && board[y+i][x+j]==color)able=false;
                else if((0<=y+i && y+i<20 && 0<=x+j && x+j<20) && board[y+i][x+j]!=0)cnt.border++;
                continue;
            }
            if(y+i<0 || x+j<0 || y+i>=20 || x+j>=20){able=false;continue;}
            if(board[y+i][x+j]!=0)able=false;
            if(i==0){
                if(y+i-1>=0 && board[y+i-1][x+j]==color)able=false;//上のますが同じ色
                else if(y+i-1>=0 && board[y+i-1][x+j]!=0)cnt.border++;
            }
            if(i==height-1){
                if(y+i+1<20 && board[y+i+1][x+j]==color)able=false;//下のマスが同じ色
                else if(y+i+1<20 && board[y+i+1][x+j]!=0)cnt.border++;

            }
            if(j==0){
                if(x+j-1>=0 && board[y+i][x+j-1]==color)able=false;//左のマスが同じ色
                else if(x+j-1>=0 && board[y+i][x+j-1]!=0)cnt.border++;
            }
            if(j==width-1){
                if(x+j+1<20 && board[y+i][x+j+1]==color)able=false;//右のマスが同じ色
                else if(x+j+1<20 && board[y+i][x+j+1]!=0)cnt.border++;
            }
        }
        Point upper_right;upper_right.y=y,upper_right.x=x;
        Point near_center=findNearCenter(board,upper_right,b);
        cnt.distance=abs(near_center.x-10)+abs(near_center.y-10);
        if(able){
            if(cnt.border>res.border){
                Point t;
                t.x=x,t.y=y;
                ans=t;
                res=cnt;
            }
            else if(cnt.border==res.border){
                if(res.distance>=cnt.distance){
                Point t;
                t.x=x,t.y=y;
                ans=t;
                res=cnt;
                }
            }
        }
    }

    return res;
}

std::vector<int> findUsedBlock(const Board& board,const int color){
    vector<vector<bool> > visited(20,vector<bool>(20,0));
    vector<int> used_list;
    for(int yi=0;yi<20;yi++){
        for(int xj=0;xj<20;xj++){
            Point t;
            t.x=xj,t.y=yi;
            if(board[yi][xj]==color && visited[yi][xj]==0){
                Block pattern;
                vector<Block> res(8);
                pattern=bfs(board,t,visited,color);
                if(pattern.size()==0){
                    vector<int> NULL_LIST;
                    NULL_LIST.emplace_back(-1);
                    return NULL_LIST;
                }

                createEightPatterns(pattern,res);
                for(auto p:res){
                    for(int i=0;i<(int)block_list[color-1].size();i++){
                        if(p==block_list[color-1][i])used_list.emplace_back(i);
                    }
                }
            }
        }
    }
    used_list.erase(unique(used_list.begin(),used_list.end()),used_list.end());
    return used_list;
}

Point findNearCenter(const Board &board, const Point upper_right/*on board*/,const Block& block){
    int height=block.size(),width=block[0].size();
    Point ans=upper_right;
    int min_distance=100;
    for(int yi=0;yi<height;yi++){
        for(int xj=0;xj<width;xj++){
            int y=upper_right.y+yi,x=upper_right.x+xj;
            if(block[yi][xj]==0)continue;
            int now_distance=abs(y-10)+abs(x-10);
            if(now_distance<=min_distance){
                min_distance=now_distance;
                Point t;t.y=y,t.x=x;
                ans=t;
            }
        }
    }
    return ans;
}


Block bfs(const Board& board, const Point point,vector<vector<bool>> &visited,const int color){
    int ans_list[100][100]={0};//初期化の方法
    std::queue<Point>q;
    q.push(point);
    visited[point.y][point.x]=true;
    ans_list[0][2]=color;

    int y_end=0;
    int x_start=-1,x_end=0;
    int cnt=0;
    while(q.size()){
        Point p=q.front();
        q.pop();
        for(int i=0;i<4;i++){
            int ny=p.y+dy[i],nx=p.x+dx[i];
            if(ny<0 || nx<0 || nx>=20 || ny>=20)continue;
            if(board[ny][nx]!=color)continue;
            if(visited[ny][nx])continue;
            Point t;
            t.x=nx,t.y=ny;
            q.push(t);//次訪れるキューにプッシュ
            cnt++;
            visited[ny][nx]=true;//訪問記録
            int y_diff=ny-point.y;//y方向にどの程度離れているか
            int x_diff=nx-point.x+2;//x方向　　々
            y_end=max(y_end,y_diff);
            ans_list[y_diff][x_diff]=color;
        }
    }
    y_end++;//y方向の高さ
    if(cnt>5){
        Block NULL_BLOCK;
        return NULL_BLOCK;
    }

    for(int xj=0;xj<8;xj++){
        for(int yi=0;yi<y_end;yi++){
            if(x_start==-1 && ans_list[yi][xj]){x_start=xj;break;}
            if(ans_list[yi][xj])break;
            if(yi==y_end-1 && x_start!=-1){
                x_end=xj;
            }
        }
        if(x_end!=0)break;
    }
    Block res(y_end);
    for(int yi=0;yi<y_end;yi++){
        for(int xj=x_start;xj<x_end;xj++){
            res[yi].emplace_back(ans_list[yi][xj]);
        }
    }
    return res;
}