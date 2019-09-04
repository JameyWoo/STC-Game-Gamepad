#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap_x_y(unsigned char* x, unsigned char* y) {
	unsigned char tmp = *x;
	*x = *y;
	*y = tmp;
}

void CreateMaze(int row, int col) {
	srand((unsigned)time(NULL));
	unsigned char Maze[64][64] = { 0 };
	int dirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
 
	//最外围设置为路，可以有效的保护里面一层墙体，并防止挖出界
	int i, j;
	for (i = 0; i < row; ++i)
		Maze[i][0] = Maze[i][col - 1] = 1;
	for (i = 0; i < col; ++i)
		Maze[0][i] = Maze[row - 1][i] = 1;
 
	//墙队列，包括X , Y
	unsigned char X[16*32] = {};
	unsigned char Y[16*32] = {};
	unsigned int top = 0; // top 位是没有数据的, 也就是它代表队列中元素的数量 
 
	//任取初始值
	X[top] = 2; 
	Y[top++] = 2;
 
	//当墙队列为空时结束循环
	while (top) {
		if (top > 16*32) printf("top = %d\n", top);
		//在墙队列中随机取一点
		int r = rand() % top;
		int x = X[r];
		int y = Y[r];
 
		//判读上下左右四个方向是否为路
		int count = 0;
		for (i = 0; i < 4; ++i) {
			if (Maze[x + dirs[i][0]][y + dirs[i][1]] > 0) ++count;
		}
 
		if (count <= 1) {
			Maze[x][y] = 1;
			//在墙队列中插入新的墙
			// 谜一样的bug为啥!!! 
			for (i = x - 1; i < x + 2; i++) {
				for (j = y - 1; j < y + 2; j++) {
					if (abs(x - i) + abs(y - j) == 1 && Maze[i][j] == 0) {
//						printf("i = %d, j = %d\n", i, j);
						X[top] = i;
						Y[top++] = j;
					}
				}
			}
		}
 
		//删除当前墙
		top--;
		swap_x_y(&X[r], &X[top]);
		swap_x_y(&Y[r], &Y[top]);
	}
 
	//设置迷宫进出口
	Maze[2][1] = 1;
	for (i = row - 3; i >= 0; i--) {
		if (Maze[i][col - 3] == 1) { 
			Maze[i][col - 2] = 1;
			break;
		}
	}
 
	//画迷宫
	for (i = 0; i < row; i++){
		for (j = 0; j < col; j++) {
//			printf("%d", Maze[i][j]);
			if (Maze[i][j] == 1) printf("  ");
			else printf("口");
		}
		printf("\n");
	}
	printf("迷宫绘制完成\n");
	int k;
	int maze[64][128] = {};
	for (i = 0; i < row; ++i) {
		for (j = 0; j < col; ++j) {
			maze[2*i + 0][2*j + 0] = maze[2*i + 1][2*j + 0] = 
			maze[2*i + 0][2*j + 1] = maze[2*i + 1][2*j + 1] = Maze[i][j];
		}
	}
	printf("大地图生成完成\n");
//	for (i = 0; i < 64; ++i) {
//		for (j = 0; j < 128; ++j) {
//			printf("%d", maze[i][j]);
//		}
//		printf("\n");
//	}
	top = 0;
	int maze_map[1024];
	for (i = 0; i < 8; ++i) {
		int i8 = 8*i;
		for (j = 0; j < 128; ++j) {
			int ch = 0;
			for (k = i8 + 7; k >= i8; --k) {
//				printf("%d ", maze[k][j]);
				int bi = (k - i8);
//				printf("\nbi = %d\n", bi);
//				printf("(!maze[k][j]) = %d\n", (!maze[k][j]));
//				printf("(!maze[k][j]) << bi = %d\n", (!maze[k][j]) << bi); 
				ch |= ((!maze[k][j]) << bi);
//				printf("ch=%d", ch);
			}
			maze_map[top++] = ch;
//			printf("ch=%d", ch);
		}
//		printf("\n");
	}
	for (i = 0; i < 1024; i++) {
		printf("%d,", maze_map[i]);
	}
}

int main() {
	CreateMaze(32, 64);
} 
