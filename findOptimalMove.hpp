#ifndef INCLUDED_FIND_OPTIMAL_MOVE_h

#define INCLUDED_FIND_OOPTIMAL_MOVE_h

#ifndef vector
#include "vector"
#endif

#ifndef iostream
#include "iostream"
#endif

#ifndef algorithm
#include "algorithm"
#endif

#ifndef queue
#include "queue"
#endif
using Block=std::vector<std::vector<int>>;
using Board=std::vector<std::vector<int>>;
struct Point{
    int y,x;
};
const int dx[]={1,0,-1,0},dy[]={0,1,0,-1};//縦横のマス
const int ex[]={1,1,-1,-1},ey[]={-1,1,1,-1};//斜めのマス

//ブロックリストを作る
const std::vector<std::vector<Block> >block_list={
{
    {{1}}, {{1,1}}, {{1,1,1}}, {{1,1}, {0,1}}, //1,2,3
    {{1,1,1,1}}, {{1,1,1},{0,0,1}}, {{1,1,1},{0,1,0}}, {{1,1},{1,1}}, {{1,1,0},{0,1,1}}, //4
    {{1,1,1,1,1}}, {{1,1,1,1},{0,0,0,1}}, {{1,1,1,0},{0,0,1,1}}, {{1,1,1},{0,1,1}}, //5
    {{1,1,1},{1,0,1}}, {{1,1,1,1},{0,0,1,0}}, {{1,1,1},{0,1,0},{0,1,0}}, //5
    {{1,0,0},{1,0,0},{1,1,1}},{{1,1,0},{0,1,1},{0,0,1}}, {{1,0,0},{1,1,1},{0,0,1}}, //5
    {{1,0,0},{1,1,1},{0,1,0}}, {{0,1,0},{1,1,1},{0,1,0}} //5
},
{
    {{2}}, {{2,2}}, {{2,2,2}}, {{2,2}, {0,2}}, //1,2,3
    {{2,2,2,2}}, {{2,2,2},{0,0,2}}, {{2,2,2},{0,2,0}}, {{2,2},{2,2}}, {{2,2,0},{0,2,2}}, //4
    {{2,2,2,2,2}}, {{2,2,2,2},{0,0,0,2}}, {{2,2,2,0},{0,0,2,2}}, {{2,2,2},{0,2,2}}, //5
    {{2,2,2},{2,0,2}}, {{2,2,2,2},{0,0,2,0}}, {{2,2,2},{0,2,0},{0,2,0}}, //5
    {{2,0,0},{2,0,0},{2,2,2}},{{2,2,0},{0,2,2},{0,0,2}}, {{2,0,0},{2,2,2},{0,0,2}}, //5
    {{2,0,0},{2,2,2},{0,2,0}} //5
}
};


struct Evaluation{
    int border=0,distance=100;//distance from the center
};




static Evaluation mx;
static Point ans_p;//置かれるブロックの右端の座標
static Block ans_block;//ブロックの形（向き）


Point findOptimalMove(const Board &board, const int color, Block & ans_B);
void createEightPatterns(Block block, std::vector<Block> &created);
void printBlock(Block b);
Evaluation check(const Board &board, const Point point, const Block b, int color, Point &ans);
Point findPlaceToPut(const Board& board, Block block,int color,Block &ans);
std::vector<int> findUsedBlock(const Board& board,const int color);
Block bfs(const Board &board,const Point point, std::vector<std::vector<bool>> &visited,const int color);
Point findNearCenter(const Board &board, const Point upper_right,const Block& block);
#endif