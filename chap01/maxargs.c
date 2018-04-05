#include "maxargs.h"
#include <stdio.h>

int max(int a, int b)
{
    return a > b ? a : b;
}

int maxargsByExp(A_exp exp)
{
    if(exp->kind == A_idExp)
    {
        return 0;
    }
    else if(exp->kind == A_numExp)
    {
        return 0;
    }
    else if(exp->kind == A_opExp)
    {
        return max(maxargsByExp(exp->u.op.left), maxargsByExp(exp->u.op.right));
    }
    else if(exp->kind == A_eseqExp)
    {
        return max(maxargs(exp->u.eseq.stm), maxargsByExp(exp->u.eseq.exp));
    }
    else
    {
        printf("exp->kind error!");
    }
    return 0;
}

int maxargsByExpList(A_expList expList)
{
    if(expList->kind == A_pairExpList)
    {
        return max(maxargsByExp(expList->u.pair.head), maxargsByExpList(expList->u.pair.tail));
    }
    else if(expList->kind == A_lastExpList)
    {
        return maxargsByExp(expList->u.last);
    }
    else
    {
        printf("expList->kind error!");
    }
}

int getCountByExpList(A_expList expList)
{
    int count = 0;
    while(expList->kind == A_pairExpList)
    {
        count++;
        expList = expList->u.pair.tail;
    }
    if(expList->kind == A_lastExpList)
    {
        count++;
    }
    else
    {
        printf("expList->kind error!");
    }
    return count;
}


//tells the maximum number of arguments of any print statement within any subexpression of a given statement
int maxargs(A_stm stm)
{
    if(stm->kind == A_compoundStm)
    {
        return max(maxargs(stm->u.compound.stm1), maxargs(stm->u.compound.stm2));
    }
    else if(stm->kind == A_assignStm)
    {
        return maxargsByExp(stm->u.assign.exp);
    }
    else if(stm->kind == A_printStm)
    {
        return max(getCountByExpList(stm->u.print.exps), maxargsByExpList(stm->u.print.exps));
        //remember that print statements can contain expressions that contain other print statements.
    }
    else
    {
        printf("stm->kind error!");
    }
}
