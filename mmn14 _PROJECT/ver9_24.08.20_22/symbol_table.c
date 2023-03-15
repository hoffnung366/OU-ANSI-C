/* */
#include "data.h"

static symbol_ptr head = NULL;

symbol *getFirstSymbol();
boolean addsymbol(int nline, char *sname, int address, enum symbol_type type);

symbol *getFirstSymbol()
{
    return head;
}

boolean addsymbol(int nline, char *sname, int address, enum symbol_type type)
{
    boolean res = TRUE;         /* by default symbol added successfully */
    symbol_ptr temp;
    symbol_ptr curr = NULL, prev = NULL;
    int is_unique = 1;          /* set to 0, if symbol already exist in symbol table */


    curr = head;
    while ((curr != NULL) && ((is_unique = strcmp(curr->sname, sname)) < 0))
    {
        prev = curr;
        curr = curr->next;
    }


    /* create a dynamic location for new symbol */
    if(!(temp = (symbol_ptr) calloc(1,sizeof(symbol)))) {
        fprintf(stderr, "Exit: No memory\n");
        exit(1);
    }

    /* fill the values */
    temp->sname = sname;
    temp->address = address;
    temp->type = type;


    if (!is_unique)
    {
        printError(DEF_SYMBOL, nline, sname);
        res = FALSE;
    }
    else
    {
        if (curr == head)
            head = temp;
        else
            prev->next = temp;

        temp->next = curr;
    }

    return res;
}

void freeSymbolstable()
{
    symbol_ptr temp;

    while(head)
    {
        temp = head;
        head = head->next;
        free(temp);
    }
}