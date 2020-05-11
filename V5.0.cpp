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
struct Command allmoveCmd[20];
struct Command alljumpCmd[20];
struct Command allcommand[20];

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
            if (!(board[midX][midY] & 1))
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
int search(int player)//player1是我，player2是敌方，search函数的功能是把某一方的所有可走选择放在allcommand[]数组里，并返回 可走选择的个数 
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
		int numchecked=0;
		for (int i = 0; i < BOARD_SIZE; i++)
		    {
		        for (int j = 0; j < BOARD_SIZE; j++)
		        {
		            if (board[i][j] > 0 && (board[i][j] & 1) == t)
		            {
		            	numchecked++;
		                if (tryToMove(i, j) > 0)
		                {
		                    for(int p=0;p<tryToMove(i, j);p++)
		                    	allcommand[num++]=allmoveCmd[p];
		                }    
		            }
		            if(numchecked>=numFlag[player])
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

float value(char currentboard[BOARD_SIZE][BOARD_SIZE])
{
	float valuation=0;
	for(int i=0;i<BOARD_SIZE;i++)
		for(int j=0;j<BOARD_SIZE;j++)
		{
			if(currentboard[i][j]==MY_FLAG)
				valuation++; 
			if(currentboard[i][j]==MY_KING)
				valuation+3; 
			if(currentboard[i][j]==ENEMY_FLAG)
				valuation--;
			if(currentboard[i][j]==ENEMY_KING)
				valuation-=3;	
		}
	for(int j=0;j<8;j+=7)
		for(int i=0;i<BOARD_SIZE;i++)
		{
			if(currentboard[i][j]==MY_FLAG||currentboard[i][j]==MY_KING)
				valuation+=0.4;
		}
	return valuation;
}

struct Command minimax()//极大极小 
{
	int alpha=-10000;
	int beta=10000;
	int tempnumFlag[6][3];
	int i,j,n,m,p,q;
	char tempboard0[BOARD_SIZE][BOARD_SIZE];
	char tempboard1[BOARD_SIZE][BOARD_SIZE];
	char tempboard2[BOARD_SIZE][BOARD_SIZE];
	char tempboard3[BOARD_SIZE][BOARD_SIZE];
	char tempboard4[BOARD_SIZE][BOARD_SIZE];
	char tempboard5[BOARD_SIZE][BOARD_SIZE];
	struct Command mycommand0;
	struct Command enemycommand1;
	struct Command mycommand2;
	struct Command enemycommand3;
	struct Command mycommand4;
	struct Command enemycommand5;
	struct Command allmycommand0[20];
	struct Command allenemycommand1[20];
	struct Command allmycommand2[20];
	struct Command allenemycommand3[20];
	struct Command allmycommand4[20];
	//极大极小开始
	cpyboard(tempboard0,board);
	tempnumFlag[0][1]=numFlag[1];
	tempnumFlag[0][2]=numFlag[2];
	i=search(1);
	int maxvaluation0=-10000;
	for(int t=0;t<i;t++)
		allmycommand0[t]=allcommand[t];
	for(int step0=0;step0<i;step0++)
	{
		place(allmycommand0[step0]);
		cpyboard(tempboard1,board);
		tempnumFlag[1][1]=numFlag[1];
		tempnumFlag[1][2]=numFlag[2];
		j=search(2);
		int minvaluation1=10000;
		for(int t=0;t<j;t++)
			allenemycommand1[t]=allcommand[t];
		for(int step1=0;step1<j;step1++)
		{
			place(allenemycommand1[step1]);
			cpyboard(tempboard2,board);
			tempnumFlag[2][1]=numFlag[1];
			tempnumFlag[2][2]=numFlag[2];
			n=search(1);
			int maxvaluation2=-10000;
			for(int t=0;t<n;t++)
				allmycommand2[t]=allcommand[t];
			for(int step2=0;step2<n;step2++)
			{
				place(allmycommand2[step2]);
				cpyboard(tempboard3,board);
				tempnumFlag[3][1]=numFlag[1];
				tempnumFlag[3][2]=numFlag[2];
				m=search(2);
				int minvaluation3=10000;
				for(int t=0;t<m;t++)
					allenemycommand3[t]=allcommand[t];
				for(int step3=0;step3<m;step3++)
				{
					place(allenemycommand3[step3]);
					cpyboard(tempboard4,board);
					tempnumFlag[4][1]=numFlag[1];
					tempnumFlag[4][2]=numFlag[2];
					p=search(1);
					int maxvaluation4=-10000;
					for(int t=0;t<p;t++)
						allmycommand4[t]=allcommand[t];
					for(int step4=0;step4<p;step4++)
					{
						place(allmycommand4[step4]);
						cpyboard(tempboard5,board);
						tempnumFlag[5][1]=numFlag[1];
						tempnumFlag[5][2]=numFlag[2];
						q=search(2);
						int minvaluation5=10000;
						for(int step5=0;step5<q;step5++)
						{
							place(allcommand[step4]);
							if(value(board)<minvaluation5)
							{
								minvaluation5=value(board);
								enemycommand5=allcommand[step5];
							}
							cpyboard(board,tempboard5);
							numFlag[1]=tempnumFlag[5][1];
							numFlag[2]=tempnumFlag[5][2];
						}
						place(enemycommand5);
						if(value(board)>maxvaluation4)
						{
							maxvaluation4=value(board);
							mycommand4=allmycommand4[step4];	
						}
						cpyboard(board,tempboard4);
						numFlag[1]=tempnumFlag[4][1];
						numFlag[2]=tempnumFlag[4][2];
					}
					place(mycommand4);
					if(value(board)<minvaluation3)
					{
						minvaluation3=value(board);
						enemycommand3=allcommand[step3];
					}
					cpyboard(board,tempboard3);
					numFlag[1]=tempnumFlag[3][1];
					numFlag[2]=tempnumFlag[3][2];
				}
				place(enemycommand3);
				if(value(board)>maxvaluation2)
				{
					maxvaluation2=value(board);
					mycommand2=allmycommand2[step2];
				}
				cpyboard(board,tempboard2);
				numFlag[1]=tempnumFlag[2][1];
				numFlag[2]=tempnumFlag[2][2];
			}
			place(mycommand2);
			if(value(board)<minvaluation1)
				{
					minvaluation1=value(board);
					enemycommand1=allenemycommand1[step1];
				}
			cpyboard(board,tempboard1);
			numFlag[1]=tempnumFlag[1][1];
			numFlag[2]=tempnumFlag[1][2];
		}
		place(enemycommand1);
		if(value(board)>maxvaluation0)
			{
				maxvaluation0=value(board);
				mycommand0=allmycommand0[step0];
			}
		cpyboard(board,tempboard0);
		numFlag[1]=tempnumFlag[0][1];
		numFlag[2]=tempnumFlag[0][2];
	} 
	return mycommand0;
}

void initAI(int me)
{
    numFlag[1]=12;
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

    return minimax();//先返回我方的头一个选择 
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
    }
}

int main(int argc, char *argv[])
{
    loop();
    return 0;
}
