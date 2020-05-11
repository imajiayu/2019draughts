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
int tempnumFlag[8][3];
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

int tryToMove(int x, int y)//搜索移动 
{
	int num=0;
    int newX, newY;
    if(board[x][y]==MY_FLAG)//我方兵棋，左下，右下两个方向 
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
	if(board[x][y]==ENEMY_FLAG)//敌方兵棋，左上，右上两个方向 
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
	 if(board[x][y]==MY_KING||board[x][y]==ENEMY_KING)//双方王棋，四个方向全部搜索 
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
    return num;//返回单个棋子移动选择个数 
}

void tryToJump(int x, int y, int currentStep,int player)//搜索单个棋子的吃子 
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
            board[midX][midY] = EMPTY;//将跳过棋子设为empty，防止重跳 
            tryToJump(newX, newY, currentStep + 1,player);
            board[x][y] = board[newX][newY];//复原 
            board[newX][newY] = EMPTY;
            board[midX][midY] = tmpFlag;
        }
    }
    if (jumpCmd.numStep > longestJumpCmd.numStep)//取最长路径 
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


int searchjump(int player)//搜索全部棋子的跳跃，player为敌我双方 
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
		for(int j=0;j<BOARD_SIZE;j++)//先遍历棋盘得到最长，有多吃多 
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
	if(maxStep==1)//无吃子选择，返回0 
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
				if (longestJumpCmd.numStep == maxStep)//有都吃多 
	            {
	                alljumpCmd[num++]=longestJumpCmd;
	            }
	        }
	        if(numchecked1>=numFlag[player])
	        	return num;
		}
	return num;	//返回全局某一方吃子选择			
}
int search(int player)//player1是我，player2是敌方，search函数的功能是把某一方的所有可走选择放在allcommand[]数组里，并返回 可走选择的个数 
{
	int t;
	if(player==1)
		t=0;
	else t=1;
	int num=0;
	if(searchjump(player)!=0)//有吃必吃 
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
		            if (board[i][j] > 0 && (board[i][j] & 1) == t)//遍历棋盘搜索移动选择 
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
	return num;//返回该局面下某一方选择的个数 
}
void cpynumFlag(int depth1,int depth2)//旗子数量复制函数 
{
	if(depth2==1)
	{
		tempnumFlag[depth1][1]=numFlag[1];
		tempnumFlag[depth1][2]=numFlag[2];
	}
	else if(depth2==0)
	{
		numFlag[1]=tempnumFlag[depth1][1];
		numFlag[2]=tempnumFlag[depth1][2];
	}
}
void cpyboard(char desboard[BOARD_SIZE][BOARD_SIZE],char iniboard[BOARD_SIZE][BOARD_SIZE])
{
	for(int i=0;i<BOARD_SIZE;i++)
		for(int j=0;j<BOARD_SIZE;j++)
			desboard[i][j]=iniboard[i][j];
} //棋盘复制函数 



int value(char currentboard[BOARD_SIZE][BOARD_SIZE])//估值函数 
{
	float valuation=0;
	for(int i=0;i<BOARD_SIZE;i++)
		for(int j=0;j<BOARD_SIZE;j++)
		{
			if(currentboard[i][j]==MY_FLAG)
				valuation++; 
			if(currentboard[i][j]==MY_KING)
				valuation+=3; 
			if(currentboard[i][j]==ENEMY_FLAG)
				valuation-=0.5;
			if(currentboard[i][j]==ENEMY_KING)
				valuation-=1.5;	
		}
	return valuation;
}

