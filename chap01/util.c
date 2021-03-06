#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

void *checked_malloc(int len)
{
    void *p = malloc(len);
    if (!p)
    {
        fprintf(stderr, "\nRun out of memory!\n");
        exit(1);
    }
    return p;
}
