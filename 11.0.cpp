#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

//����0���Է���1 
struct Command
{
    int x[MAX_STEP];
    int y[MAX_STEP];
    int numStep;
};
int singleCheckJumpNum;
int maxJumpStepNum;
int roundNum; 
char board[BOARD_SIZE][BOARD_SIZE] = {0};
int myFlag;
int moveDir[4][2] = {{1, -1}, {1, 1}, {-1, -1}, {-1, 1}};
int jumpDir[4][2] = {{2, -2}, {2, 2}, {-2, -2}, {-2, 2}};
int numFlag[2];
struct Command moveCmd = { .x={0}, .y={0}, .numStep=2 };
struct Command jumpCmd = { .x={0}, .y={0}, .numStep=0 };
struct Command singlejumpCmd = { .x={0}, .y={0}, .numStep=0 };
struct Command longestJumpCmd = { .x={0}, .y={0}, .numStep=1 };
struct Command singleCheckJumpCmd[4];
struct Command allMoveCmd[20];
struct Command allJumpCmd[20];
struct Command allCmd[20];
struct Command bestCommand;

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

int tryToMove(int x, int y)//�����ƶ� 
{
	int singleCheckMoveNum=0;
    int newX, newY;
    if(board[x][y]==MY_FLAG)//�ҷ����壬���£������������� 
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
	            allMoveCmd[singleCheckMoveNum++]=moveCmd;
	        }
	    }
	if(board[x][y]==ENEMY_FLAG)//�з����壬���ϣ������������� 
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
	            allMoveCmd[singleCheckMoveNum++]=moveCmd;
	        }
	    }
	 if(board[x][y]==MY_KING||board[x][y]==ENEMY_KING)//˫�����壬�ĸ�����ȫ������ 
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
	           	allMoveCmd[singleCheckMoveNum++]=moveCmd;
	        }
	    }
    return singleCheckMoveNum;//���ص��������ƶ�ѡ����� 
}

/*��Ծ���Ӳ���*/ 

int getBitwise(int player,int mode)//�õ�λ����ʱ�Ĳ��� 
{
	if(mode==1)
	{
		if(player==0)
			return 1;
		else return 0;	
	}
	else 
	{
		if(player==0)
			return 0;
		else return 1;
	}
}

void tryToJump(int x, int y, int currentStep,int player)//�����������ӵĳ����·��,��������� 
{
	int bitwise=getBitwise(player,1);
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
        if (isInBound(newX, newY) && board[midX][midY]>0&&((board[midX][midY] & 1)==bitwise) && (board[newX][newY] == EMPTY))
        {
            board[newX][newY] = board[x][y];
            board[x][y] = EMPTY;
            tmpFlag = board[midX][midY];
            board[midX][midY] = EMPTY;//������������Ϊempty����ֹ���� 
            tryToJump(newX, newY, currentStep + 1,player);
            board[x][y] = board[newX][newY];//��ԭ 
            board[newX][newY] = EMPTY;
            board[midX][midY] = tmpFlag;
        }
    }
    if (jumpCmd.numStep > longestJumpCmd.numStep)//ȡ�·�� 
    {
        memcpy(&longestJumpCmd, &jumpCmd, sizeof(struct Command));
    }
    jumpCmd.numStep--;
}

int getLongestJumpStep(int player) 
{
	int numChecked=0; 
	int bitwise=getBitwise(player,2);
	int maxStep = 1;
	longestJumpCmd.numStep = 1;
	for(int i=0;i<BOARD_SIZE;i++)
		for(int j=0;j<BOARD_SIZE;j++)//�ȱ������̵õ�����ж�Զ� 
		{
			if (board[i][j] > 0 && (board[i][j] & 1) == bitwise)
			{
				numChecked++;
				longestJumpCmd.numStep = 1;
				tryToJump(i, j, 0,player);
				if (longestJumpCmd.numStep > maxStep)
	                maxStep=longestJumpCmd.numStep;
	        }
	        if(numChecked>=numFlag[player])
	        	return maxStep;
		}
}

void getSingleCheckJumpCmd(int x, int y, int currentStep,int player)
{
	int bitwise=getBitwise(player,1);
    int newX, newY, midX, midY;
    char tmpFlag;
    singlejumpCmd.x[currentStep] = x;
    singlejumpCmd.y[currentStep] = y;
    singlejumpCmd.numStep++;
	if (singlejumpCmd.numStep ==maxJumpStepNum)
	    	{
	        	singleCheckJumpCmd[singleCheckJumpNum++]=singlejumpCmd;
	    	}
    for (int i = 0; i < 4; i++)
    {
        newX = x + jumpDir[i][0];
        newY = y + jumpDir[i][1];
        midX = (x + newX) / 2;
        midY = (y + newY) / 2;
        if (isInBound(newX, newY) && board[midX][midY]>0&&((board[midX][midY] & 1)==bitwise) && (board[newX][newY] == EMPTY))
        {
        	
            board[newX][newY] = board[x][y];
            board[x][y] = EMPTY;
            tmpFlag = board[midX][midY];
            board[midX][midY] = EMPTY;
            getSingleCheckJumpCmd(newX, newY, currentStep + 1,player);
            board[x][y] = board[newX][newY];
            board[newX][newY] = EMPTY;
            board[midX][midY] = tmpFlag;
        }
    }
    singlejumpCmd.numStep--;
}

