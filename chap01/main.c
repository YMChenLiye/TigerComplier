#include "prog1.h"
#include "maxargs.h"
#include "interp.h"
#include <stdio.h>

int main()
{
    prog();

    printf("maxargs = %d\n", maxargs(prog()));

    printf("=============================\n");
    interp(prog());

    return 0;
}
