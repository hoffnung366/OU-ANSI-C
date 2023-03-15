/*  */
#include "validation.c"

static int IC = FMCELL;   /* instruction counter, starts at first memory cell */
static int DC = 0;        /* data counter */

extern word *createFirstWord(int opcode, int funct, access are, address_mode src_mode, int src_arg,
                             address_mode dest_mode, int dest_arg, int nline);
extern word *createAdditionalWord(int digit, access are, int nline);
extern void adddata(word new, int address, int nline);
extern void addcode(word new, int address, int L, char **operands, int nline);
extern boolean addsymbol(int nline, char *sname, int address, enum symbol_type type);
extern boolean isGotError(char *file);
extern void makeOutputFiles(FILE *file, char *fname, int FIC, int FDC);

void readfile(FILE * file, char *fname);
void parseLine(char line[], int nline);
void checkLabelArg(int nline, boolean hasSymbol, char *symbol, char *label, char *rest);
void checkOperationArg(int nline, boolean hasSymbol, char *symbol, int index, char *rest);
void checkString(int nline, char *args);
void checkData(int nline, char *args);
void checkExtern(int nline, char *args);
void checkEntry(int nline, char *args);
char **getOperands(char *rest, int index, int start, int nline);
address_mode getArgMode(char *arg);
int getDirectDigit(char *operand);

void readfile(FILE * file, char *fname)
{
    int lc = 1;                     /* line counter */
    char line[MAX_LEN + 2] = "";         /* plus 2 for characters \0 and \n */
    unsigned int len;                        /* length of given line */
    boolean isNewLine = TRUE;

    /* Read the file */
    while(fgets(line, (MAX_LEN + 2), file) != NULL)
    {
        /* Checks if the all line was read */
        if((len = strlen(line)) == MAX_LEN && line[len + 1] != '\0') {
            printError(LEN_LINE, lc, MAX_LEN);
            isNewLine = FALSE;
        }
        else if(!isNewLine)
        {
            isNewLine = TRUE;           /* turn on flag for read a next new line */
            lc++;
        }
        else
        {
            parseLine(line, lc);
            lc++;
        }
    }	/* End of while loop - fgets */

    /* Checks if an error was NOT found in the file */
    if(!isGotError(fname))
    {
        makeOutputFiles(file, fname, IC, DC);
    }
} /* end of function readfile */


/* Parses the line into parts and checks their validity */
void parseLine(char line[], int nline)
{
    boolean valid, hasSymbol;
    char *token,  *rest;
    char symbol[MAX_LEN], label[MAX_LEN], copyline[MAX_LEN];
    char delim[] = {'\t', '\r', '\n', '\v', '\f', ' '};           /* white spaces */
    int index;

    valid = TRUE;              /* by default line is valid, until will not found error */

    /* take a copy of line for analysis arguments further */
    strcpy(copyline, line);

    /* get the first token */
    token = strtok(line, delim);

    /* checks if token is empty or comment line */
    if(token == NULL || token[0] == ';')
        return;

    /* checks if token is symbol */
    if((hasSymbol = (token[strlen(token)-1] == ':')))    /* character ':' indicates the end of symbol definition */
    {
        /* Copies token without character ':' */
        token[strlen(token)-1] = '\0';              /* put character '\0' to signify the end of a string */
        strcpy(symbol, token);

        /* checks if symbol is valid */
        if((valid = isSymbolValid(symbol, nline)))
            token = strtok(NULL, delim);                  /* move pointer to next token */
    }  /* end if: checking symbol */

    /* checks if token is label */
    if(valid && (token[0] == '.'))             /* character '.' indicates data label definitions */
    {
        /* copies token without indicator '.' */
        memmove(label, token + 1, strlen(token));

        /* checks if label is valid */
        if(isLabelValid(label, nline))
        {
            /* copy rest line (after a label) for analysis of arguments */
            rest = strstr(copyline, label) + strlen(label);
            checkLabelArg(nline, hasSymbol, symbol, label, rest);
        }
    } /* end if: checking label */

    /* checks if token is operation */
    else if(valid && ((index = indexValidOperation(token, nline)) >= 0))
    {
        /* copy rest line (after an operation) for analysis of arguments */
        rest = strstr(copyline, token) + strlen(token);
        checkOperationArg(nline, hasSymbol, symbol, index, rest);
    } /* end if: checking operation */
} /* end of function parseline */