int getJumpCmd(int player)//����ȫ�����ӵ���Ծ
{
	int numChecked=0; 
	int bitwise=getBitwise(player,2);
	int jumpCmdNum=0;
	if(maxJumpStepNum==1)//�޳���ѡ�񣬷���0 
		return 0;
	else
	{
		for(int i=0;i<BOARD_SIZE;i++)
			for(int j=0;j<BOARD_SIZE;j++)
			{
				if (board[i][j] > 0 && (board[i][j] & 1) == bitwise)
				{
					numChecked++;
					singlejumpCmd.numStep=0;
					singleCheckJumpNum=0;
					getSingleCheckJumpCmd(i,j,0,player);
					if(singleCheckJumpNum!=0)
						for(int i=0;i<singleCheckJumpNum;i++)
							allJumpCmd[jumpCmdNum++]=singleCheckJumpCmd[i];
		        }	
				if(numChecked>=numFlag[player])
		        	return jumpCmdNum;	//����ȫ��ĳһ������ѡ��
			}
	}
}


int getCmd(int player)//0���ң�1�ǵз���getCmd�����Ĺ����ǰ�ĳһ�������п���ѡ�����allCmd[]����������ؿ���ѡ��ĸ��� 
{
	maxJumpStepNum=getLongestJumpStep(player);
	int bitwise=getBitwise(player,2);
	int cmdNum=0;
	int jumpCmdNum=getJumpCmd(player);
	if(jumpCmdNum!=0)
	{
		for(int i=0;i<jumpCmdNum;i++)
			allCmd[cmdNum++]=allJumpCmd[i];
		return cmdNum;//�гԱس� 
	}
	else
	{
		int numChecked=0;
		for (int i = 0; i < BOARD_SIZE; i++)
		    {
		        for (int j = 0; j < BOARD_SIZE; j++)
		        {
		            if (board[i][j] > 0 && (board[i][j] & 1) == bitwise)//�������������ƶ�ѡ�� 
		            {
		            	numChecked++;
		                if (tryToMove(i, j) > 0)
		                {
		                    for(int p=0;p<tryToMove(i, j);p++)
		                    	allCmd[cmdNum++]=allMoveCmd[p];
		                }    
		            }
		            if(numChecked>=numFlag[player])
		            {
		            	return cmdNum;
					}
		        }
		    }
		}
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
                numFlag[0]--;
            }
            if (board[midX][midY] & 1)
            {
                numFlag[1]--;
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

/*�㷨����*/ 

void cpyNumFlag(int tmpNumFlag[2],int mode)//�����������ƺ��� 
{
	if(mode==0)
	{
		tmpNumFlag[0]=numFlag[0];
		tmpNumFlag[1]=numFlag[1];
	}
	else if(mode==1)
	{
		numFlag[0]=tmpNumFlag[0];
		numFlag[1]=tmpNumFlag[1];
	}
}

void cpyCheck(char desBoard[BOARD_SIZE][BOARD_SIZE],char iniBoard[BOARD_SIZE][BOARD_SIZE])//���Ӹ��ƺ���
{
	for(int i=0;i<BOARD_SIZE;i++)
		for(int j=0;j<BOARD_SIZE;j++)
			desBoard[i][j]=iniBoard[i][j];
} 

void cpyBoard(char desBoard[BOARD_SIZE][BOARD_SIZE],char iniBoard[BOARD_SIZE][BOARD_SIZE],int tmpNumFlag[2],int mode)//���̸��ƺ��� 
{
	cpyCheck(desBoard,iniBoard);
	cpyNumFlag(tmpNumFlag,mode);
}

int evaluation()//��ֵ���� 
{
	int score=0;
	for(int i=0;i<BOARD_SIZE;i++)
		for(int j=0;j<BOARD_SIZE;j++)
		{
			if(board[i][j]==MY_FLAG)
				score+=5; 
			if(board[i][j]==MY_KING)
				score+=10; 
			if(board[i][j]==ENEMY_FLAG)
				score-=1;
			if(board[i][j]==ENEMY_KING)
				score-=3;	
		}
	return score;
}

void initAI(int me)
{
    numFlag[0]=12;
	numFlag[1]=12;
}

int minimax(int currentDepth,int depth)//����С���� 
{
	int valuation=0;
 	int upperDepthValuation=0; 
	switch(currentDepth)
	{
		case 0:valuation=-10000;break;
		case 1:valuation=10000;break;
	}
	int tmpNumFlag[2];
	char tmpBoard[BOARD_SIZE][BOARD_SIZE];
	struct Command allCommand[20];
	cpyBoard(tmpBoard,board,tmpNumFlag,0);
	int cmdNum=getCmd(currentDepth%2);
	for(int i=0;i<cmdNum;i++)
		allCommand[i]=allCmd[i];
	for(int i=0;i<cmdNum;i++)
	{
		place(allCommand[i]);
		if(currentDepth==depth)
		{
			if(currentDepth%2==0)
			{
				upperDepthValuation=evaluation();
				valuation=upperDepthValuation>valuation?upperDepthValuation:valuation;
			}
			if(currentDepth%2==1)
			{
				upperDepthValuation=evaluation();
				valuation=upperDepthValuation<valuation?upperDepthValuation:valuation;		
			}		
		}
		if(currentDepth!=depth)
		{
			if(currentDepth%2==0)
			{
				upperDepthValuation=minimax(currentDepth+1,depth);
				if(upperDepthValuation>valuation&&currentDepth==0)//�ڵ�0�㷵��bestCommand 
					bestCommand=allCommand[i];
				valuation=upperDepthValuation>valuation?upperDepthValuation:valuation;
			}
			if(currentDepth%2==1)
			{
				upperDepthValuation=minimax(currentDepth+1,depth);
				valuation=upperDepthValuation<valuation?upperDepthValuation:valuation;
			}
		}
			cpyBoard(board,tmpBoard,tmpNumFlag,1);	
	}
	return valuation;
}

struct Command aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me)
{
	minimax(0,2);
	return bestCommand; 
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
        	roundNum=0; 
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
        	roundNum++;
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
