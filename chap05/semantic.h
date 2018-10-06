#pragma once
#include "absyn.h"
#include "types.h"

typedef void* Tr_exp;

struct expty
{
    Tr_exp exp;
    Ty_ty ty;
};

struct expty expTy(Tr_exp exp, Ty_ty ty);

struct expty transVar(S_table venv, S_table tenv, A_var v);
struct expty transExp(S_table venv, S_table tenv, A_exp a);
void transDec(S_table venv, S_table tenv, A_dec d);
Ty_ty transTy(S_table tenv, A_ty a);

Ty_ty actual_ty(Ty_ty type);
Ty_tyList makeFormalTyList(S_table tenv, A_fieldList params);
Ty_fieldList makeFieldTys(S_table tenv, A_fieldList fields);
bool is_equal_ty(Ty_ty leftType, Ty_ty rightType);
bool is_Type_Cycle(S_table tenv, Ty_ty type);

void SEM_transProg(A_exp exp);

struct expty transExp_callExp(S_table venv, S_table tenv, A_exp a);
struct expty transExp_opExp(S_table venv, S_table tenv, A_exp a);
struct expty transExp_recordExp(S_table venv, S_table tenv, A_exp a);
struct expty transExp_seqExp(S_table venv, S_table tenv, A_exp a);
struct expty transExp_assignExp(S_table venv, S_table tenv, A_exp a);
struct expty transExp_ifExp(S_table venv, S_table tenv, A_exp a);
struct expty transExp_whileExp(S_table venv, S_table tenv, A_exp a);
struct expty transExp_forExp(S_table venv, S_table tenv, A_exp a);
struct expty transExp_breakExp(S_table venv, S_table tenv, A_exp a);
struct expty transExp_letExp(S_table venv, S_table tenv, A_exp a);
struct expty transExp_arrayExp(S_table venv, S_table tenv, A_exp a);