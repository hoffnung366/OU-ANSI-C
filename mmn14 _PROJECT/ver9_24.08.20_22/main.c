/* Assembler */
/* Translates a source code of a program written in assembly language (which is defined especially to the project)
 * into a program in machine language. */
#include "data.h"
#define OK 0

extern void setCurrFile(char *file);
extern void readfile(FILE * file, char *fname);

int main(int argc, char *argv[])
{
    FILE *ifp; 				/* input file */
    char fname[FNAME_MAXLEN];
    int i;

    if(argc == 1)
    {
        fprintf(stderr, "\nError. No files for read.\n");
        return !OK;
    }

    for(i = 1; i < argc; i++)
    {
        strcpy(fname, argv[i]);
        strcat(fname,".as");
        if((ifp = fopen(fname, "r")) == NULL)
            fprintf(stderr, "\nCannot open file %s.\n\n", fname);
        else
        {
            setCurrFile(argv[i]);
            readfile(ifp, argv[i]);
            fclose(ifp);
        }
    }
    return OK;
}
