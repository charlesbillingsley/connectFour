#ifndef CONNECT_FOUR_   /* Include guard */
#define CONNECT_FOUR_

#include <argp.h>

struct arguments
{
  int width;			/* '-w' */
  int height;			/* '-h' */
  int square;			/* '-s' */
  int connect;   		/* '-c' */
  char *input_file;     /* file arg to '--load' */
};

// Function to setup values once they are parsed.
error_t parse_opt(int key, char* arg, struct argp_state *state);
int main (int argc, char **argv);
int createGameBoard(int numOfRows, int numOfCols, char** gameBoard);
void loadGameBoard(char* filename, char** gameBoard);
void saveGameBoard(int numOfRows, int numOfCols, int winSize, char* fileName, char* gameBoard);
void printGameBoard(int numOfRows, int numOfCols, char* gameBoard);
int nextTurn(char currentPlayer, int numOfRows, int numOfCols, int winSize, char* gameBoard);
char checkForWinner(char currentPlayer, int winSize,
	int numOfRows, int numOfCols, char* gameBoard);
int checkForHorWin(char currentPlayer, int winSize, 
	int numOfRows, int numOfCols, char* gameBoard);
int checkForVerWin(char currentPlayer, int winSize,
	int numOfRows, 	int numOfCols, char* gameBoard);
int checkForDiagWin(char currentPlayer, int winSize,
	int numOfRows, 	int numOfCols, char* gameBoard);

#endif // CONNECT_FOUR_
