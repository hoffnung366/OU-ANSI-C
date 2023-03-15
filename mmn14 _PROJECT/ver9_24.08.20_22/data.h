#include <stdio.h>      /* fopen, fprintf, ... */
#include <stdlib.h>     /* malloc, exit, NULL, ... */
#include <string.h>     /* strcmp, strcpy, ... */
#include <ctype.h>      /* isspace, isaplpha, ... */

#if !defined(FMCELL)
    #define FMCELL 100    /* first memory cell */
#endif

#if !defined(MAX_LEN)
    #define MAX_LEN 80    /* valid command length is 80 characters */
#endif

#if !defined(FNAME_MAXLEN)
#define FNAME_MAXLEN 100    /* valid file name length is 100 characters */
#endif

#if !defined(SMAXLEN)
    #define SMAXLEN 31    /* permitted length for symbol */
#endif

#if !defined(MAX_MODE)
#define MAX_MODE 3        /* permitted number of mode for operation */
#endif

#if !defined(MAX_OP)
#define MAX_OP 2        /* maximum number of operands for operation */
#endif

typedef enum {FALSE, TRUE} boolean;
enum symbol_type {CODE, DATA, EXTERN};
enum operand_type {SRC, DEST};
/* address_mode in binary: 0 is 00, 1 is 01, 2 is 10, 3 is 11 */
typedef enum {IMMEDIATE, LINK, RELATIVE, REGISTER, NONE} address_mode;
/* access: Absolute, Relocatable, External; in binary: 4 is 100, 2 is 010, 1 is 001 */
typedef enum {A = 4, R = 2, E = 1} access;
/* type of errors */
typedef enum
{   LEN_LINE = 1,             /* Line has forbidden length, should be <= 80. */
    UN_OPERATION = 2,         /* Unrecognized operation: %s */
    INV_MODE = 3,             /* Invalid addressing mode for operand #%d. */
    MANY_OPERANDS = 4,        /* Too many operands for \"%s\" operation, should be %d. */
    FEW_OPERANDS = 5,         /* Not enough operands for \"%s\" operation, should be %d. */
    INV_REGISTER = 6,         /* Register with name \"%s\" doesn’t exist. */
    INV_STRING = 7,           /* String have to be defined between quotation marks. */
    UN_COMMA = 8,             /* Unexpected comma after instruction \"%s\". */
    SEV_COMMAS = 9,           /* Several consecutive commas. */
    MIS_DATA = 10,            /* Missing data after %s */
    INV_OPERAND = 11,         /* An operand type is not suitable for \"&s\" operation. */
    W_ARG = 12,               /* Unrecognized argument for word: "%s". */
    INV_CHAR = 13,            /* Symbol "%s" contains not valid characters. */
    LEN_SYMBOL = 14,          /* Symbol has forbidden length, should be <= %d. */
    UN_LABEL = 15,            /* Unrecognized data label: %s */
    DEF_SYMBOL = 16,          /* Symbol %s already defined earlier. */
    KEY_SYMBOL = 17,          /* Invalid symbol, "%s" is a keyword. */
    IN_OPERATION = 18,        /* Invalid operation name: \"%s\" (must be in lower case). */
    MIS_COMMA = 19,           /* Missing comma in %s operation */
    INV_ARG = 20,             /* The argument type is not suitable for .data instruction, expected integer. */
    INV_COMMA = 21,           /* Unexpected comma in the end of instruction \"%s\". */
    NONE_MODE = 22,            /* The operand does not fit for any address mode */
    UNDEF_SYMBOL = 23          /* Symbol \"%s\" is NOT defined. */

} error;

typedef struct
{
    long int binary: 24;
} word;


typedef struct dnode *data_ptr;
typedef struct dnode {
    int address;
    word text;
    data_ptr next;
} data;


typedef struct cnode *code_ptr;
typedef struct cnode {
    int address;
    word text;
    int L;
    char **operands;
    int nline;
    code_ptr next;
} code;

typedef struct snode *symbol_ptr;
typedef struct snode {
    char *sname;
    int address;
    enum symbol_type type;
    symbol_ptr next;
} symbol;


void printError(error type, int nline, ...);
