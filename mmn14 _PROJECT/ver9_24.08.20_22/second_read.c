/* */
#include "data.h"

extern boolean isGotError(char *file);
extern symbol *getFirstSymbol();
extern word *createAdditionalWord(int digit, access are, int nline);
extern getFirstCode();
extern getFirstData();
extern address_mode getArgMode(char *arg);

void makeOutputFiles(FILE *file, char *fname, int FIC, int FDC);
void getEntries(char *line, int nline, char *fname);
void getOperationCode(char *fname, int FIC, int FDC);
void getDataValue(char *fname);
void addToEntries(char *symbol, int address, char *fname);
void addToExternals(char *symbol, int address, char *fname);
void updateSymbolTable(int FIC);
int getSymbolAddress(char *symbol, int nline);
boolean isExternalSymbol(char *symbol, int nline);

void makeOutputFiles(FILE *file, char *fname, int FIC, int FDC)
{
    int lc = 1;                         /* line counter */
    char line[MAX_LEN + 2] = "";        /* plus 2 for characters \0 and \n */
    FILE *ofp;
    char temp_name[MAX_LEN];

    /* update addresses for symbols according instruction counter */
    updateSymbolTable(FIC);

    /* Read the file for looking label .entry */
    while(fgets(line, (MAX_LEN + 2), file) != NULL)
    {
        getEntries(line, lc, fname);
        lc++;
    }

    /* read code table */
    getOperationCode(fname, FIC, FDC);

    /* read data table */
    getDataValue(fname);

    /* Checks if an error was found in the file */
    if(isGotError(fname))
    {
        /* removing file .ob, .ent, .ext */
        strcpy(temp_name, fname);
        strcat(temp_name,".ob");
        remove(temp_name);

        strcpy(temp_name, fname);
        strcat(temp_name,".ent");
        remove(temp_name);

        strcpy(temp_name, fname);
        strcat(temp_name,".ext");
        remove(temp_name);
    }
}

/* Parses the line into parts and looking for label .entry */
void getEntries(char *line, int nline, char *fname)
{
    char *token,  *rest;
    char copyline[MAX_LEN];
    char delim[] = {'\t', '\r', '\n', '\v', '\f', ' '};           /* white spaces */
    int address;

    /* take a copy of line for analysis arguments further */
    strcpy(copyline, line);

    /* get the first token */
    token = strtok(line, delim);

    /* checks if token is empty or comment line */
    if(token == NULL || token[0] == ';')
        return;

    /* checks if token is symbol */
    if((token[strlen(token)-1] == ':'))            /* character ':' indicates the end of symbol definition */
        token = strtok(NULL, delim);               /* move pointer to next token */

    /* checks if token is label */
    if(token[0] == '.')                            /* character '.' indicates data label definitions */
    {
        /* checks if label is .entry */
        if((strcmp(token, ".entry") == 0))
        {
            token = strtok(NULL, delim);               /* move pointer to next token - to the symbol */
            address = getSymbolAddress(token, nline);

            /* check if symbol exist in symbol table */
            if(address < 0)
                addToEntries(token, address, fname);
        }
    } /* end if: label */

} /* end of function getEntries */


void getOperationCode(char *fname, int FIC, int FDC)
{
    FILE *ofp; 				/* output file */
    char object[FNAME_MAXLEN];
    code_ptr curr;
    address_mode temp_mode;
    word *temp;
    int i, address, jump;
    boolean external = FALSE;

    strcpy(object, fname);
    strcat(object,".ob");

    if((ofp = fopen(object, "a")) == NULL)
        fprintf(stderr, "\nCannot open file %s.\n\n", object);
    else
    {
        fprintf(ofp, "%7d %-7d\n", FIC, FDC);

        for(curr = getFirstCode(); curr != NULL; curr->next)
        {
            fprintf(ofp, "%07d %07x\n", curr->address, curr->text.binary);

            /* checks if one ore more additional word required */
            if(curr->L >= 2)
            {
                temp_mode = getArgMode(curr->operands[0]);
                if(temp_mode == IMMEDIATE)    /* additional word already filled in first read */
                {
                    curr = curr->next;
                    fprintf(ofp, "%07d %07x\n", curr->address, curr->text.binary);
                }
                else if((temp_mode == RELATIVE) || (temp_mode == LINK))
                {
                    address = getSymbolAddress(curr->operands[0], curr->nline);
                    if(address > 0)
                    {
                        external = isExternalSymbol;
                        if (external) {
                            switch (temp_mode) {
                                case LINK:
                                    temp = createAdditionalWord(address, E, curr->nline);
                                    fprintf(ofp, "%07d %07x\n", (curr->address + 1), temp->binary);
                                    addToExternals(curr->operands[0], (curr->address + 1), fname);
                                    break;
                                case RELATIVE:
                                    printError(INV_MODE, curr->nline, 1);
                                    break;
                            }
                        }
                            /* symbol is internal */
                        else {
                            switch (temp_mode) {
                                case LINK:
                                    temp = createAdditionalWord(address, R, curr->nline);
                                    fprintf(ofp, "%07d %07x\n", (curr->address + 1), temp->binary);
                                    break;
                                case RELATIVE:
                                    jump = address - curr->address;
                                    temp = createAdditionalWord(jump, A, curr->nline);
                                    fprintf(ofp, "%07d %07x\n", (curr->address + 1), temp->binary);
                                    break;
                            }
                        }
                    } /* end if symbol exist and have valid address */
                } /* end if mode RELATIVE or LINK */
            } /* end if required 2 or more additional words */


            /* checks if one ore more additional word required */
            if(curr->L == 3)
            {
                temp_mode = getArgMode(curr->operands[1]);
                if((temp_mode == RELATIVE) || (temp_mode == LINK))
                {
                    address = getSymbolAddress(curr->operands[1], curr->nline);
                    if(address > 0)
                    {
                        external = isExternalSymbol;
                        if (external)
                        {
                            switch (temp_mode)
                            {
                                case LINK:
                                    temp = createAdditionalWord(address, E, curr->nline);
                                    fprintf(ofp, "%07d %07x\n", (curr->address + 2), temp->binary);
                                    addToExternals(curr->operands[1], (curr->address + 2), fname);
                                    break;
                                case RELATIVE:
                                    printError(INV_MODE, curr->nline, 2);
                                    break;
                            }
                        }

                        /* symbol is internal */
                        else {
                            switch (temp_mode)
                            {
                                case LINK:
                                    temp = createAdditionalWord(address, R, curr->nline);
                                    fprintf(ofp, "%07d %07x\n", (curr->address + 2), temp->binary);
                                    break;
                                case RELATIVE:
                                    jump = address - curr->address;
                                    temp = createAdditionalWord(jump, A, curr->nline);
                                    fprintf(ofp, "%07d %07x\n", (curr->address + 2), temp->binary);
                                    break;
                            }
                        }
                    } /* end if symbol exist and have valid address */
                } /* end if mode RELATIVE or LINK */
            } /* end if required second additional words */

        } /* end for loop through code table */

        fclose(ofp);
    } /* end else file opened successfully */
} /* end function getOperationCode */

