#include "semantic.h"
#include "stdlib.h"
#include "errormsg.h"
#include "absyn.h"
#include "env.h"

struct expty expTy(Tr_exp exp, Ty_ty ty)
{
    struct expty e;
    e.exp = exp;
    e.ty = ty;
    return e;
}

struct expty transVar(S_table venv, S_table tenv, A_var v)
{
    switch (v->kind)
    {
        case A_simpleVar:
        {
            E_enventry x = S_look(venv, v->u.simple);
            if (x && x->kind == E_varEntry)
            {
                return expTy(NULL, actual_ty(x->u.var.ty));
            }
            else
            {
                EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
                return expTy(NULL, Ty_Int());
            }
        }
        case A_fieldVar:
        {
        }
        case A_subscriptVar:
        {
        }
    }
}

struct expty transExp(S_table venv, S_table tenv, A_exp a)
{
    switch (a->kind)
    {
        case A_varExp:
        case A_nilExp:
        case A_intExp:
        {
            return expTy(NULL, Ty_Int());
        }
        case A_stringExp:
        {
            return expTy(NULL, Ty_String());
        }
        case A_callExp:
            break;
        case A_opExp:
        {
            A_oper oper = a->u.op.oper;
            struct expty left = transExp(venv, tenv, a->u.op.left);
            struct expty right = transExp(venv, tenv, a->u.op.right);
            if (oper == A_plusOp)
            {
                if (left.ty->kind != Ty_int)
                {
                    EM_error(a->u.op.left->pos, "integer required");
                }
                if (right.ty->kind != Ty_int)
                {
                    EM_error(a->u.op.right->pos, "integer required");
                }
                return expTy(NULL, Ty_Int());
            }

            break;
        }
        case A_recordExp:
        case A_seqExp:
        case A_assignExp:
        case A_ifExp:
        case A_whileExp:
        case A_forExp:
        case A_breakExp:
        case A_letExp:
        {
            struct expty exp;
            S_beginScope(venv);
            S_beginScope(tenv);
            A_decList d;
            for (d = a->u.let.decs; d != NULL; d = d->tail)
            {
                transDec(venv, tenv, d->head);
            }
            exp = transExp(venv, tenv, a->u.let.body);
            S_endScope(tenv);
            S_endScope(venv);
            return exp;
        }
        case A_arrayExp:
            break;

            // return expTy(NULL, Ty_Void());
    }
    assert(0); /* should have returned from some clause of the switch */
}

void transDec(S_table venv, S_table tenv, A_dec d)
{
    switch (d->kind)
    {
        case A_varDec:
        {
            struct expty e = transExp(venv, tenv, d->u.var.init);
            if (d->u.var.typ != NULL)
            {
                // check type
                Ty_ty ty = S_look(tenv, d->u.var.typ);
                if (ty != NULL)
                {
                    if (actual_ty(ty) != e.ty)
                    {
                        EM_error(d->pos, "type should be same!");
                        return;
                    }
                }
                else
                {
                    EM_error(d->pos, "no type:%s", S_name(d->u.var.typ));
                    return;
                }
            }
            S_enter(venv, d->u.var.var, E_VarEntry(e.ty));
            break;
        }
        case A_typeDec:
        {
            break;
        }
        case A_functionDec:
        {
            break;
        }
        default:
        {
        }
    }
}

Ty_ty transTy(S_table tenv, A_ty a) {}

Ty_ty actual_ty(Ty_ty type)
{
    if (type->kind == Ty_name)
    {
        return actual_ty(type->u.name.ty);
    }
    else
    {
        return type;
    }
}

void SEM_transProg(A_exp exp)
{
    S_table tenv = E_base_tenv();
    S_table venv = E_base_venv();

    transExp(venv, tenv, exp);
}