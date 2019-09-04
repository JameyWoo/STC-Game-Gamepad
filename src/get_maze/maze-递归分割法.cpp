#include <iostream>
#include <cstdlib> 
#include <ctime>
 
static const int row = 32;
static const int col = 64;
 
static const int ROUTE=0;
static const int WALL=1;
 
void CreateMaze(int **maze,int x1,int y1,int x2,int y2){
    //判断是否还能继续分割
    if(x2-x1<2||y2-y1<2){
        return ;
    }
    
    //随机取点
    int x=x1+1+rand()%(x2-x1-1);
    int y=y1+1+rand()%(y2-y1-1);
    
    //画墙
    for(int i=x1;i<=x2;i++) maze[i][y]=WALL;
    for(int i=y1;i<=y2;i++) maze[x][i]=WALL;
 
    //递归分割，继续划分区域
    CreateMaze(maze,x1,y1,x-1,y-1);
    CreateMaze(maze, x+1, y+1, x2, y2);
    CreateMaze(maze,x+1,y1,x2,y-1);
    CreateMaze(maze, x1, y+1, x-1, y2);
    
    //随机取其中的三面墙
    int r[4]={0};
    r[rand()%4]=1;
    
    //在墙上随机取点开孔
    for(int i=0;i<4;i++){
        if(r[i]==0){
            int rx=x;
            int ry=y;
            switch (i) {
                case 0:
                    //判断该位置是否能确保打通相邻两块区域，判断依据，上下左右位置最多只有两面墙，下面一样
                    do{
                        rx=x1+rand()%(x-x1);
                    }while(maze[rx-1][ry]+maze[rx+1][ry]+maze[rx][ry-1]+maze[rx][ry+1]>2*WALL);
                    break;
                case 1:
                    do{
                        ry=y+1+rand()%(y2-y);
                    }while(maze[rx-1][ry]+maze[rx+1][ry]+maze[rx][ry-1]+maze[rx][ry+1]>2*WALL);
                    break;
                case 2:
                    do{
                        rx=x+1+rand()%(x2-x);
                    }while(maze[rx-1][ry]+maze[rx+1][ry]+maze[rx][ry-1]+maze[rx][ry+1]>2*WALL);
                    break;
                case 3:
                    do{
                        ry=y1+rand()%(y-y1);
                    }while(maze[rx-1][ry]+maze[rx+1][ry]+maze[rx][ry-1]+maze[rx][ry+1]>2*WALL);
                    break;
                default:
                    break;
            }
            maze[rx][ry]=ROUTE;
        }
    }
}
 
int main(int argc, const char * argv[]) {
    srand((unsigned)time(NULL));
    
    int **Maze=(int**)malloc(row*sizeof(int*));
    for(int i=0;i<row;i++){
        Maze[i]=(int*)calloc(col,sizeof(int));
    }
    
    //外侧一圈为墙
    for (int i = 0; i < row; ++i)
    	Maze[i][0] = Maze[i][col - 1] = WALL;
    for (int i = 0; i < col; ++i)
		Maze[0][i] = Maze[row - 1][i] = WALL; 
    
    //生成迷宫
    CreateMaze(Maze, 1,1, row-2, col-2);
    
    //设置出口
    Maze[1][0]=ROUTE;
    Maze[row-2][col-1]=ROUTE;
    
    //画迷宫
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            if(Maze[i][j]==WALL){
                printf("XX");
            }else{
                printf("  ");
            }
        }
        printf("\n");
    }
    
    // 扩展为大地图 
    int maze[64][128] = {};
	for (int i = 0; i < row; ++i) {
		for (int j = 0; j < col; ++j) {
			maze[2*i + 0][2*j + 0] = maze[2*i + 1][2*j + 0] = 
			maze[2*i + 0][2*j + 1] = maze[2*i + 1][2*j + 1] = Maze[i][j];
		}
	}
    
    int maze_map[1024], top = 0;
	for (int i = 0; i < 8; ++i) {
		int i8 = 8*i;
		for (int j = 0; j < 128; ++j) {
			int ch = 0;
			for (int k = i8 + 7; k >= i8; --k) {
				int bi = (k - i8);
				ch |= ((maze[k][j]) << bi);
			}
			maze_map[top++] = ch;
		}
	}
	for (int i = 0; i < 1024; i++) {
		printf("%d,", maze_map[i]);
	}
	
	for(int i=0;i<row;i++) free(Maze[i]);
    free(Maze);
    
    return 0;
}
