/* */
#include "data.h"

static code_ptr c_head = NULL;
static code_ptr c_prev = NULL;

static data_ptr d_head = NULL;
static data_ptr d_prev = NULL;

#define CREATE_GETFIRST(TYPE, FUNC_NAME, HEAD_NAME) \
TYPE *FUNC_NAME()                                   \
{                                                   \
    return HEAD_NAME;                               \
}                                                   \

CREATE_GETFIRST(code, getFirstCode, c_head);
CREATE_GETFIRST(data, getFirstData, d_head);

void adddata(word new, int address, int nline)
{
    data_ptr temp;

    /* create a dynamic location for new node */
    if(!(temp = (data_ptr) calloc(1,sizeof(data))))
    {
        fprintf(stderr, "Exit: No memory\n");
        exit(1);
    }

    /* fill the values */
    temp->address = address;
    temp->text.binary = new.binary;

    if(d_head == NULL)
        d_head = temp;
    else
        d_head->next = temp;

    d_head = temp;
}

void addcode(word new, int address, int L, char **operands, int nline)
{
    code_ptr temp;

    /* create a dynamic location for new node */
    if(!(temp = (code_ptr) calloc(1,sizeof(code))))
    {
        fprintf(stderr, "Exit: No memory\n");
        exit(1);
    }

    /* fill the values */
    temp->address = address;
    temp->text.binary = new.binary;
    temp->L = L;
    temp->operands = operands;
    temp->nline = nline;
    code_ptr next;

    if(d_head == NULL)
        d_head = temp;
    else
        d_head->next = temp;

    d_head = temp;
}

void freeDatatable(data_ptr head)
{
    data_ptr temp;

    while(d_head)
    {
        temp = d_head;
        d_head = d_head->next;
        free(temp);
    }
}

void freeCodetable(code_ptr head)
{
    code_ptr temp;
    int i;

    while(c_head)
    {
        /* checks if operands are exists */
        if(c_head->operands != NULL)
        {
            /* free memory */
            for(i = 0; i < MAX_OP; i++)
                free(c_head->operands[i]);
            free(c_head->operands);
        }

      /*  free(c_head->text);*/       /* CHECK LETTER !!!!!! */

      temp = c_head;
      c_head = c_head->next;
      free(temp);
    }
}