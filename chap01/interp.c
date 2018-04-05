#include "interp.h"
#include <string.h>
#include <stdio.h>

//新表总是加在旧表的前面
Table_ Table(string id, int value, Table_ tail)
{
    Table_ t = checked_malloc(sizeof(*t));
    t->id = id;
    t->value = value;
    t->tail = tail;
    //printf("table create\n");
    return t;
}

int lookup(Table_ t, string key)
{
    if(t == NULL || key == NULL || t->id == NULL)
    {
        return 0;
    }
    else
    {
        //printf("len1 = %d, len2 = %d\n", strlen(t->id), strlen(key));
        if(t->id != NULL && strcmp(t->id, key) == 0)
        {
            return t->value;
        }
        else
        {
            return lookup(t->tail, key);
        }
    }
}

Table_ interp(A_stm stm)
{
    return interpStm(stm, NULL);
}

Table_ interpStm(A_stm s, Table_ t)
{
    if(s->kind == A_compoundStm)
    {
        Table_ result = interpStm(s->u.compound.stm1, t);
        return interpStm(s->u.compound.stm2, result);
    }
    else if(s->kind == A_assignStm)
    {
        struct IntAndTable expResult = interpExp(s->u.assign.exp, t);
        Table_ result = Table(s->u.assign.id, expResult.i, expResult.t);
        return result;
    }
    else if(s->kind == A_printStm)
    {
        return printExpList(s->u.print.exps, t);
    }
    else
    {
        assert(!"s->kind error");
    }
}

struct IntAndTable interpExp(A_exp e, Table_ t)
{

    struct IntAndTable result;
    if(e->kind == A_idExp)
    {
        result.i = lookup(t, e->u.id);
        result.t = t;
        return result;
    }
    else if(e->kind == A_numExp)
    {
        result.i = e->u.num;
        result.t = t;
        return result;
    }
    else if(e->kind == A_opExp)
    {
        struct IntAndTable resultLeft;
        struct IntAndTable resultRight;
        resultLeft = interpExp(e->u.op.left, t);
        resultRight = interpExp(e->u.op.right, resultLeft.t);
        switch(e->u.op.oper)
        {
            case A_plus:
                result.i = resultLeft.i + resultRight.i;
                break;
            case A_minus:
                result.i = resultLeft.i - resultRight.i;
                break;
            case A_times:
                result.i = resultLeft.i * resultRight.i;
                break;
            case A_div:
                result.i = resultLeft.i / resultRight.i;
                break;
            default:
                assert(!"e->u.oper error");
                break;
        }
        result.t = resultRight.t;
        return result;
    }
    else if(e->kind == A_eseqExp)
    {
        Table_ stmResult = interpStm(e->u.eseq.stm, t);
        return interpExp(e->u.eseq.exp, stmResult);
    }
    else
    {
        assert(!"e->kind error");
    }
}

Table_ printExpList(A_expList expList, Table_ t)
{
    if(expList->kind == A_pairExpList)
    {
        struct IntAndTable resultExp = interpExp(expList->u.pair.head, t);
        printf("%d ", resultExp.i);
        Table_ result = printExpList(expList->u.pair.tail, resultExp.t);
        //printf("\n");
        return result;
    }
    else if(expList->kind == A_lastExpList)
    {
        struct IntAndTable resultExp = interpExp(expList->u.last, t);
        printf("%d ", resultExp.i);
        //换行符一定是表达式列中最后一项打印的
        printf("\n");
        return resultExp.t;
    }
    else
    {
        assert(!"expList->kind error");
    }
}

