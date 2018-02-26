#ifndef FILE_UTILS_   /* Include guard */
#define FILE_UTILS_

int read_file( char* filename, char **buffer );
int write_file( char* filename, char *buffer, int size);

#endif // FILE_UTILS_