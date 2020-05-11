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
int numMyFlag;
struct Command moveCmd = { .x={0}, .y={0}, .numStep=2 };
struct Command jumpCmd = { .x={0}, .y={0}, .numStep=0 };
struct Command longestJumpCmd = { .x={0}, .y={0}, .numStep=1 };

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

void printfboard(char tempboard[BOARD_SIZE][BOARD_SIZE]) 
{
	for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            switch (board[i][j])
            {
                case EMPTY:
                    tempboard[i][j] = '.';
                    break;
                case ENEMY_FLAG:
                    tempboard[i][j] = 'O';
                    break;
                case MY_FLAG:
                    tempboard[i][j] = 'X';
                    break;
                case ENEMY_KING:
                    tempboard[i][j] = '@';
                    break;
                case MY_KING:
                   tempboard[i][j] = '*';
                    break;
                default:
                    break;
            }
        }
        printf("%s\n", tempboard[i]);
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
    int newX, newY;
    for (int i = 0; i < board[x][y]; i++)
    {
        newX = x + moveDir[i][0];
        newY = y + moveDir[i][1];
        if (isInBound(newX, newY) && board[newX][newY] == EMPTY)
        {
            moveCmd.x[0] = x;
            moveCmd.y[0] = y;
            moveCmd.x[1] = newX;
            moveCmd.y[1] = newY;
            return i;
        }
    }
    return -1;
}

void tryToJump(int x, int y, int currentStep)
{
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
        if (isInBound(newX, newY) && (board[midX][midY] & 1) && (board[newX][newY] == EMPTY))
        {
            board[newX][newY] = board[x][y];
            board[x][y] = EMPTY;
            tmpFlag = board[midX][midY];
            board[midX][midY] = EMPTY;
            tryToJump(newX, newY, currentStep + 1);
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
                numMyFlag--;
            }
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
 * ��Ĵ��뿪ʼ
 */

/**
 * You can define your own struct and variable here
 * ����������ﶨ�����Լ��Ľṹ��ͱ���
 */

/**
 * ������������ʼ�����AI
 */
 
 
int value(char board[BOARD_SIZE][BOARD_SIZE])
{
	int valuation=0;
	for(int i=0;i<BOARD_SIZE;i++)
		for(int j=0;j<BOARD_SIZE;j++)
		{
			switch(board[i][j])
			{
				case MY_FLAG :
					valuation++;
					break;
				case MY_KING :
					valuation+=3;
					break;
				case ENEMY_FLAG:
					valuation--;
					break;
				case ENEMY_KING :
				 	valuation-=2;
				 	break;
			}	
		}
	return valuation;
} //��ֵ�������Ե��ұ���������������Ϊ���� 

char tempboard[BOARD_SIZE][BOARD_SIZE];//aiTURN ���������const�ͣ��޷��ı䣬���Զ���һ����ʱ���� 


void cpyboard(const char board[BOARD_SIZE][BOARD_SIZE],char desboard[BOARD_SIZE][BOARD_SIZE])
{
	for(int i=0;i<BOARD_SIZE;i++)
		for(int j=0;j<BOARD_SIZE;j++)
			desboard[i][j]=board[i][j];
}//���̸��ƺ��� 

void tempplace(struct Command cmd)
{
    int midX, midY, curFlag;
    int tempnumMyFlag=numMyFlag;
    curFlag = tempboard[cmd.x[0]][cmd.y[0]];
    for (int i = 0; i < cmd.numStep - 1; i++)
    {
        tempboard[cmd.x[i]][cmd.y[i]] = EMPTY;
        tempboard[cmd.x[i + 1]][cmd.y[i + 1]] = curFlag;
        if (abs(cmd.x[i] - cmd.x[i + 1]) == 2)
        {
            midX = (cmd.x[i] + cmd.x[i + 1]) / 2;
            midY = (cmd.y[i] + cmd.y[i + 1]) / 2;
            if ((tempboard[midX][midY] & 1) == 0)
            {
                tempnumMyFlag--;
            }
            tempboard[midX][midY] = EMPTY;
        }
    }
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (tempboard[0][i] == ENEMY_FLAG)
        {
            tempboard[0][i] = ENEMY_KING;
        }
        if (tempboard[BOARD_SIZE - 1][i] == MY_FLAG)
        {
            tempboard[BOARD_SIZE - 1][i] = MY_KING;
        }
    }
}//��ʱ�������ӵĺ����������ı���ʱ���̵ľ��� 

void initAI(int me)
{
    numMyFlag = 12;
}

/**
 * �ֵ������ӡ�
 * ������0��ʾ�հף�1��ʾ���壬2��ʾ����
 * me��ʾ��������������(1��2)
 * ����Ҫ����һ���ṹ��Command������numStep����Ҫ�ƶ������Ӿ����ĸ�����������㡢�յ㣩��
 * x��y�ֱ��Ǹ��������ξ�����ÿ�����ӵĺᡢ������
 */
struct Command aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me)
{
    /*
     * TODO��������д�����AI��
     */
    int maxvaluation=0;
    struct Command bestcommand;
    struct Command command =
    {
        .x = {0},
        .y = {0},
        .numStep = 0
    };
    int numChecked = 0;
    int maxStep = 1;
	    for (int i = 0; i < BOARD_SIZE; i++)
	    {
	        for (int j = 0; j < BOARD_SIZE; j++)
	        {
	        	cpyboard(board,tempboard);//ÿһ�ζ���ԭʼ���̴�����ʱ���� 
	            if (board[i][j] > 0 && (board[i][j] & 1) == 0)
	            {
	                numChecked++;
	                longestJumpCmd.numStep = 1;
	                tryToJump(i, j, 0);
	                if (longestJumpCmd.numStep > maxStep)
	                {
	                    memcpy(&command, &longestJumpCmd, sizeof(struct Command));
	                }
	                if (command.numStep == 0)
	                {
	                    if (tryToMove(i, j) >= 0)
	                    {
	                        memcpy(&command, &moveCmd, sizeof(struct Command));
	                    }
	                }
	            }
	            tempplace(command); //������ʱ������ִ������
				printfboard(tempboard); 
        		if(value(tempboard)>=maxvaluation)//��ֵ��ȡ�������� 
		        {
		        	maxvaluation=value(tempboard);
		        	memcpy(&bestcommand, &command, sizeof(struct Command));
				}
				if( numChecked>=numMyFlag)
					return bestcommand; 
	        }    		
        } 
    return bestcommand;
}

/**
 * ��Ĵ������
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