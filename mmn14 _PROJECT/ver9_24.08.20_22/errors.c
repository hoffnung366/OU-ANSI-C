/*
 *
 */
#include <stdarg.h>  /* functions with arguments */
#include "data.h"

static char fname[FNAME_MAXLEN] = "";
static boolean error_flag;

void setCurrFile(char *file);
boolean isGotError(char *file);
void printError(error type, int nline, ...);

/* Sets a fname for a file that current in work */
void setCurrFile(char *file)
{
    /* Checks if fname didn't set for this file yet*/
    if(strcmp(fname,file))
    {
        strcpy(fname,file);
        error_flag = FALSE;
    }
}

/* Returns an indicator about errors in the file.
 * Return TRUE if there was an error in the file, FALSE if there wasn't errors
 * and NULL if the file is different from the current file
 */

boolean isGotError(char *file)
{
    /* Checks if the file is not a current file */
    if(strcmp(fname,file))
        fprintf(stderr, "Warning: Current file in work is %s, requested file for checking error is %s.\n",
                fname, file);

    return error_flag;
}

/* Prints errors to a standard error output.
 * Format is line #<line>:  <message>
 */
void printError(error type, int nline, ...)
{
    va_list arg_ptr;      /* points to each unnamed arg in turn */

    /* make arg_ptr point to 1st unnamed arg */
    va_start(arg_ptr, nline);

    /* Checks if it is the first error in the file */
    if (!error_flag) {
        fprintf(stderr, "\nFile %s. List of errors is below:\n", fname);
        error_flag = TRUE;
    }

    /* Prints an error to standard error output by given type */
    switch (type) {
        case LEN_LINE:
            fprintf(stderr, "Line #%d:  Line has forbidden length, should be <= %d.\n", nline, va_arg(arg_ptr, int));
            break;
        case UN_OPERATION:
            fprintf(stderr, "Line #%d:  Unrecognized operation: \"%s\".\n", nline, va_arg(arg_ptr, char *)); break;
        case INV_MODE:
            fprintf(stderr, "Line #%d:  Invalid addressing mode for operand #%d.\n", nline, va_arg(arg_ptr, int));
            break;
        case MANY_OPERANDS:
            fprintf(stderr, "Line #%d:  Too many operands for \"%s\" operation.\n", nline, va_arg(arg_ptr, char *));
            break;
        case FEW_OPERANDS:
            fprintf(stderr, "Line #%d:  Not enough operands for \"%s\" operation.\n", nline, va_arg(arg_ptr, char *));
            break;
        case INV_REGISTER:
            fprintf(stderr, "Line #%d:  Register with name \"%s\" doesn’t exist.\n", nline, va_arg(arg_ptr, char *));
            break;
        case INV_STRING:
            fprintf(stderr, "Line #%d:  String have to be defined between quotation marks.\n", nline); break;
        case UN_COMMA:
            fprintf(stderr, "Line #%d:  Unexpected comma after instruction \"%s\".\n", nline, va_arg(arg_ptr, char *));
            break;
        case SEV_COMMAS:
            fprintf(stderr, "Line #%d:  Several consecutive commas.\n", nline); break;
        case MIS_DATA:
            fprintf(stderr, "Line #%d:  Missing data after \"%s\".\n", nline, va_arg(arg_ptr, char *)); break;
        case INV_OPERAND:
            fprintf(stderr, "Line #%d:  An operand type is not suitable for \"%s\" operation.\n",
                    nline, va_arg(arg_ptr, char *)); break;
        case W_ARG:
            fprintf(stderr, "Line #%d:  Unrecognized argument for word: \"%s\".\n", nline, va_arg(arg_ptr, char *));
            break;
        case INV_CHAR:
            fprintf(stderr, "Line #%d:  Symbol \"%s\" contains not valid characters.\n", nline, va_arg(arg_ptr, char *));
            break;
        case LEN_SYMBOL:
            fprintf(stderr, "Line #%d:  Symbol has forbidden length, should be <= %d.\n", nline, va_arg(arg_ptr, int));
            break;
        case UN_LABEL:
            fprintf(stderr, "Line #%d:  Unrecognized data label: \"%s\".\n", nline, va_arg(arg_ptr, char *)); break;
        case  DEF_SYMBOL:
            fprintf(stderr, "Line #%d:  Symbol \"%s\" already defined earlier.\n", nline, va_arg(arg_ptr, char *)); break;
        case KEY_SYMBOL:
            fprintf(stderr, "Line #%d:  Invalid symbol, \"%s\" is a keyword.\n", nline, va_arg(arg_ptr, char *)); break;
        case IN_OPERATION:
            fprintf(stderr, "Line #%d:  Invalid operation name: \"%s\" (must be in lower case).\n",
                    nline, va_arg(arg_ptr, char *)); break;
        case MIS_COMMA:
            fprintf(stderr, "Line #%d:  Missing comma after \"%s\".\n", nline, va_arg(arg_ptr, char *)); break;
        case INV_ARG:
            fprintf(stderr, "Line #%d:  An argument type is not suitable for .data instruction, expected integer.\n",
                    nline); break;
         case INV_COMMA:
            fprintf(stderr, "Line #%d:  Unexpected comma in the end of instruction \"%s\".\n",
                    nline, va_arg(arg_ptr, char *)); break;
        case NONE_MODE:
            fprintf(stderr, "Line #%d:  The operand does not fit for any address mode for operation \"%s\".\n",
                    nline, va_arg(arg_ptr, char *)); break;
        case UNDEF_SYMBOL:
            fprintf(stderr, "Line #%d:  Symbol \"%s\" is NOT defined.\n", nline, va_arg(arg_ptr, char *)); break;
         default:
            fprintf(stderr, "Line #%d:  Error type is not defined.\n", nline);
    }

    va_end(arg_ptr);  /* clean up when done */
}