void getDataValue(char *fname)
{
    FILE *ofp;                /* output file */
    char object[FNAME_MAXLEN];
    data_ptr curr;

    strcpy(object, fname);
    strcat(object, ".ob");

    if ((ofp = fopen(object, "a")) == NULL)
        fprintf(stderr, "\nCannot open file %s.\n\n", object);
    else
     {
        for (curr = getFirstData(); curr != NULL; curr->next)
        {
            fprintf(ofp, "%07d %07x\n", curr->address, curr->text.binary);
        }

        fclose(ofp);
    } /* end else file opened successfully */
} /* end function getOperationCode */

/* Create/open output file, and write the entry symbol in it. */
void addToEntries(char *symbol, int address, char *fname)
{
    FILE *ofp; 				/* output file */
    char entries[FNAME_MAXLEN];

    strcpy(entries, fname);
    strcat(entries, ".ent");

    if((ofp = fopen(entries, "a")) == NULL)
        fprintf(stderr, "\nCannot open file %s.\n\n", entries);
    else
    {
        fprintf(ofp, "%s %07d\n", symbol, address);
        fclose(ofp);
    }
} /* end of function addToEntries */

/* Create/open output file, and write the external symbol in it. */
void addToExternals(char *symbol, int address, char *fname)
{
    FILE *ofp; 				/* output file */
    char externals[FNAME_MAXLEN];

    strcpy(externals, fname);
    strcat(externals, ".ext");

    if((ofp = fopen(externals, "a")) == NULL)
        fprintf(stderr, "\nCannot open file %s.\n\n", externals);
    else
    {
        fprintf(ofp, "%s %07d\n", symbol, address);
        fclose(ofp);
    }
} /* end of function addToExternals */


/* Updates address in symbol table for data members */
void updateSymbolTable(int FIC)
{
    symbol_ptr curr;

    curr = getFirstSymbol();

    while (curr != NULL)
    {
        if(curr->type == DATA)
            curr->address = curr->address + FIC;
        curr = curr->next;
    }
}

/* Checks if given symbol contains in a symbol table */
/* Returns address in memory for the symbol, if symbol was found, otherwise -1 */
int getSymbolAddress(char *symbol, int nline)
{
    int address, i = 0;
    symbol_ptr curr;

    if(symbol[0] == '&')        /* RELATIVE mode */
        i = i + 1;

    /* empty loop for looking symbol match */
    for(curr = getFirstSymbol(); (curr != NULL) && (strcmp((symbol+i), curr->sname) != 0); curr = curr->next)
        ;

    /* check if loop finished without founding match */
    if(curr == NULL)
    {
        printError(UNDEF_SYMBOL, nline, symbol);
        address = -1;
    }
    else
        address = curr->address;

    return address;
}

/* Checks if given symbol contains in a symbol table and it is an external */
/* Returns TRUE, if symbol is external, otherwise FALSE */
boolean isExternalSymbol(char *symbol, int nline)
{
    boolean isExternal = FALSE;         /* by default symbol is internal */
    int i = 0;
    symbol_ptr curr;

    if(symbol[0] == '&')        /* RELATIVE mode */
        i = i + 1;

    /* empty loop for looking symbol match */
    for(curr = getFirstSymbol(); (curr != NULL) && (strcmp((symbol+i), curr->sname) != 0); curr = curr->next)
        ;

    /* check if loop finished without founding match */
    if(curr == NULL)
        printError(UNDEF_SYMBOL, nline, symbol);

    else if(curr->type == EXTERN)
        isExternal = TRUE;

    return isExternal;
}