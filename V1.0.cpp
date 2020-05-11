#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<time.h>

// board information
#define BOARD_SIZE 8
#define EMPTY 0
#define MY_FLAG 2
#define MY_KING 4
#define ENEMY_FLAG 1
#define ENEMY_KING 3


// bool
typedef int BOOL;
#define TRUE 1
#define FALSE 0

#define MAX_STEP 15

#define START "START"
#define PLACE "PLACE"
#define TURN "TURN"
#define END "END"
//我是1，对方是2 
struct Command
{
    int x[MAX_STEP];
    int y[MAX_STEP];
    int numStep;
};
char board[BOARD_SIZE][BOARD_SIZE] = {0};
int myFlag;
int moveDir[4][2] = {{1, -1}, {1, 1}, {-1, -1}, {-1, 1}};
int jumpDir[4][2] = {{2, -2}, {2, 2}, {-2, -2}, {-2, 2}};
int numFlag[3];
struct Command moveCmd = { .x={0}, .y={0}, .numStep=2 };
struct Command jumpCmd = { .x={0}, .y={0}, .numStep=0 };
struct Command longestJumpCmd = { .x={0}, .y={0}, .numStep=1 };
struct Command allmoveCmd[15];
struct Command singleflagjumpCmd[15];
struct Command alljumpCmd[15];
struct Command allcommand[15];
void debug(const char *str)
{
    printf("DEBUG %s\n", str);			
    fflush(stdout);
}

void printBoard()
{
    char visualBoard[BOARD_SIZE][BOARD_SIZE + 1] = {0};
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            switch (board[i][j])
            {
                case EMPTY:
                    visualBoard[i][j] = '.';
                    break;
                case ENEMY_FLAG:
                    visualBoard[i][j] = 'O';
                    break;
                case MY_FLAG:
                    visualBoard[i][j] = 'X';
                    break;
                case ENEMY_KING:
                    visualBoard[i][j] = '@';
                    break;
                case MY_KING:
                    visualBoard[i][j] = '*';
                    break;
                default:
                    break;
            }
        }
        printf("%s\n", visualBoard[i]);
    }
}

BOOL isInBound(int x, int y)
{
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

void rotateCommand(struct Command *cmd)
{
    if (myFlag == ENEMY_FLAG)
    {
        for (int i = 0; i < cmd->numStep; i++)
        {
            cmd->x[i] = BOARD_SIZE - 1 - cmd->x[i];
            cmd->y[i] = BOARD_SIZE - 1 - cmd->y[i];
        }
    }
}

int tryToMove(int x, int y)
{
	int num=0;
    int newX, newY;
    if(board[x][y]==MY_FLAG)
	    for (int i = 0; i < 2; i++)
	    {
	        newX = x + moveDir[i][0];
	        newY = y + moveDir[i][1];
	        if (isInBound(newX, newY) && board[newX][newY] == EMPTY)
	        {
	            moveCmd.x[0] = x;
	            moveCmd.y[0] = y;
	            moveCmd.x[1] = newX;
	            moveCmd.y[1] = newY;
	            allmoveCmd[num++]=moveCmd;
	        }
	    }
	if(board[x][y]==ENEMY_FLAG)
	    for (int i = 0; i < 2; i++)
	    {
	        newX = x + moveDir[2+i][0];
	        newY = y + moveDir[2+i][1];
	        if (isInBound(newX, newY) && board[newX][newY] == EMPTY)
	        {
	            moveCmd.x[0] = x;
	            moveCmd.y[0] = y;
	            moveCmd.x[1] = newX;
	            moveCmd.y[1] = newY;
	            allmoveCmd[num++]=moveCmd;
	        }
	    }
	 if(board[x][y]==MY_KING||board[x][y]==ENEMY_KING)
	    for (int i = 0; i < 4; i++)
	    {
	        newX = x + moveDir[i][0];
	        newY = y + moveDir[i][1];
	        if (isInBound(newX, newY) && board[newX][newY] == EMPTY)
	        {
	            moveCmd.x[0] = x;
	            moveCmd.y[0] = y;
	            moveCmd.x[1] = newX;
	            moveCmd.y[1] = newY;
	           	allmoveCmd[num++]=moveCmd;
	        }
	    }
    return num;
}

void tryToJump(int x, int y, int currentStep,int player)
{
	int t;
	if(player==1)
		t=1;
	else t=0; 
    int newX, newY, midX, midY;
    char tmpFlag;
    jumpCmd.x[currentStep] = x;
    jumpCmd.y[currentStep] = y;
    jumpCmd.numStep++;
    for (int i = 0; i < 4; i++)
    {
        newX = x + jumpDir[i][0];
        newY = y + jumpDir[i][1];
        midX = (x + newX) / 2;
        midY = (y + newY) / 2;
        if (isInBound(newX, newY) && board[midX][midY]>0&&((board[midX][midY] & 1)==t) && (board[newX][newY] == EMPTY))
        {
            board[newX][newY] = board[x][y];
            board[x][y] = EMPTY;
            tmpFlag = board[midX][midY];
            board[midX][midY] = EMPTY;
            tryToJump(newX, newY, currentStep + 1,player);
            board[x][y] = board[newX][newY];
            board[newX][newY] = EMPTY;
            board[midX][midY] = tmpFlag;
        }
    }
    if (jumpCmd.numStep > longestJumpCmd.numStep)
    {
        memcpy(&longestJumpCmd, &jumpCmd, sizeof(struct Command));
    }
    jumpCmd.numStep--;
}

void place(struct Command cmd)
{
    int midX, midY, curFlag;
    curFlag = board[cmd.x[0]][cmd.y[0]];
    for (int i = 0; i < cmd.numStep - 1; i++)
    {
        board[cmd.x[i]][cmd.y[i]] = EMPTY;
        board[cmd.x[i + 1]][cmd.y[i + 1]] = curFlag;
        if (abs(cmd.x[i] - cmd.x[i + 1]) == 2)
        {
            midX = (cmd.x[i] + cmd.x[i + 1]) / 2;
            midY = (cmd.y[i] + cmd.y[i + 1]) / 2;
            if ((board[midX][midY] & 1) == 0)
            {
                numFlag[1]--;
            }
            if (board[midX][midY] & 1)
            {
                numFlag[2]--;
            }
            board[midX][midY] = EMPTY;
            board[midX][midY] = EMPTY;
        }
    }
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (board[0][i] == ENEMY_FLAG)
        {
            board[0][i] = ENEMY_KING;
        }
        if (board[BOARD_SIZE - 1][i] == MY_FLAG)
        {
            board[BOARD_SIZE - 1][i] = MY_KING;
        }
    }
}