/* Validates the given arguments for label */
void checkLabelArg(int nline, boolean hasSymbol, char *symbol, char *label, char *rest) {
    char copyrest[MAX_LEN];
    int i;

    /* checks if label has symbol*/
    if(hasSymbol)
    {
        /* checks if label is .extern or .entry */
        if((strcmp(label, "extern") == 0) || (strcmp(label, "entry") == 0))
            fprintf(stderr, "Warning (line #%d):  There is internal symbol \"%s\" for label .%s.\n",
                    nline, symbol, label);
        else
            addsymbol(nline, symbol, DC, DATA);
    }

    if(rest == NULL)
        printError(MIS_DATA, nline, label);
    else
    {
        /* empty loop for skip white spaces in the beginning of arguments */
        for (i = 0; i < strlen(rest) && isspace(rest[i]); i++)
        ;

        /* checks if arguments contain only white spaces */
        if(i == strlen(rest))
            printError(MIS_DATA, nline, label);
        /* checks if there is a forbidden comma after the label */
        else if(rest[i] == ',')
            printError(UN_COMMA, nline, label);
        else {
            /* copies arguments without whitespaces in the beginning */
            memmove(copyrest, rest + i, strlen(rest));

            /* empty loop for skip white spaces in the end of arguments */
            for(i = strlen(copyrest) - 1; i >= 0 && isspace(copyrest[i]); i--);

            /* checks if there were white spaces in the end of arguments */
            if(i != strlen(copyrest))
                copyrest[i + 1] = '\0';       /* put character '\0' to signify the end of a string */

            /* checks the given arguments depending on type of label */
            if((strcmp(label, "string") == 0))
                checkString(nline, copyrest);
            else if((strcmp(label, "data") == 0))
                checkData(nline, copyrest);
            else if((strcmp(label, "extern") == 0))
                checkExtern(nline, copyrest);
            else if((strcmp(label, "entry") == 0))
                checkEntry(nline, copyrest);
        }
    }
}  /* end of function checkLabelArg */


/* Checks if given argument it is a string, then put it to a data table */
void checkString(int nline, char *args)
{
    int i;
    word curr;

    /* checks if string contains between double quotes */
    if((args[0] == '"') && (args[strlen(args)-1] == '"'))
    {
        /* fills data table for each character separately */
        for(i = 1; i < strlen(args)-1; i++)
        {
            curr.binary = args[i];
            adddata(curr, DC, nline);
            DC++;
        }

        /* put character '\0' to data table to signify the end of a string */
        curr.binary = '\0';
        adddata(curr, DC, nline);
        DC++;
    }
    else
        printError(INV_STRING, nline);

} /* end of function checkString */


/* Checks if given arguments it is a digits, then put it to a data table */
void checkData(int nline, char *args)
{
    boolean valid = TRUE;               /* by default arguments are valid, until will not found error */
    word curr;
    long int num;
    char *ptr;

    /* get first digit argument */
    num = strtol(args, &ptr, 10);

    do
    {
        /* checks if function srttol returns an error and not a digit zero */
        if (num == 0 && ((ptr-1)[0] != '0'))
        {
            printError(INV_ARG, nline);
            valid = FALSE;
        }
        else    /* put a digit to data table */
        {
            curr.binary = num;
            adddata(curr, DC, nline);
            DC++;
        }

        /* check if there is no characters for scanning */
        if(ptr[0] == '\0')
            break;

        /* loop for skipping white spaces after a digit */
        while(valid && isspace(ptr[0]))
            ptr = ptr + 1;

        /* check if next character is a comma */
        if(valid && ptr[0] == ',')
        {
            ptr = ptr + 1;          /* move pointer to point after comma */

            /* checks if comma was last character in the arguments */
            if(ptr[0] == '\0')
            {
                printError(INV_COMMA, nline, ".data");
                valid = FALSE;
            }

            /* loop for skipping white spaces after a comma */
            while(valid && isspace(ptr[0]))
                ptr = ptr + 1;

            /* checks if there are several consecutive commas */
            if(valid && ptr[0] == ',')
            {
                printError(SEV_COMMAS, nline);
                valid = FALSE;
            }
            else         /* get next digit argument */
                num = strtol(ptr, &ptr, 10);
        }  /* end if isComma */
        else
        {
            printError(MIS_COMMA, nline, ".data");
            valid = FALSE;
        }

    } while(valid);

} /* end of function checkData */


