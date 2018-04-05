#pragma once

#include "util.h"
#include "slp.h"

typedef struct table * Table_;
struct table
{
    string id;
    int value;
    Table_ tail;
};

struct IntAndTable
{
    int i;
    Table_ t;
};

Table_ Table(string id, int value, Table_ tail);
int lookup(Table_ t, string key);

Table_ interp(A_stm stm);
Table_ interpStm(A_stm s, Table_ t);
struct IntAndTable interpExp(A_exp e, Table_ t);
Table_ printExpList(A_expList expList, Table_ t);