struct Command minimax()//极大极小 
{
	int numstep[8];//每一层走法选择个数 
	char tempboard0[BOARD_SIZE][BOARD_SIZE];
	char tempboard1[BOARD_SIZE][BOARD_SIZE];
	char tempboard2[BOARD_SIZE][BOARD_SIZE];
	char tempboard3[BOARD_SIZE][BOARD_SIZE];
	char tempboard4[BOARD_SIZE][BOARD_SIZE];
	char tempboard5[BOARD_SIZE][BOARD_SIZE];
	char tempboard6[BOARD_SIZE][BOARD_SIZE];
	char tempboard7[BOARD_SIZE][BOARD_SIZE];//临时棋盘，用来复原 
	struct Command mycommand0;
	struct Command allmycommand0[20];
	struct Command allenemycommand1[20];
	struct Command allmycommand2[20];
	struct Command allenemycommand3[20];
	struct Command allmycommand4[20];
	struct Command allenemycommand5[20];
	struct Command allmycommand6[20];
	struct Command allenemycommand7[20];//每一层某方所有选择 
	//极大极小开始
	cpyboard(tempboard0,board);
	cpynumFlag(0,1);
	numstep[0]=search(1);
	int alpha0=-10000;
	int beta0=10000;
	int maxvaluation0=-10000;
	for(int i=0;i<numstep[0];i++)
		allmycommand0[i]=allcommand[i];
	for(int step0=0;step0<numstep[0];step0++)//第一层 
	{
		place(allmycommand0[step0]);
		cpyboard(tempboard1,board);
		cpynumFlag(1,1);
		numstep[1]=search(2);
		int alpha1=alpha0;
		int beta1=beta0;
		int minvaluation1=10000;
		for(int i=0;i<numstep[1];i++)
			allenemycommand1[i]=allcommand[i];
		for(int step1=0;step1<numstep[1];step1++)//第二层 
		{
			place(allenemycommand1[step1]);
			cpyboard(tempboard2,board);
			cpynumFlag(2,1);
			numstep[2]=search(1);
			int alpha2=alpha1;
			int beta2=beta1;
			int maxvaluation2=-10000;
			for(int i=0;i<numstep[2];i++)
				allmycommand2[i]=allcommand[i];
			for(int step2=0;step2<numstep[2];step2++)//第三层 
			{
				place(allmycommand2[step2]);
				cpyboard(tempboard3,board);
				cpynumFlag(3,1);
				numstep[3]=search(2);
				int alpha3=alpha2;
				int beta3=beta2;
				int minvaluation3=10000;
				for(int i=0;i<numstep[3];i++)
					allenemycommand3[i]=allcommand[i];
				for(int step3=0;step3<numstep[3];step3++)//第四层
				{
					place(allenemycommand3[step3]);
					cpyboard(tempboard4,board);
					cpynumFlag(4,1);
					numstep[4]=search(1);
					int alpha4=alpha3;
					int beta4=beta3;
					int maxvaluation4=-10000;
					for(int i=0;i<numstep[4];i++)
						allmycommand4[i]=allcommand[i];
					for(int step4=0;step4<numstep[4];step4++)//第五层
					{
						place(allmycommand4[step4]);
						cpyboard(tempboard5,board);
						cpynumFlag(5,1);
						numstep[5]=search(2);
						int alpha5=alpha4;
						int beta5=beta4;
						int minvaluation5=10000;
						for(int i=0;i<numstep[5];i++)
							allenemycommand5[i]=allcommand[i];
						for(int step5=0;step5<numstep[5];step5++)//第六层 
						{
							place(allenemycommand5[step5]);
							cpyboard(tempboard6,board);
							cpynumFlag(6,1);
							numstep[6]=search(1);
							int alpha6=alpha5;
							int beta6=beta5;
							int maxvaluation6=-10000;
							for(int i=0;i<numstep[6];i++)
								allmycommand6[i]=allcommand[i];
							for(int step6=0;step6<numstep[6];step6++)//第七层 
							{
								place(allmycommand6[step6]);
								cpyboard(tempboard7,board);
								cpynumFlag(7,1);
								numstep[7]=search(2);
								int alpha7=alpha6;
								int beta7=beta6;
								int minvaluation7=10000;
								for(int i=0;i<numstep[7];i++)
									allenemycommand7[i]=allcommand[i];
								for(int step7=0;step7<numstep[7];step7++)//第八层 
								{
									place(allenemycommand7[step7]);
									if(value(board)<minvaluation7)
									{
										minvaluation7=value(board);
									}
									if(minvaluation7<beta7)
										beta7=minvaluation7;
									cpyboard(board,tempboard7);
									cpynumFlag(7,0);
								}
								if(minvaluation7>maxvaluation6)
								{
									maxvaluation6=minvaluation7;
								}
								if(maxvaluation6>alpha6)
									alpha6=maxvaluation6;
								cpyboard(board,tempboard6);
								cpynumFlag(6,0);
							}
							if(maxvaluation6<minvaluation5)
							{
								minvaluation5=maxvaluation6;
							}
							if(minvaluation5<beta5)
								beta5=minvaluation5;
							cpyboard(board,tempboard5);
							cpynumFlag(5,0);
						}
						if(minvaluation5>maxvaluation4)
						{
							maxvaluation4=minvaluation5;
						}
						if(maxvaluation4>alpha4)
							alpha4=maxvaluation4;
						cpyboard(board,tempboard4);
						cpynumFlag(4,0);
					}
					if(maxvaluation4<minvaluation3)
					{
						minvaluation3=maxvaluation4;
					}
					if(minvaluation3<beta3)
						beta3=minvaluation3;
					cpyboard(board,tempboard3);
					cpynumFlag(3,0);
				}
				if(minvaluation3>maxvaluation2)
				{
					maxvaluation2=minvaluation3;
				}
				if(maxvaluation2>alpha2)
					alpha2=maxvaluation2;
				cpyboard(board,tempboard2);
				cpynumFlag(2,0);
			}
			if(maxvaluation2<minvaluation1)
			{
				minvaluation1=maxvaluation2;
			}
			if(minvaluation1<beta1)
				beta1=minvaluation1;
			cpyboard(board,tempboard1);
			cpynumFlag(1,0);
		}
		if(minvaluation1>maxvaluation0)
		{
			maxvaluation0=minvaluation1;
			mycommand0=allmycommand0[step0];
		}
		if(maxvaluation0>alpha0)
			alpha0=maxvaluation0;
		cpyboard(board,tempboard0);
		cpynumFlag(0,0);
	}
	return mycommand0;
}

void initAI(int me)
{
    numFlag[1]=12;
	numFlag[2]=12;
}


struct Command aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me)
{

    return minimax();
}


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
}//八层不带剪枝 
