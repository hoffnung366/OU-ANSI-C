#include <stdio.h>
#include <string.h>

#define MAX 100
#define ASCEND 0
#define DESCEND 1
#define NO_ORDER 2
enum boolean { false, true };

void f_sequence(char str[]);

int main()
{	
	char str[MAX+1] = {0}; /* +1 is for character \0 */

	printf("\n Please enter a series with maximum %d characters.\n", MAX);
	while (scanf("%s", str) != EOF)
		f_sequence(str);
	
	return 0;
}

void f_sequence(char str[]) 
{
	int order, strictly, length, i, next;
	
	if(str[0] == '\0' || str == NULL)
		printf("\n It is an empty string \n");
	
	else {
		length = strlen(str);
		strictly = true;
		/*checks what order must be in a series 
		by checking first and last character*/
		order = (str[0]>str[length-1]) ? DESCEND : ASCEND;

		for(i=0; i<length-1 && order!=NO_ORDER; i++) {
			if(str[i] == str[i+1])
				strictly = false;
			else {
				next = (str[i]>str[i+1]) ? DESCEND : ASCEND;	
				if(order != next)
					order = NO_ORDER;	
			}
		}

		printf("The character series is %s, ", str);	
		
		switch(order) {
			case 0: strictly ? printf("it is strictly ascending order. \n") :
					printf("it is ascending order. \n"); break;
			case 1: strictly ? printf("it is strictly descending order. \n") :
					printf("it is descending order. \n"); break;
			case 2: printf("it is with out order. \n");
		}
	}

}