/**
 * YOUR CODE BEGIN
 * 你的代码开始
 */

/**
 * You can define your own struct and variable here
 * 你可以在这里定义你自己的结构体和变量
 */

/**
 * 你可以在这里初始化你的AI
 */
 
 
int searchjump(int player)
{
	int numchecked1=0; 
	int k;
	if(player==1)
		k=0;
	else k=1;
	int num=0;
	int maxStep = 1;
	longestJumpCmd.numStep = 1;
	for(int i=0;i<BOARD_SIZE;i++)
		for(int j=0;j<BOARD_SIZE;j++)
		{
			if (board[i][j] > 0 && (board[i][j] & 1) == k)
			{
				numchecked1++;
				longestJumpCmd.numStep = 1;
				tryToJump(i, j, 0,player);
				if (longestJumpCmd.numStep > maxStep)
	            {
	                maxStep=longestJumpCmd.numStep;
	            }
	        }
	        if(numchecked1>=numFlag[player])
	        	break;
		}
	if(maxStep==1)
		return 0;
	numchecked1=0;
	longestJumpCmd.numStep = 1;
	for(int i=0;i<BOARD_SIZE;i++)
		for(int j=0;j<BOARD_SIZE;j++)
		{
			if (board[i][j] > 0 && (board[i][j] & 1) == k)
			{
				numchecked1++;
				longestJumpCmd.numStep = 1;
				tryToJump(i, j, 0,player);
				if (longestJumpCmd.numStep == maxStep)
	            {
	                alljumpCmd[num++]=longestJumpCmd;
	            }
	        }
	        if(numchecked1>=numFlag[player])
	        	return num;
		}
	return num;				
}
int search(int player)
{
	int t;
	if(player==1)
		t=0;
	else t=1;
	int num=0;
	if(searchjump(player)!=0)
	{
		for(int i=0;i<searchjump(player);i++)
			allcommand[num++]=alljumpCmd[i];
		return num;
	}
	else
	{
		int numChecked = 0;
		for (int i = 0; i < BOARD_SIZE; i++)
		    {
		        for (int j = 0; j < BOARD_SIZE; j++)
		        {
		            if (board[i][j] > 0 && (board[i][j] & 1) == t)
		            {
		                numChecked++;
		                if (tryToMove(i, j) > 0)
		                {
		                    for(int p=0;p<tryToMove(i, j);p++)
		                    	allcommand[num++]=allmoveCmd[p];
		                }    
		            }
		            if (numChecked >= numFlag[player])
		            {
		                return num;
		            }
		        }
		    }
		}
	return num;
}

