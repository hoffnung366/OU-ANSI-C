/* Supporting functions for creating words */
#include "data.h"

static struct word_arg
{
    char *arg_name;
    int st_bit;    /* number of start bit for argument */
} word_args[] = {
        {"data", 0},           /* use bits 0-23 */
        {"string", 0},         /* use bits 0-23 */
        {"digit", 3},          /* use bits 3-23 */
        {"opcode", 18},        /* use bits 18-23 */
        {"src_mode", 16},      /* use bits 16-17 */
        {"src_arg", 13},       /* use bits 13-15 */
        {"dest_mode", 11},     /* use bits 11-12 */
        {"dest_arg", 8},       /* use bits 8-10 */
        {"funct", 3},          /* use bits 3-7 */
        {"ARE", 0}             /* use bits 0-2 */
};

boolean fillWordArg(word *curr, char *w_arg, long int arg, int nline);
word *createFirstWord(int opcode, int funct, access are, address_mode src_mode, int src_arg,
                      address_mode dest_mode, int dest_arg, int nline);
word *createAdditionalWord(int digit, access are, int nline);


/* Fills the current word, by left shift the data on number of bits accord w_arg */
/* and inserts the value into the current word. */
boolean fillWordArg(word *curr, char *w_arg, long int arg, int nline)
{
    boolean valid = TRUE;       /* by default word filled successfully */

    word mask;
    int i;
    int len = sizeof(word_args)/sizeof(struct word_arg);  /* number of word arguments */

    /* Looks an index of type word argument */
    for(i=0; i < len && strcmp(w_arg,word_args[i].arg_name); i++)
    ;

    /* Checks if loop ends and any word argument didn't match to w_arg  */
    if(i == len)
    {
        printError(W_ARG, nline, w_arg);
        valid = FALSE;
    }
    else
    {
        mask.binary = arg << word_args[i].st_bit;            /* left shift the given value */
        curr->binary = curr->binary | mask.binary;           /* inserts arg into the current word */
    }

    return valid;
}

/* Create a first word for operation */
/* Return pointer to word, if create successfully, otherwise NULL */
word *createFirstWord(int opcode, int funct, access are, address_mode src_mode, int src_arg,
                      address_mode dest_mode, int dest_arg, int nline)
{
    boolean valid = FALSE;      /* by default word did not create */
    word *temp;

    /* create a dynamic location for new word */
    if(!(temp = (word *) calloc(1,sizeof(word))))
    {
        fprintf(stderr, "Exit: No memory\n");
        exit(1);
    }

    if(fillWordArg(temp, "opcode", opcode, nline))
        if(fillWordArg(temp, "funct", funct, nline))
            if(fillWordArg(temp, "ARE", are, nline))
                if(fillWordArg(temp, "src_mode", src_mode, nline))
                    if(fillWordArg(temp, "src_arg", src_arg, nline))
                        if(fillWordArg(temp, "dest_mode", dest_mode, nline))
                            if(fillWordArg(temp, "dest_arg", dest_arg, nline))
                                valid = TRUE;

    /* checks if word did not create */
    if(!valid)
    {
        free(temp);
        return NULL;
    }

    return temp;
}


/* Create a additional word for operation */
/* Return pointer to word, if create successfully, otherwise NULL */
word *createAdditionalWord(int digit, access are, int nline)
{
    boolean valid = FALSE;      /* by default word did not create */
    word *temp;

    /* create a dynamic location for new word */
    if(!(temp = (word *) calloc(1,sizeof(word))))
    {
        fprintf(stderr, "Exit: No memory\n");
        exit(1);
    }

    if(fillWordArg(temp, "digit", digit, nline))
        if(fillWordArg(temp, "ARE", are, nline))
            valid = TRUE;

    /* checks if word did not create */
    if(!valid)
    {
        free(temp);
        return NULL;
    }

    return temp;
}