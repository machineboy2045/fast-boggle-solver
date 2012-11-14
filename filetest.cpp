#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;

    pFile = fopen ( "mydictionary.txt" , "r" );
    if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);

    // allocate memory to contain the whole file:
    buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

    // copy the file into the buffer:
    result = fread (buffer,1,lSize,pFile);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

    /* the whole file is now loaded in the memory buffer. */
    // printf("boggle_easy: %s\n", buffer);

    char * pch;
	pch = strtok (buffer,"\n\t");
	while (pch != NULL) {
		// printf ("%s\n",pch);
		pch = strtok (NULL, "\n\t");
	}

    // terminate
    fclose (pFile);
    free (buffer);
    exit(1);

}