/* Checks if given argument it is a symbol, then put it to a symbol table */
void checkExtern(int nline, char *args)
{
    int i;

    /* empty loop for looking white spaces in the argument */
    for (i = 0; i < strlen(args) && !isspace(args[i]); i++)
    ;

    /* checks that no white spaces in the argument and the symbol is valid */
    if(i == strlen(args) && isSymbolValid(args, nline))
        addsymbol(nline, args, 0, EXTERN);
    else
        printError(MANY_OPERANDS, nline, ".extern");

} /* end of function checkExtern */

/* Checks if given argument it is a symbol */
void checkEntry(int nline, char *args)
{
    int i;

    /* empty loop for looking white spaces in the argument */
    for (i = 0; i < strlen(args) && !isspace(args[i]); i++)
        ;

    /* checks if there are white spaces in the argument or the symbol is NOT valid */
    if((i != strlen(args)) || (!isSymbolValid(args, nline)))
        printError(MANY_OPERANDS, nline, ".entry");

} /* end of function checkEntry */

/* Checks if the given arguments for operation are valid*/
void checkOperationArg(int nline, boolean hasSymbol, char *symbol, int index, char *rest)
{
    boolean valid = TRUE;              /* by default arguments are valid, until will not found error */
    int L = 1;                         /* number of words required for the operation, min 1 and max 3 */
    char **operands;
    address_mode src_mode, dest_mode;
    int src_arg, dest_arg;
    long int add_val1, add_val2;
    int i, temp_L, cnt_arg = 0;
    word *temp;

    /* checks if operation has symbol*/
    if(hasSymbol)
        addsymbol(nline, symbol, IC, CODE);

    if(rest != NULL)
    {
        /* empty loop for checking that rest of line contains only white spaces */
        for(i = 0; i < strlen(rest) && isspace(rest[i]); i++)
        ;
    }

    /* checks if there are no operands (rest is NULL or contains only white spaces) */
    if ((rest == NULL) || (i == strlen(rest)))
    {
        /* checks if operands are required, but missing */
        if ((operations[index].num_of_operands > 0))
        {
            printError(FEW_OPERANDS, nline, operations[index].opname);
            valid = FALSE;
        }
        else
        {
            /* operands for operation are NOT required */
            temp = createFirstWord(operations[index].opcode, operations[index].funct, A, 0, 0, 0, 0, nline);
            addcode(*temp, IC, L, NULL, nline);
        }
    } /* end if rest is NULL or contains only white spaces */

    /* rest contains some arguments, checks if operands are NOT required for operation */
    else if(operations[index].num_of_operands == 0)
    {
        printError(MANY_OPERANDS, nline, operations[index].opname);
        valid = FALSE;
    }

    /* checks if there is a forbidden comma after the operation */
    else if (rest[i] == ',')
    {
        printError(UN_COMMA, nline, operations[index].opname);
        valid = FALSE;
    }

    /* rest contains some arguments, and one or two operands are required for operation */
    else
    {
        operands = getOperands(rest, index, i, nline);

        /* checks if operands did not pass validation */
        if(operands == NULL)
            valid = FALSE;
        else
        {
            /* counts how many operands got */
            for(i =0; i < MAX_OP; i++)
            {
                if (operands[i][0] != '\0')
                    cnt_arg++;
            }

            /* checks destination operand, value location depends by number of operands (cnt_arg) */
            dest_mode = getArgMode(operands[cnt_arg-1]);
            if((valid = isModeValid(dest_mode, index, DEST, nline)))
            {
                switch (dest_mode)
                {
                    case IMMEDIATE:
                        if((valid = isDigitOperand(operands[cnt_arg-1], dest_mode, index, nline)))
                        {
                            dest_arg = 0;
                            add_val2 = getDirectDigit(operands[cnt_arg - 1]);
                            L++;
                        } break;
                    case REGISTER:
                        if((valid = isDigitOperand(operands[cnt_arg-1], dest_mode, index, nline)))
                            dest_arg = operands[cnt_arg - 1][1] - '0';
                        break;
                    case RELATIVE:
                    case LINK:
                        dest_arg = 0;
                        L++;
                        break;
                } /* end switch by mode */
            } /* end checking destination operand */

            if(valid && (cnt_arg == 1))
            {
                /* required only one operand for operation */
                temp = createFirstWord(operations[index].opcode, operations[index].funct, A, 0, 0, dest_mode, dest_arg,
                              nline);
                addcode(*temp, IC, L, operands, nline);
            }

            else if(valid && (cnt_arg == 2))
            {
                /* checks source operand */
                src_mode = getArgMode(operands[0]);
                if((valid = isModeValid(src_mode, index, SRC, nline)))
                {
                    switch (src_mode)
                    {
                        case IMMEDIATE:
                            if((valid = isDigitOperand(operands[0], src_mode, index, nline)))
                            {
                                src_arg = 0;
                                add_val1 = getDirectDigit(operands[0]);
                                L++;
                            } break;
                        case REGISTER:
                            if((valid = isDigitOperand(operands[0], src_mode, index, nline)))
                                src_arg = operands[0][1] - '0';;
                            break;
                        case RELATIVE:
                        case LINK:
                            src_arg = 0;
                            L++;
                            break;
                    } /* end switch by mode */
                } /* end checking source operand */

                if(valid)
                {
                    /* required two operands for operation */
                    temp = createFirstWord(operations[index].opcode, operations[index].funct, A, src_mode, src_arg,
                                  dest_mode, dest_arg, nline);
                    addcode(*temp, IC, L, operands, nline);
                }

            } /* end if there are two operands */

            /* checks if an additional word required and can be filled in the first read */
            if(valid && L > 1)
            {
                if (src_mode == IMMEDIATE)
                {
                    temp = createAdditionalWord(add_val1, A, nline);
                    /* checks if second operand also required additional word */
                    temp_L = L == 3 ? L - 1 : L;
                    addcode(*temp, (IC + temp_L), 0, NULL, nline);
                }
                if (dest_mode == IMMEDIATE)
                {
                    temp = createAdditionalWord(add_val2, A, nline);
                    addcode(*temp, (IC + L), 0, NULL, nline);
                }
            } /* end if checking additional words */

        } /* end else (handling operands) */

        /* free memory */
        for(i = 0; i < MAX_OP; i++)
            free(operands[i]);
        free(operands);

    } /* end else rest of line contains something */

    /* update current memory sell after reading successfully the statement (line) */
    if(valid)
        IC = IC + L;

} /* end of function checkOperationArg */


