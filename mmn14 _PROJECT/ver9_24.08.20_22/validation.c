/* Validations a statement's parts */
#include "data.h"

static char *registers[] =  {"r0","r1","r2","r3","r4","r5","r6","r7"};
static char *labels[] = {"data", "string", "entry", "extern"};
static struct operation
{
    char *opname;
    int opcode;
    int funct;
    int num_of_operands;
    address_mode src_modes[MAX_MODE];
    address_mode dest_modes[MAX_MODE];
} operations[] = {
        {"mov", 0, 0, 2, {IMMEDIATE, LINK, REGISTER}, {LINK, REGISTER, NONE}},
        {"cmp", 1, 0, 2, {IMMEDIATE, LINK, REGISTER}, {IMMEDIATE, LINK, REGISTER}},
        {"add", 2, 1, 2, {IMMEDIATE, LINK, REGISTER}, {LINK, REGISTER, NONE}},
        {"sub", 2, 2, 2, {IMMEDIATE, LINK, REGISTER}, {LINK, REGISTER, NONE}},
        {"lea", 4, 0, 2, {LINK, NONE, NONE}, {LINK, REGISTER, NONE}},
        {"clr", 5, 1, 1, {NONE, NONE, NONE}, {LINK, REGISTER, NONE}},
        {"not", 5,2, 1, {NONE, NONE, NONE}, {LINK, REGISTER, NONE}},
        {"inc", 5, 3, 1, {NONE, NONE, NONE}, {LINK, REGISTER, NONE}},
        {"dec", 5, 4, 1, {NONE, NONE, NONE}, {LINK, REGISTER, NONE}},
        {"jmp", 9, 1, 1, {NONE, NONE, NONE}, {LINK, RELATIVE, NONE}},
        {"bne", 9, 2, 1, {NONE, NONE, NONE}, {LINK, RELATIVE, NONE}},
        {"jsr", 9, 3, 1, {NONE, NONE, NONE}, {LINK, RELATIVE, NONE}},
        {"red", 12,0, 1, {NONE, NONE, NONE}, {LINK, REGISTER, NONE}},
        {"prn", 13,0, 1, {NONE, NONE, NONE}, {IMMEDIATE, LINK, REGISTER}},
        {"rts", 14,0, 0, {NONE, NONE, NONE}, {NONE, NONE, NONE}},
        {"stop",15,0, 0, {NONE, NONE, NONE}, {NONE, NONE, NONE}}
};

#define CREATE_KEY_CHECK(FUNC_NAME, ARR_NAME, TYPE)   \
/* Checks if str is keyword (by his type)           */\
/* Returns TRUE, if founded match, otherwise FALSE. */\
boolean FUNC_NAME(char * str)                         \
{                                                     \
    /* by default is not a keyword */                 \
    boolean flag = FALSE;                             \
    int i;                                            \
                                                      \
    /* number of keys for TYPE */                     \
    int len = sizeof(ARR_NAME)/sizeof(TYPE);          \
    /* looks if str match to any key by TYPE */       \
    for(i=0; i < len && !flag; i++)                   \
        if(!strcmp(str,ARR_NAME[i]))                  \
            flag = TRUE;                              \
                                                      \
    return flag;                                      \
}

/* functions for checking if str is one of keyword */
CREATE_KEY_CHECK(isRegister, registers, char *)
CREATE_KEY_CHECK(isLabel, labels, char *);
int isOperation(char * str);
boolean isKeyword(char *str);

/* functions for validations the parts of line */
boolean isSymbolValid(char *symbol, int nline);
boolean isLabelValid(char *label, int nline);
int indexValidOperation(char *op, int nline);
boolean isModeValid(address_mode mode, int index, enum operand_type type, int nline);
boolean isDigitOperand(char *operand, address_mode mode, int index, int nline);


/* Checks if str is one of operation keyword               */
/* Returns index the keyword, if they match, otherwise -1. */
int isOperation(char * str)
{
    int i;
    /* number of operations */
    int len = sizeof(operations)/sizeof(struct operation);

    /* looks if str match to any operation key */
    for(i=0; i < len && (strcmp(str,operations[i].opname) != 0); i++);

    return ((i == len) ? -1 : i);
}

/* Checks is str is one of keywords. */
boolean isKeyword(char *str)
{
    if(isRegister(str) || isLabel(str) || (isOperation(str) >= 0))
        return TRUE;

    return FALSE;
}

