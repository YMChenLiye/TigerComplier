#include "prog1.h"
#include "maxargs.h"
#include "stdio.h"

int main()
{
    prog();

    printf("maxargs = %d\n", maxargs(prog()));

    return 0;
}