/* Checks the address_mode for given argument */
/* Returns the mode if it suitable for one of types, otherwise return NONE */
address_mode getArgMode(char arg[])
{
    address_mode res = NONE;     /* by default operand does not fit for any address mode */

    switch(arg[0])
    {
        case '#': res = IMMEDIATE; break;
        case '&': res = RELATIVE; break;
    }

    /* checks if argument start with a letter */
    if(isalpha(arg[0]))
    {
        if((strlen(arg) > 1) && (arg[0] == 'r') && (isdigit(arg[1])))
            res = REGISTER;
        else
            res = LINK;
    }

    return res;
} /* end of function getArgMode */


/* Parse rest of line and collects operands */
/* Return pointer to array of operands, if collect successfully, otherwise NULL */
char **getOperands(char *rest, int index, int start, int nline)
{
    boolean valid = TRUE;             /* by default operands are valid, until will not found error */
    char buffer[MAX_OP][MAX_LEN];          /* 2 is maximum permitted number of operands for operation */
    char **res;
    int i, j;
    int len = strlen(rest) - 1;       /* length rest of line, excluded '\n' character */

    /* empty loop for looking the end of first operand */
    for (i = start; i < len && !isspace(rest[i]) && (rest[i] != ','); i++);

    /* copies first operand separately */
    memmove(buffer[0], (rest + start), (i - start));
    /* put character '\0' to signify the end of a string */
    buffer[0][i - start] = '\0';
    buffer[1][0] = '\0';                /* in case if second operand does not exist */

    /* checks if it is not the end of rest line and looking for second operand */
    if(i != len)
    {
        /* empty loop for looking the first not white space character */
        for (; i < len && isspace(rest[i]); i++)
        ;

        /* checks if rest of line contains only white spaces, but required second operands */
        if((i == len) && (operations[index].num_of_operands == 2))
        {
            printError(FEW_OPERANDS, nline, operations[index].opname);
            valid = FALSE;
        }

        /* checks if it is a comma and looking for second operand */
        else if((i != len) && rest[i] == ',')
        {
            /* empty loop for looking the first character for second operand */
            for(i = i+1; i < len && isspace(rest[i]); i++)
            ;

            start = i;

            /* checks if it is the end of rest line after comma */
            if(i == len)
            {
                switch (operations[index].num_of_operands)
                {
                    case 1:
                        printError(INV_COMMA, nline, operations[index].opname);
                        valid = FALSE; break;
                    case 2:
                        printError(FEW_OPERANDS, nline, operations[index].opname);
                        valid = FALSE; break;
                }
            }

            /* looking for operand after comma */
            else
            {
                /* empty loop for looking the end of second operand */
                for(; i < len && !isspace(rest[i]) && (rest[i] != ','); i++);

                /* copies second operand separately */
                memmove(buffer[1], (rest + start), (i - start));
                /* put character '\0' to signify the end of a string */
                buffer[1][i - start] = '\0';
             }
        } /* end else if there is a comma */

        /* checks if there is a second operand without comma */
        else if((i != len) && rest[i] != ',')
        {
            switch (operations[index].num_of_operands)
            {
                case 1:
                    printError(MANY_OPERANDS, nline, operations[index].opname);
                    valid = FALSE; break;
                case 2:
                    printError(MIS_COMMA, nline, operations[index].opname);
                    valid = FALSE; break;
            }
        }
    } /* end if for looking second operand */

    /* checks if it is not the end of rest line and looking if there is forbidden operand */
    if(valid && (i != len))
    {
        /* empty loop for looking the first not white space character */
        for (; i < len && isspace(rest[i]); i++)
        ;

        /* checks if there are extra characters in the rest of line */
        if (i != len)
        {
            if (rest[i] == ',')
            {
                /* empty loop for looking the first character for extra operand */
                for (; i < len && isspace(rest[i]); i++);
            }

            /* checks if it is the end of rest line after comma */
            if (i == len)
            {
                printError(INV_COMMA, nline, operations[index].opname);
                valid = FALSE;
            }
            else
            {
                printError(MANY_OPERANDS, nline, operations[index].opname);
                valid = FALSE;
            }
        } /* end if rest of line contains extra characters */
    } /* end if for looking an extra operand */

    /* check if there is error in operands */
    if(!valid)
        return NULL;

    /* create array of pointers to operands */
    res = (char **)calloc(MAX_OP,sizeof(char *));

    /* copy operands from buffer */
    for(i = 0; i < MAX_OP; i++)
    {
        res[i] = (char *)calloc(MAX_LEN, sizeof(char));
        for(j = 0; j < strlen(buffer[i]); j++)
        {
            res[i][j] = buffer[i][j];
        }

        /* put character '\0' to signify the end of a string */
        if(j < MAX_LEN)
            res[i][j] = '\0';
    }

    return res;
} /* end of function getOperands */

int getDirectDigit(char *operand)
{
    int temp, num = 0;
    int i = 1;                   /* index of character, operand[0] is # */
    int sign = 1;

    /* checks if there is a sign */
    if((operand[i] == '-') || (operand[i] == '+'))
    {
        i = i + 1;
        sign = (operand[i] == '-') ? -1 : 1;
    }

    for(; i < strlen(operand); i++)
    {
        temp = operand[i] - '0';
        num = num*10 + temp;
    }

    return (sign*num);
}