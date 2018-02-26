#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "file_utils.h"

//Charles Billingsley
int read_file( char *filename, char **buffer ) {
	//Code given from project description to get size
	struct stat st;
	stat(filename, &st);
	int size = st.st_size;

	//Pull in the file
    	FILE *fileContents;
		if (fopen(filename, "r")) {
			fileContents = fopen(filename, "r");
		} else {
			printf("The file %s does not exist.\n", filename);
			return 0;
		}

	//Allocate memory
	char* ptBuffer;
    	ptBuffer = malloc(size * sizeof(char));
		
	//Fill in the memory
    	for (int i = 0; i < size; i++) {
		char currentChar = (char) fgetc(fileContents); 
		ptBuffer[i] = currentChar;	   
    	}
	
	*buffer = ptBuffer;
	fclose(fileContents);
	return size;
}

int write_file( char* filename, char *buffer, int size) {	
	//Pull in the file
    	FILE * fileContents = fopen(filename, "w");
	
	//Write out to file from memory
	for (int i = 0; i <= size; i++) {
		fputc(buffer[i], fileContents);
	}
	fclose(fileContents);
	return 0;
}
