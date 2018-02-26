#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <argp.h>
#include <math.h>
#include "connectFour.h"
#include "file_utils.h"

const char *argp_program_version =
  "argp-ex4 1.0";
const char *argp_program_bug_address =
  "<billinch@mail.gvsu.edu>";

/* Program documentation. */
static char doc[] =
  "This is a connect four game made for CIS 343 by Charles Billingsley. The argp code was based off of example 4 given from the GNU library at www.gnu.org/software/libc/manual/html_node/Argp-Example-4.html#Argp-Example-4\
options";

// Description of the required arguments we accept.
char args_doc[] = "";

/* The options we understand. */
static struct argp_option options[] = {
  {"width",    'w', 0,       0, "The width of the board" },
  {"height",   'h', 0,       0, "The height of the board" },
  {"square",   's', 0,       0, "Single parameter for a square board" },
  {"connect",  'c', 0,	     0, "Number of pieces to connect for a win" },
  {"load",     'l', 0,       0, "Load a previously saved game" },
  { 0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments arguments;

/* Parse a single option. */
error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;

  switch (key)
    {
    case 'w':
      arguments->width = arg ? atoi(arg) : 7;
      break;
    case 'h':
      arguments->height = arg ? atoi(arg) : 7;
      break;
    case 's':
      arguments->square = arg ? atoi(arg) : 0;
      break;
    case 'c':
      arguments->connect = arg ? atoi(arg) : 4;
      break;
    case 'l':
      arguments->input_file = arg;
      break;

    case ARGP_KEY_NO_ARGS:
      break;
	case ARGP_KEY_ARG:
		printf("%d arguments.\n\n", state->arg_num);
		break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, doc };

int main (int argc, char **argv) {
	
	//------------Arguments Logic--------------
	struct arguments arguments;

	/* Default values. */
	arguments.width = 7;
	arguments.height = 7;
	arguments.square = 0;
	arguments.connect = 4;
	arguments.input_file = "";

	/* Parse our arguments; every option seen by parse_opt will be
	reflected in arguments. */
	argp_parse (&argp, argc, argv, 0, 0, &arguments);
	printf ("Width = %i\n", arguments.width);
	printf ("Height = %i\n", arguments.height);
	printf ("Square = %i\n", arguments.square);
	printf ("Connect = %i\n", arguments.connect);
	printf ("Load = %p\n", arguments.input_file);

	//------------Game Logic--------------
	int playGame = 1; 
	char* newFileName = malloc(50 * sizeof(char));
	int breakAndLoad = 0;
	while (playGame) {
		int height = arguments.height;
		int width = arguments.width;
		int boardSize = height * width;
		int squareSize = arguments.square;
		int winSize = arguments.connect;
		int winnerSymbol = 0;
		int numPiecesUsedInBoard = 0;
		char currentPlayer = 'X';
		char* fileName = arguments.input_file;

		// Create a new board or Load the board
		char* gameBoard;
		if (fileName[0] != 0) {
			loadGameBoard(fileName, &gameBoard);
			char* delim = ";";
			char* removedSection;
			char* heightString;
			char* widthString;
			char* winSizeString;
			
			// Get game board section
			removedSection = strtok(gameBoard, delim);
			
			int sectionCounter = 0;
			// Move through each section of the string
			while( removedSection != NULL ) {	
				sectionCounter++;		
				removedSection = strtok(NULL, delim);
				if (sectionCounter == 1) {
					// Set up a string to capture the height
					heightString = removedSection;					
				} else if (sectionCounter == 2) {
					// Set up a string to capture the width
					widthString = removedSection;					
				} else if (sectionCounter == 3) {
					// Set up a string to capture the connection win size
					winSizeString = removedSection;				
				}
			}
			// Save the newly found requirements
			width = atoi(widthString);
			height = atoi(heightString);
			boardSize = height * width;
			winSize = atoi(winSizeString);
			
			// Loop through and count the number of pieces used
			for (int i = 0; i < boardSize; i++) {
				if (gameBoard[i] == 'X' || gameBoard[i] == 'O') {
					numPiecesUsedInBoard++;
				}
			}
		} else {
			if (squareSize > 0) {
				height = squareSize;
				width = squareSize;
				boardSize = squareSize * squareSize;
			} 
			createGameBoard(height, width, &gameBoard);
		}
		// While there are still open spots and there is no winner,
		// keep the game running
		while (numPiecesUsedInBoard < boardSize && winnerSymbol == 0) {
					printf("Pieces Used: %d / %d\n", numPiecesUsedInBoard, boardSize);
			if (breakAndLoad) {
				break;
			}
			// Print out the board
			printGameBoard(height, width, gameBoard);
			
			// Run the next turn (returns 1 if load is called)
			printf("Connect %d to win.\n",winSize);
			printf("At any time type (l) to load a game,\n or (s) to save your current game.\n");
			if (nextTurn(currentPlayer, height, width, winSize, gameBoard)){
				printf("\nFile Name: ");
				scanf("%s", newFileName);
				arguments.input_file = newFileName;
				breakAndLoad = 1;
				break;
			}					

			// Check for a winner		
			winnerSymbol = checkForWinner(currentPlayer, winSize, height, width,
			gameBoard);
			
			if (winnerSymbol == 0) {
				// Switch between players
				(currentPlayer == 'X') ? (currentPlayer = 'O') : (currentPlayer = 'X');
			}
			numPiecesUsedInBoard++;
		}
		
		// Start the game over if load is called mid game
		if (breakAndLoad) {
			breakAndLoad = 0;
			continue;
		}
		
		printGameBoard(height, width, gameBoard);
		// Verify tie or win
		if(numPiecesUsedInBoard == boardSize && winnerSymbol == 0){
			printf("Cat's Game! We're all winners here!");
		} else {
			printf("Winner Winner Chicken Dinner! \nPlayer %c wins!\n", currentPlayer);
			
			switch(winnerSymbol) {

			case 1  :
				printf("The winning move was horizontal\n");
				break;
			
			case 2  :
				printf("The winning move was vertical\n");
				break;
				
			case 3  :
				printf("The winning move was diagonal\n");
				break;
			}
		}
		
		char playAgain = 'n';
		printf("\nWould you like to play again? (yes (y) / no (n) / load (l)):");
		scanf(" %c", &playAgain);
		if (playAgain == 'y' || playAgain == 'Y') {
			arguments.input_file = "";
			continue;
		} else if (playAgain == 'l' || playAgain == 'L'){
			printf("\nFile Name: ");
			scanf("%s", newFileName);
			arguments.input_file = newFileName;
		} else {
			playGame = 0;
		}
	}
	exit (0);
}

/** 
* Builds the Connect Four board.
* @param size the number of pieces required for the game
* @param gameBoard a pointer to the pointer holding the board information
*/
int createGameBoard(int numOfRows, int numOfCols, char** gameBoard){
	int boardSize = numOfRows * numOfCols;
	char* ptBoard = malloc(boardSize * sizeof(char));
	for (int i = 0; i < boardSize; i++) {
		ptBoard[i] = '*';
	}
	*gameBoard = ptBoard;
	return 0;
}

/** 
* Loads in a Connect Four board.
*	Example Game:
*		* * * *
*		O * O *
*		X X O X
*	
*	Example Save File:
*		****O*O*XXOX;3;4;3
*	With 3 being height, 4 being width, and 3 being the win size.
* @param fileName the name of the file to load
* @param gameBoard a pointer to the array holding the board information
*/
void loadGameBoard(char* fileName, char** gameBoard){	
	// Copy the pointer
	char* ptBoard = *gameBoard;
	int success = 0;
	
	while (!success) {
		// Load the file into my pointer using file_utils
		int readRtn = read_file(fileName, &ptBoard);
		if (readRtn == 0) {
			printf("Invalid file name. Try again: ");
			scanf("%s", fileName);
		} else {
			*gameBoard = ptBoard;
			success = 1;
		}
	}
}

/** 
* Saves out a Connect Four board.
*	Example Game:
*		* * * *
*		O * O *
*		X X O X
*	
*	Example Save File:
*		****O*O*XXOX;3;4;3
*	With 3 being height, 4 being width, and 3 being the win size.
* @param numOfRows the number of rows in the board
* @param numOfCols the number of columns in the board
* @param winSize the number of connections needed to win
* @param fileName the name of the file to save
* @param gameBoard the string holding the board information
*/
void saveGameBoard(int numOfRows, int numOfCols, int winSize,
					char* fileName, char* gameBoard){
	int boardSize = numOfCols * numOfRows;
	// Check if the board has the size information in it already
	if (strstr(gameBoard, ";") != NULL) {
		write_file(fileName, gameBoard, strlen(gameBoard));
	} else {
		// Get the strings for the save info
		char* rowsString;
		char* colsString;
		char* winSizeString;
		
		// Get their lengths with an equation
		int rowsStringLength = floor(log10(abs(numOfRows))) + 1;
		int colsStringLength = floor(log10(abs(numOfCols))) + 1;
		int winSizeStringLength = floor(log10(abs(winSize))) + 1;
		
		// Allocate the memory for them
		rowsString = malloc(rowsStringLength * sizeof(char));
		colsString = malloc(colsStringLength * sizeof(char));
		winSizeString = malloc(winSizeStringLength * sizeof(char));
		
		// Fill them in
		sprintf(rowsString, "%d", numOfRows);
		sprintf(colsString, "%d", numOfCols);
		sprintf(winSizeString, "%d", winSize);
		printf("rowsString: %s\n", rowsString);
		printf("colsString: %s\n", colsString);
		printf("winSizeString: %s\n", winSizeString);
		
		/* Make a new board that is the correct size to hold the size information
		*		-boardSize makes room for board information ex: ****O*O*XXOX
		*		-add 3 to make room for the delimiters ;
		*		-add the size of the string for the rows, columns, and win size
		*/
		int saveSize = (boardSize + 3 + rowsStringLength + colsStringLength 
						+ winSizeStringLength);
		char* boardToSave = malloc(saveSize * sizeof(char));
		
		// Populate the new board with the board information
		for (int i = 0; i < boardSize; i++) {
			boardToSave[i] = gameBoard[i];
		}
		
		// Populate the new board with the first dilimiter
		boardToSave[boardSize] = ';';
		
		// Populate the new board with the row size information
		int j = 0;
		for (int i = boardSize + 1; i < boardSize + 1 + rowsStringLength; i++) {
			boardToSave[i] = rowsString[j];
			j++;
		}
		
		// Populate the new board with the second dilimiter
		boardToSave[boardSize + 1 + rowsStringLength] = ';';
		
		// Populate the new board with the column size information
		j = 0;
		for (int i = boardSize + rowsStringLength + 2;
				i < boardSize + rowsStringLength + colsStringLength + 2; i++) {
			boardToSave[i] = colsString[j];
			j++;
		}
		
		// Populate the new board with the third dilimiter
		boardToSave[boardSize + rowsStringLength + colsStringLength + 2] = ';';
		
		// Populate the new board with the win size information
		j = 0;
		for (int i = boardSize + rowsStringLength + colsStringLength + 3;
				i < saveSize; i++) {
			boardToSave[i] = winSizeString[j];
			j++;
		}
		
		// Write out the file
		write_file(fileName, boardToSave, saveSize);
	}
}

/** 
* Prints out the Connect Four board.
* @param numOfRows the number of rows in the board
* @param numOfCols the number of columns in the board
* @param gameBoard a pointer to the array holding the board information
*/
void printGameBoard(int numOfRows, int numOfCols, char* gameBoard){
	system("clear");
	for(int y = 0; y < numOfRows; y++){
		for(int x = 0; x < numOfCols; x++){
			// numOfCols*y moves to each row one by one,
			// then +x iterates thrugh the row
			printf(" %c",  gameBoard[numOfCols * y + x]); 
		}
		printf("\n");
	}
}

/**
* Prompts user for the next move and then applies it to the game board.
* @param currentPlayer the player whos turn it is
* @param numOfRows the number of rows in the board
* @param numOfCols the number of columns in the board
* @param gameBoard a pointer to the game board
*/
int nextTurn(char currentPlayer, int numOfRows, int numOfCols, int winSize,
				char* gameBoard){
	int validEntry = 0; //false
	int selectedCol = 0;
	int scannedIntInput;
	char scannedStringInput[50];
	int foundEmptyCell = 0; // Used to see whether or not a cell was found
	
	printf("It is Player %c's turn!\nPlease enter a column number: ", currentPlayer);
			
	// loop until there is a valid column choosen
	int counter = 0; // prevents endless loop
	int loadNew = 0; // Changes if a new file is to be loaded
	while (!validEntry) {
		// retrieve user input
		int rtn = scanf(" %s", scannedStringInput);
		if (rtn == EOF) {
			exit(0);
		} 
		
		if (counter >= 10){
			printf("\nToo many errors closing program.\n");
			exit(0);
		}
	
		if (scannedStringInput[0] == 's' || scannedStringInput[0] == 'S') {
			char scannedFileName[20];
			printf("Please enter a file name to save to: ");
			scanf("%s", scannedFileName);
			saveGameBoard(numOfRows, numOfCols, winSize, scannedFileName,
							gameBoard);
			printf("Game Saved!\nIt is Player %c's turn!\nPlease enter a column number: ", currentPlayer);
			continue;
		} else if (scannedStringInput[0] == 'l' || scannedStringInput[0] == 'L') {
			loadNew = 1;
			break;
		} else {
			selectedCol = atoi(scannedStringInput);
		}
		
		// Verify that the column exists on our board
		if (selectedCol < 1 || selectedCol > numOfCols) {
			printf("Invalid column. \nTry again!:");
			counter++;
			continue;
		} else {
			// Our board is 0 indexed so we must subtract 1 from the chosen column
			selectedCol = selectedCol - 1;
			
			// Loop through backward to make sure the column isn't full
			for (int y = numOfRows - 1; y >= 0 && foundEmptyCell == 0; y--) {
				if (gameBoard[numOfCols * y + selectedCol] == '*') {
					gameBoard[numOfCols * y + selectedCol] = currentPlayer;
					validEntry = 1;
					foundEmptyCell = 1;
					continue;
				}
			}
			if (!foundEmptyCell) {
				printf("Column is full. Try again!: ");
			}
		}
	}
	if (loadNew) {
		return 1;
	} else {
		printf("\nPiece successfully added!\n");
		return 0;
	}
}

/**
* Checks if the winning number of cells are connected.
* @param currentPlayer the player whos turn it is
* @param numOfRows the number of rows in the board
* @param numOfCols the number of columns in the board
* @param gameBoard a pointer to the game board
*/
char checkForWinner(char currentPlayer, int winSize,
	int numOfRows, int numOfCols, char* gameBoard){
    if (checkForHorWin(currentPlayer, winSize, numOfRows, 
		numOfCols, gameBoard) == 1){
			return 1;
	} else if (checkForVerWin(currentPlayer, winSize, numOfRows,
			numOfCols, gameBoard) == 2 ){
				return 2;
	} else if (checkForDiagWin(currentPlayer, winSize, numOfRows,
			numOfCols, gameBoard) == 3){
				return 3;
	} else {
		return 0;
	}
}

/**
* Checks if the winning number of cells are connected horizontally.
* @param currentPlayer the player whos turn it is
* @param numOfRows the number of rows in the board
* @param numOfCols the number of columns in the board
* @param gameBoard a pointer to the game board
*/
int checkForHorWin(char currentPlayer, int winSize,
	int numOfRows, int numOfCols, char* gameBoard){
	int currCell; // The current spot being evaluated
	int numFoundAdjConnects = 0; // The current number of adjacent connections found
	
	// Loop through each row checking for the expected number of connections
    for(int y = 0; y < numOfRows; y++){
		for(int x = 0; x < numOfCols; x++){
			currCell = numOfCols * y + x;
			if(gameBoard[currCell] == currentPlayer){ // If current player owns cell
				numFoundAdjConnects++; // Iterate the tally counting towards win
			} else {
				numFoundAdjConnects = 0; // Reset the tally; not enough connections
			}
			if (numFoundAdjConnects >= winSize) { // Are there enough connections?
				printf("hit Hor\n");
				return 1;
			}
		}
		numFoundAdjConnects = 0; // Reset the tally; not enough connections
    }
	numFoundAdjConnects = 0;
    return 0;
}

/**
* Checks if the winning number of cells are connected vertically.
* @param currentPlayer the player whos turn it is
* @param numOfRows the number of rows in the board
* @param numOfCols the number of columns in the board
* @param gameBoard a pointer to the game board
*/
int checkForVerWin(char currentPlayer, int winSize,
	int numOfRows, int numOfCols, char* gameBoard){
	int currCell; // The current spot being evaluated
	int numFoundAdjConnects = 0; // The current number of adjacent connections found
	
	// Loop through each column checking for the expected number of connections
    for(int x = 0; x < numOfCols; x++){
		for(int y = 0; y < numOfRows; y++){
			currCell = numOfCols * y + x;
			if(gameBoard[currCell] == currentPlayer){ // If current player owns cell
				numFoundAdjConnects++; // Iterate the tally counting towards win
			} else {
				numFoundAdjConnects = 0; // Reset the tally; not enough connections
			}
			if (numFoundAdjConnects >= winSize) { // Are there enough connections?
				printf("hit Vert\n");
				return 2;
			}
		}
		numFoundAdjConnects = 0; // Reset the tally; not enough connections	
    }
	numFoundAdjConnects = 0;
    return 0;
}

/**
* Checks if the winning number of cells are connected diagonally.
* @param currentPlayer the player whos turn it is
* @param numOfRows the number of rows in the board
* @param numOfCols the number of columns in the board
* @param gameBoard a pointer to the game board
*/
int checkForDiagWin(char currentPlayer, int winSize,
	int numOfRows, int numOfCols, char* gameBoard){
	int currCell; // The current spot being evaluated
	int numFoundAdjConnects = 0; // The current number of adjacent connections found
	int tmpX;
	int tmpY;
	
	// Loop through the top half diagonals Left to Right
	// checking for the expected number of connections
	for (int startPointY = 0; startPointY < numOfRows; startPointY++) {
		tmpX = 0;
		tmpY = startPointY;
		while (tmpY < numOfCols) {
			currCell = numOfCols * tmpY + tmpX;
			if(gameBoard[currCell] == currentPlayer){ // If current player owns cell
				numFoundAdjConnects++; // Iterate the tally counting towards win
			} else {
				numFoundAdjConnects = 0; // Reset the tally; not enough connections
			}
			if (numFoundAdjConnects >= winSize) { // Are there enough connections?
				printf("hit Diag TLR\n");
				return 3;
			}
			tmpX++;
			tmpY++;			
		}
		numFoundAdjConnects = 0;
	}
	numFoundAdjConnects = 0;
	
	// Loop through the bottom half diagonals Left to Right
	// checking for the expected number of connections
	for (int startPointX = 0; startPointX < numOfCols; startPointX++) {
		tmpX = startPointX;
		tmpY = 0;
		while (tmpX < numOfRows) {
			currCell = numOfCols * tmpY + tmpX;
			if(gameBoard[currCell] == currentPlayer){ // If current player owns cell
				numFoundAdjConnects++; // Iterate the tally counting towards win
			} else {
				numFoundAdjConnects = 0; // Reset the tally; not enough connections
			}
			if (numFoundAdjConnects >= winSize) { // Are there enough connections?
				printf("hit DiagBLR\n");
				return 3;
			}
			tmpX++;
			tmpY++;			
		}
		numFoundAdjConnects = 0;
	}
	numFoundAdjConnects = 0;
	
	// Loop through the bottom half diagonals Right to Left
	// checking for the expected number of connections
	for (int startPointY = 0; startPointY < numOfRows; startPointY++) {
		tmpX = numOfCols -1;
		tmpY = startPointY;
		while (tmpY < numOfRows) {
			currCell = numOfCols * tmpY + tmpX;
			if(gameBoard[currCell] == currentPlayer){ // If current player owns cell
				//printf("Good Cell: %d Col#s: %d Y: %d X: %d\n",
				//	currCell, numOfCols, tmpY, tmpX);
				numFoundAdjConnects++; // Iterate the tally counting towards win
			} else {
				numFoundAdjConnects = 0; // Reset the tally; not enough connections
			}
			if (numFoundAdjConnects >= winSize) { // Are there enough connections?
				printf("hit DiagBRL\n");
				return 3;
			}
			tmpX--;
			tmpY++;			
		}
		numFoundAdjConnects = 0;
	}
	numFoundAdjConnects = 0;
	
	// Loop through the top half diagonals Right to Left
	// checking for the expected number of connections
	for (int startPointX = numOfCols-1; startPointX > 0; startPointX--) {
		tmpX = startPointX;
		//printf("startx %d\n", startPointX);
		tmpY = 0;
		while (tmpX >= 0) {
			currCell = numOfCols * tmpY + tmpX;
			if(gameBoard[currCell] == currentPlayer){ // If current player owns cell
				numFoundAdjConnects++; // Iterate the tally counting towards win
				//printf("foundconnects: %d\n", numFoundAdjConnects);
				//printf("currCell: %d\n", currCell);
				//printf("x: %d y: %d\n", tmpX, tmpY);
				//printf("%d * %d + %d\n", numOfCols, tmpY, tmpX);
			} else {
				numFoundAdjConnects = 0; // Reset the tally; not enough connections
				//printf("reset\n");
			}
			if (numFoundAdjConnects >= winSize) { // Are there enough connections?
				//printf("hit DiagTRL\n");
				//printf("numFoundAdjConnects: %d\n", numFoundAdjConnects);
				//printf("winSize: %d\n", winSize);
				return 3;
			}
			tmpX--;
			tmpY++;			
		}
		numFoundAdjConnects = 0;
	}
	numFoundAdjConnects = 0;
	return 0;
}