void cpyboard(char desboard[BOARD_SIZE][BOARD_SIZE],char iniboard[BOARD_SIZE][BOARD_SIZE])
{
	for(int i=0;i<BOARD_SIZE;i++)
		for(int j=0;j<BOARD_SIZE;j++)
			desboard[i][j]=iniboard[i][j];
} 

float value()
{
	float valuation=0;
	for(int i=0;i<BOARD_SIZE;i++)
		for(int j=0;j<BOARD_SIZE;j++)
		{
			if(board[i][j]==MY_FLAG)
				valuation++;
			if(board[i][j]==MY_KING)
				valuation+3;
			if(board[i][j]==ENEMY_FLAG)
				valuation--;
			if(board[i][j]==ENEMY_KING)
				valuation-=3;	
		}
	for(int j=0;j<8;j+=7)
		for(int i=0;i<BOARD_SIZE;i++)
		{
			if(board[i][j]==MY_FLAG||board[i][j]==MY_KING)
				valuation+=0.5;
		}
	return valuation;
}

struct Command minimax()
{

}

void initAI(int me)
{
    numFlag[1] = 12;
	numFlag[2]=12;
}

/**
 * 轮到你落子。
 * 棋盘上0表示空白，1表示黑棋，2表示白旗
 * me表示你所代表的棋子(1或2)
 * 你需要返回一个结构体Command，其中numStep是你要移动的棋子经过的格子数（含起点、终点），
 * x、y分别是该棋子依次经过的每个格子的横、纵坐标
 */
struct Command aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me)
{
	printf("%d\n",search(1));
	printf("%d\n",search(2));
	search(1);
	printf("%d ",allcommand[0].numStep);
	for(int i=0;i<allcommand[0].numStep;i++)
		printf("%d,%d ", BOARD_SIZE - 1-allcommand[0].x[i], BOARD_SIZE - 1-allcommand[0].y[i]);
	printf("\n");
	return allcommand[0];
	
}

/**
 * 你的代码结束
 */

//.X.X.X.X
//X.X.X.X.
//.X.X.X.X
//........
//........
//O.O.O.O.
//.O.O.O.O
//O.O.O.O.
void start(int flag)
{
    memset(board, 0, sizeof(board));
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 8; j += 2)
        {
            board[i][j + (i + 1) % 2] = MY_FLAG;
        }
    }
    for (int i = 5; i < 8; i++)
    {
        for (int j = 0; j < 8; j += 2)
        {
            board[i][j + (i + 1) % 2] = ENEMY_FLAG;
        }
    }
    
    initAI(flag);
}

void turn()
{
    // AI
    struct Command command = aiTurn((const char(*)[BOARD_SIZE])board, myFlag);
    place(command);
    rotateCommand(&command);
    printf("%d", command.numStep);
    for (int i = 0; i < command.numStep; i++)
    {
        printf(" %d,%d", command.x[i], command.y[i]);
    }
    printf("\n");
    fflush(stdout);
}

void end(int x)
{
    exit(0);
}

void loop()
{
    char tag[10] = {0};
    struct Command command =
    {
        .x = {0},
        .y = {0},
        .numStep = 0
    };
    int status;
    while (TRUE)
    {
        memset(tag, 0, sizeof(tag));
        scanf("%s", tag);
        if (strcmp(tag, START) == 0)
        {
            scanf("%d", &myFlag);
            start(myFlag);
            printf("OK\n");
            fflush(stdout);
        }
        else if (strcmp(tag, PLACE) == 0)
        {
            scanf("%d", &command.numStep);
            for (int i = 0; i < command.numStep; i++)
            {
                scanf("%d,%d", &command.x[i], &command.y[i]);
            }
            rotateCommand(&command);
            place(command);
        }
        else if (strcmp(tag, TURN) == 0)
        {
            turn();
        }
        else if (strcmp(tag, END) == 0)
        {
            scanf("%d", &status);
            end(status);
        }
        printBoard();
    }
}

int main(int argc, char *argv[])
{
    loop();
    return 0;
}



