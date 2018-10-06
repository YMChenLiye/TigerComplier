#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "parse.h"
#include "prabsyn.h"
#include "semantic.h"

void DoError(char* filename)
{
    fprintf(stdout, "Error!!! filename: %s \n", filename);
    FILE* ferr = fopen("errfile", "ab+");
    if (ferr)
    {
        fputs(filename, ferr);
        fputs("\n", ferr);
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s filename\n", argv[0]);
        return (1);
    }

    // build Abstract Syntax Tree
    A_exp root = parse(argv[1]);
    if (root == NULL)
    {
        fprintf(stdout, "parse error\n");
        DoError(argv[1]);
        return -1;
    }
    // pr_exp(stdout, root, 10);
    // fprintf(stdout, "pr_exp done\n");
    // Semantic Analysis
    SEM_transProg(root);
    if (anyErrors)
    {
        DoError(argv[1]);
        return -1;
    }

    // fprintf(stdout, "print exp\n");
    // pr_exp(stdout, root, 10);
    // fprintf(stdout, "\n");

    return 0;
}