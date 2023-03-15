#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS 0
#define FAILED 1
#define MAX_LEN 100 	/* Maximum number of characters in one line of input */

void print_number(FILE *p, int num);


/* Program gets from user integer numbers and write them by words */
/* Returns zero if all input read and wrote successfully, otherwise 1 */
int main(int argc, char *argv[])
{
	FILE *ifp; 				/* input */
	FILE *ofp; 				/* output */
	char line[MAX_LEN];	
	int num;
	
	switch(argc)
	{
		case 1:	/* none arguments were sent */ 
				ifp = stdin;
   			ofp = stdout; 
				printf("\nPlease enter numbers.\n");break;
		case 2: /* input from file and output to standrat output */
				ifp = fopen(argv[1], "r");
				ofp = stdout; break;
		case 3: /* input and output to file */
				ifp = fopen(argv[1], "r");
				ofp = fopen(argv[2], "r+"); break;	
		default: printf("\nToo many arguments\n");break;
	}

	/* Checks if files were opened successfully */
	if(!ifp || !ofp)	
	{
		fprintf(stderr, "\n Can not open file \n");
		exit(FAILED);
	}

	/* Read the file */
	while(fgets(line,MAX_LEN,ifp)!=NULL)
	{	
		char *input = line;		
		int p;

		while(sscanf(input,"%d%n",&num, &p)==1)
		{	
			if(num < 0 || num > 99)
			{
				fprintf(stderr, "\nNumber must be in range from 0 to 99\n");
				exit(FAILED);
			}
			else
			{
				input += p;  /* Moves pointer to the next number */
				print_number(ofp,num);
			}
		}  /* End of inner loop - sscanf */
	}	/* End of outer loop - fgets */

	return (SUCCESS);
}

void print_number(FILE *fd, int num)
{
	int dozen = 0;

	if(num >= 20) {
		dozen = num/10;
		num = num%10;
	
		switch(dozen)
		{
			case 2: fprintf(fd, "twenty "); break;
			case 3: fprintf(fd, "thirty "); break;
			case 4: fprintf(fd, "fourty "); break;
			case 5: fprintf(fd, "fifty "); break;
			case 6: fprintf(fd, "sixty "); break;
			case 7: fprintf(fd, "seventy "); break;
			case 8: fprintf(fd, "eighty "); break;
			case 9: fprintf(fd, "ninety "); break;
		}
	}

	if(num == 0 && dozen == 0)
		printf("zero\n");

	else
	{
		switch(num)
			{
				case 1: fprintf(fd, "one\n"); break;
				case 2: fprintf(fd, "two\n"); break;
				case 3: fprintf(fd, "three\n"); break;
				case 4: fprintf(fd, "four\n"); break;
				case 5: fprintf(fd, "five\n"); break;
				case 6: fprintf(fd, "six\n"); break;
				case 7: fprintf(fd, "seven\n"); break;
				case 8: fprintf(fd, "eight\n"); break;
				case 9: fprintf(fd, "nine\n"); break;
				case 10: fprintf(fd, "ten\n"); break;
				case 11: fprintf(fd, "eleven\n"); break;
				case 12: fprintf(fd, "twelve\n"); break;
				case 13: fprintf(fd, "thirteen\n"); break;
				case 14: fprintf(fd, "fourteen\n"); break;
				case 15: fprintf(fd, "fifteen\n"); break;
				case 16: fprintf(fd, "sixteen\n"); break;
				case 17: fprintf(fd, "seventeen\n"); break;
				case 18: fprintf(fd, "eighteen\n"); break;
				case 19: fprintf(fd, "nineteen\n"); break;
			}
	}
}
