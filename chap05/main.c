#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "parse.h"
#include "prabsyn.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s filename\n", argv[0]);
        return (1);
    }
    A_exp root = parse(argv[1]);
    if (root != NULL)
    {
        fprintf(stdout, "print exp\n");
        pr_exp(stdout, root, 10);
        fprintf(stdout, "\n");
    }
    else
    {
        fprintf(stdout, "parse error\n");
    }
    return 0;
}