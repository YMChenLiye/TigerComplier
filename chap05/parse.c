/*
 * parse.c - Parse source file.
 */

#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "parse.h"
#include "prabsyn.h"

extern int yyparse(void);
extern A_exp absyn_root;

/* parse source file fname;
   return abstract syntax data structure */
A_exp parse(string fname)
{
    EM_reset(fname);
    int iRet = yyparse();
    // fprintf(stdout, "yyparse, iRet = %d\n", iRet);
    if (iRet == 0)
        return absyn_root;
    else
        return absyn_root;
    return NULL;
}
