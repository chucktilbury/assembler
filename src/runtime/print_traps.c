
#include "common.h"
#include "vmachine.h"

// print the value given in R0
bool print_trap()
{
    Value val = popVal();
    printVal(&val);

    return false;
}

// print the value given in R0 with the type
bool dbg_print_trap()
{
    Value val = popVal();
    printf("\n(%s)", valTypeToStr(val.type));
    printVal(&val);
    printf("\n");

    return false;
}
