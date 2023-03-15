#include <stdio.h>
#include <string.h>

#define SIZE 100

int match(char pattern[], char text[]);

int main()
{	
	char pattern[SIZE+2], text[SIZE+2]; /* +2 is for characters \n and \0 */
	int index, stop;

	printf("\n Please enter two strings (pattern and text). \n");
	fgets(pattern, SIZE+2, stdin);
	fgets(text, SIZE+2, stdin);
	pattern[strlen(pattern)-1] = '\0';  /*deleted character \n*/
	text[strlen(text)-1] = '\0';		/*deleted character \n*/	
	index = match(pattern, text);
	if(index == -1)
		printf("\n The match of pattern \"%s\" wasn't found in text "
				"\"%s\".\n", pattern, text);
	else
		printf("\n The best match of pattern \"%s\" was found in text "
					"\"%s\" in position %d. \n", pattern, text, index);

	return 0;
}

int match(char pattern[], char text[])
{
	int index, i, j, k, plength, tlength, max, count;
	
	plength = strlen(pattern);
	tlength = strlen(text);

	if(plength > tlength)
		index = -1;
	else {
		index = 0;   /* index of the first best match to a pattern */
		max = 0;     /* number of matching characters */
		
		for(i=0; i<=(tlength-plength) && max != plength; i++) {
			count = 0;  /* counter for matching characters */
			for(j=0, k=i; j<plength; j++, k++) {
				count += (pattern[j] == text[k]) ? 1 : 0; 
			}
			if(count > max) {
				index = i;
				max = count;
			}
		}
	}
	
	return index;
}
