#include "adjacency.h"


int main()
{
	char val;
	int v,u; /* indexes of nodes */
	int full = FALSE; /* matrix filling indicator */
	
	adjmat mat = {{0}};

	printf("\nPlease enter a list values for matrix. Size martrix is %d*%d."
			"\nValues can be 0 or 1.\n", N,N);

	while(full == FALSE && (val = getchar()) != EOF) {
		if(val == '0' || val == '1')
			full = fill_mat(mat, val); /* filling the matrix */
		else if(!isspace(val))
			printf("Input must contains only 0 or 1. Other symbols will be "
					"dismiss.\n");
	}
	print_mat(mat);

	do 
	{
		printf("\nPlease enter two indexes of nodes. \nIt must be numbers from 0 "
			"to %d. For exit enter -1, -1.\n", N);	
		if((scanf("%d%d", &v,&u) == 2)) {
			if(path(mat, v, u))
				printf("From node %d to node %d a path exists\n", v,u);
			else
				printf("From node %d to node %d a path doesn't exist\n", v,u);
		}
	}while(v!=-1 && u!=-1);
}

int fill_mat(adjmat mat, char val)
{
	static int full = FALSE;	
	static int count = 0;	 /* filled cell counter */
	static int row = 0;
	static int col = 0;

	if(!full) {
		if(col == LENGTH) {
			row++;
			col = 0;	
		}
		mat[row][col] = val - '0';
		col++;		
		count++;
	
		if(count == LENGTH*LENGTH) /* maximum number of cells */
		{
			full = TRUE;
			printf("Matrix is full.");
		}
	}

	return full;
}

void print_colindex();
void print_row(adjmat mat, int row);

void print_mat(adjmat mat)
{
	int row;
	
	print_colindex();
	for(row=0; row < LENGTH; row++)
		print_row(mat,row);
}

void print_colindex(int len)
{
	int col;
	
	printf("\n%4c%c", ' ', '|');
	for(col=0; col < LENGTH; col++)
		printf("%4d", col);
	printf("\n%4c%c", '_', '|');
	for(col=0; col < LENGTH; col++) 
		printf("%4c", '_');
	printf("\n");
}

void print_row(adjmat mat, int row)
{
	int col;

	printf("%4d%c", row, '|');
	for(col=0; col < LENGTH; col++)
		printf("%4d", mat[row][col]);
	printf("\n");
}

int path(adjmat mat, int u, int v)
{
	int i;	
	int res = FALSE;

	if(u >= LENGTH || v >= LENGTH || u < 0 || v < 0)
		printf("Error: incorrect indexes.");
	else if(u == v)
		res = TRUE;
	else {
	 	res = mat[u][v];
		if(res == FALSE) {
			for(i=0; i < LENGTH && mat[i][v] != TRUE; i++);
			if(i != LENGTH)
				res = path(mat,u, i);
		}
	}

	return res;
}
