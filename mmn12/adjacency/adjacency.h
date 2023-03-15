#include <stdio.h>
#include <ctype.h>

#define N 15 /*ammount of nodes in tree*/
#define LENGTH N

enum {FALSE, TRUE};

typedef int adjmat[N][N];

int fill_mat(adjmat mat, char val);
void print_mat(adjmat mat);
int path(adjmat mat, int row, int col);