/* Checks if symbol contains only valid characters. */
/* Returns TRUE, if check passed, otherwise FALSE.  */
boolean isSymbolValid(char *symbol, int nline)
{
    boolean valid = FALSE;              /* by default symbol is not valid, until will pass all checks */
    int i, len;

    len = strlen(symbol);

    if (isalpha(symbol[0]))                /* symbol has to start from alphabet letter (upper or lower) */
    {
       /* empty loop for checks if all character are alphabet letters or digits */
       for (i = 1; i < len && isalnum(symbol[i]); i++)
       ;

       /* checks if loop ends earlier, not valid character was found */
       if (i < len)
           printError(INV_CHAR, nline, symbol);
       /* checks if symbol has forbidden length */
       else if(len > SMAXLEN)
           printError(LEN_SYMBOL, nline, SMAXLEN);
       else if(isKeyword(symbol))
           printError(KEY_SYMBOL,nline, symbol);
       else
           valid = TRUE;
    }

    return valid;
}

/* Checks if label contains in the labels list.    */
/* Returns TRUE, if check passed, otherwise FALSE. */
boolean isLabelValid(char *label, int nline)
{
    boolean valid = TRUE;               /* by default label is valid, until will not found error */

    /* checks if the label contains in a labels list */
    if(!isLabel(label))
    {
        printError(UN_LABEL,nline,label);
        valid = FALSE;
    }

    return valid;
}

/* Checks if op is one of operation keyword and contains only valid characters.*/
/* Returns index the keyword, if they match, otherwise -1. */
int indexValidOperation(char *op, int nline)
{
    int i;
    boolean flag = TRUE;             /* by default all characters are lower case, until will not found upper */

    for(i = 0; i < strlen(op) && flag; i++)
    {
        if(isupper(op[i]))
        {
            op[i] = tolower(op[i]);
            flag = FALSE;
        }
    }

    /* checks if the operation contains in a labels list, by looking his index */
    if((i = isOperation(op)) < 0)
        printError(UN_OPERATION,nline,op);
    else if(!flag)
    {
        i = -1;
        printError(IN_OPERATION, nline, op);
    }

    return i;
}

/* Checks if mode is one of permitted address modes for operation. */
/* Returns TRUE, if match founded, otherwise FALSE. */
boolean isModeValid(address_mode mode, int index, enum operand_type type, int nline)
{
    boolean valid = TRUE;
    int i = 0;

    /* checks if operand does not fit for any address mode */
    if (mode == NONE)
    {
        printError(NONE_MODE, nline, operations[index].opname);
        valid = FALSE;
    }

    else
    {
        /* switch by type, if it is source or destination operand */
        switch (type) {
            /* empty loop for checking if mode are permitted for the operation */
            case SRC:
                for (; (i < MAX_MODE) && (mode != operations[index].src_modes[i]); i++);
                break;
            case DEST:
                for (; (i < MAX_MODE) && (mode != operations[index].dest_modes[i]); i++);
                break;
            default:
                valid = FALSE;    /* maximum permitted number of operands is two */
        }
    }

    /* checks if loop finished and the mode did not match to permitted modes */
    if(i == MAX_MODE)
    {
        printError(INV_MODE, nline, type);
        valid = FALSE;
    }

    return valid;
}

/* Checks if digit operand contains only valid characters */
/* Returns TRUE, if check passed, otherwise FALSE. */
boolean isDigitOperand(char *operand, address_mode mode, int index, int nline)
{
    boolean valid = TRUE;       /* by default operand is valid, until will not found error */
    int i = 1;                  /* index of character for checking, operand[0] is # */

    if(mode == IMMEDIATE)
    {
        /* checks if there is a sign */
        if(operand[i] == '-' || operand[i] == '+')
            i = i + 1;

        /* empty loop for checking that operand contains only a digits */
        for (; i < strlen(operand) && isdigit(operand[i]); i++)
        ;

        /* checks if no digit character was found */
        if (i < strlen(operand))
        {
            printError(INV_OPERAND, nline, operations[index].opname);
            valid = FALSE;
        }
    } /* end if mode is IMMEDIATE */

    else if (mode == REGISTER)
    {
        /* checks if it is not valid register */
        if (!isRegister(operand))
        {
            printError(INV_REGISTER, nline, operand);
            valid = FALSE;
        }
    }   /* end if mode is REGISTER */

    /* if mode RELATIVE or LINK (required a symbol operand, not a digit) */
    else
        valid = FALSE;

    return valid;
} /* end of function isDigitOperand */