#include "semantic.h"
#include "stdlib.h"
#include "errormsg.h"
#include "absyn.h"
#include "env.h"
#include <stdio.h>
#include "prabsyn.h"

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
                return expTy(NULL, Ty_Void());
            }
        }
        case A_fieldVar:
        {
            struct expty expRecord = transVar(venv, tenv, v->u.field.var);
            if (expRecord.ty->kind != Ty_record)
            {
                EM_error(v->pos, "not Ty_record");
            }
            else
            {
                Ty_fieldList fieldList = expRecord.ty->u.record;
                for (; fieldList != NULL; fieldList = fieldList->tail)
                {
                    if (fieldList->head->name == v->u.field.sym)
                    {
                        return expTy(NULL, actual_ty(fieldList->head->ty));
                    }
                }
                EM_error(v->pos, "no filed name %s", S_name(v->u.field.sym));
            }
            return expTy(NULL, Ty_Int());
        }
        case A_subscriptVar:
        {
            struct expty expArray = transVar(venv, tenv, v->u.subscript.var);
            if (expArray.ty->kind != Ty_array)
            {
                EM_error(v->pos, "not Ty_array");
            }
            else
            {
                struct expty expIndex =
                    transExp(venv, tenv, v->u.subscript.exp);
                if (expIndex.ty->kind != Ty_int)
                {
                    EM_error(v->pos, "integer need");
                }
                else
                {
                    return expTy(NULL, actual_ty(expArray.ty->u.array));
                }
            }
            return expTy(NULL, Ty_Int());
        }
    }
    assert(0);
}

struct expty transExp(S_table venv, S_table tenv, A_exp a)
{
    switch (a->kind)
    {
        case A_varExp:
        {
            return transVar(venv, tenv, a->u.var);
        }
        case A_nilExp:
        {
            return expTy(NULL, Ty_Nil());
        }
        case A_intExp:
        {
            return expTy(NULL, Ty_Int());
        }
        case A_stringExp:
        {
            return expTy(NULL, Ty_String());
        }
        case A_callExp:
        {
            return transExp_callExp(venv, tenv, a);
        }
        case A_opExp:
        {
            return transExp_opExp(venv, tenv, a);
        }
        case A_recordExp:
        {
            return transExp_recordExp(venv, tenv, a);
        }
        case A_seqExp:
        {
            return transExp_seqExp(venv, tenv, a);
        }
        case A_assignExp:
        {
            return transExp_assignExp(venv, tenv, a);
        }
        case A_ifExp:
        {
            return transExp_ifExp(venv, tenv, a);
        }
        case A_whileExp:
        {
            return transExp_whileExp(venv, tenv, a);
        }
        case A_forExp:
        {
            return transExp_forExp(venv, tenv, a);
        }
        case A_breakExp:
        {
            return transExp_breakExp(venv, tenv, a);
        }
        case A_letExp:
        {
            return transExp_letExp(venv, tenv, a);
        }
        case A_arrayExp:
        {
            return transExp_arrayExp(venv, tenv, a);
        }
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
            // 是否指定了类型
            if (d->u.var.typ != NULL)
            {
                // check type
                Ty_ty ty = S_look(tenv, d->u.var.typ);
                if (ty != NULL)
                {
                    if (e.ty->kind != Ty_nil && actual_ty(ty) != e.ty)
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
                S_enter(venv, d->u.var.var, E_VarEntry(ty));
            }
            else
            {
                // 没有指定类型
                // 不允许nil
                if (e.ty->kind == Ty_nil)
                {
                    EM_error(d->pos, "not specify type, cant be nil");
                    return;
                }
                S_enter(venv, d->u.var.var, E_VarEntry(e.ty));
            }
            return;
        }
        case A_typeDec:
        {
            A_nametyList types = d->u.type;
            while (types)
            {
                S_enter(tenv, types->head->name,
                        transTy(tenv, types->head->ty));
                types = types->tail;
            }
            return;
        }
        case A_functionDec:
        {
            A_fundecList funList = d->u.function;
            while (funList)
            {
                A_fundec f = funList->head;
                Ty_ty resultTy = Ty_Void();
                if (f->result)
                {
                    resultTy = S_look(tenv, f->result);
                    if (!resultTy)
                    {
                        EM_error(f->pos, "undefined result type");
                        return;
                    }
                }
                Ty_tyList formalTys = makeFormalTyList(tenv, f->params);
                S_enter(venv, f->name, E_FunEntry(formalTys, resultTy));
                S_beginScope(venv);
                {
                    A_fieldList l;
                    Ty_tyList t;
                    for (l = f->params, t = formalTys; l;
                         l = l->tail, t = t->tail)
                    {
                        S_enter(venv, l->head->name, E_VarEntry(t->head));
                    }
                }
                struct expty body = transExp(venv, tenv, f->body);
                if (!is_equal_ty(resultTy, body.ty))
                {
                    EM_error(f->pos, "function result type error");
                }
                S_endScope(venv);

                funList = funList->tail;
            }
            return;
        }
    }
    assert(0);
}

Ty_ty transTy(S_table tenv, A_ty a)
{
    switch (a->kind)
    {
        case A_nameTy:
        {
            Ty_ty ty = S_look(tenv, a->u.name);
            if (ty != NULL)
            {
                return ty;
            }
            else
            {
                EM_error(a->pos, "undefined type %s", S_name(a->u.name));
                return Ty_Void();
            }
            break;
        }
        case A_recordTy:
        {
            Ty_fieldList fieldTys = makeFieldTys(tenv, a->u.record);
            return Ty_Record(fieldTys);
        }
        case A_arrayTy:
        {
            Ty_ty ty = S_look(tenv, a->u.array);
            if (ty != NULL)
            {
                return Ty_Array(ty);
            }
            else
            {
                EM_error(a->pos, "undefined type %s", S_name(a->u.array));
            }
            break;
        }
    }
    assert(0);
}

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

Ty_tyList makeFormalTyList(S_table tenv, A_fieldList params)
{
    Ty_tyList resultTys = NULL;
    Ty_tyList curResult = resultTys;

    A_fieldList curParam = params;
    for (; curParam != NULL; curParam = curParam->tail)
    {
        Ty_ty t = S_look(tenv, curParam->head->typ);
        if (t == NULL)
        {
            EM_error(curParam->head->pos, "undefined type %s",
                     S_name(curParam->head->typ));
        }
        else
        {
            if (curResult != NULL)
            {
                curResult->tail = Ty_TyList(t, NULL);
                curResult = curResult->tail;
            }
            else
            {
                resultTys = Ty_TyList(t, NULL);
                curResult = resultTys;
            }
        }
    }
    return resultTys;
}

Ty_fieldList makeFieldTys(S_table tenv, A_fieldList fields)
{
    Ty_fieldList resultFields = NULL;
    Ty_fieldList curResult = resultFields;

    A_fieldList curField = fields;
    for (; curField != NULL; curField = curField->tail)
    {
        Ty_ty t = S_look(tenv, curField->head->typ);
        if (t == NULL)
        {
            EM_error(curField->head->pos, "undefined type %s",
                     S_name(curField->head->typ));
        }
        else
        {
            Ty_field f = Ty_Field(curField->head->name, t);
            if (curResult != NULL)
            {
                curResult->tail = Ty_FieldList(f, NULL);
                curResult = curResult->tail;
            }
            else
            {
                resultFields = Ty_FieldList(f, NULL);
                curResult = resultFields;
            }
        }
    }
    return resultFields;
}

bool is_equal_ty(Ty_ty leftType, Ty_ty rightType)
{
    leftType = actual_ty(leftType);
    rightType = actual_ty(rightType);
    if (leftType->kind == Ty_record || leftType->kind == Ty_array)
    {
        if (leftType->kind == Ty_record && rightType->kind == Ty_nil)
        {
            return true;
        }
        else
        {
            return leftType == rightType;
        }
    }
    else
    {
        return leftType->kind == rightType->kind;
    }
}

void SEM_transProg(A_exp exp)
{
    S_table tenv = E_base_tenv();
    S_table venv = E_base_venv();

    transExp(venv, tenv, exp);
}

struct expty transExp_callExp(S_table venv, S_table tenv, A_exp a)
{
    E_enventry x = S_look(venv, a->u.call.func);
    if (x && x->kind == E_funEntry)
    {
        // check params' type
        A_expList args = a->u.call.args;
        Ty_tyList argsType = x->u.fun.formals;
        for (; args && argsType; args = args->tail, argsType = argsType->tail)
        {
            struct expty argsTy = transExp(venv, tenv, args->head);
            if (!is_equal_ty(argsType->head, argsTy.ty))
            {
                EM_error(a->pos, "args type not match");
                return expTy(NULL, Ty_Void());
            }
        }
        if (args != NULL || argsType != NULL)
        {
            EM_error(a->pos, "args num not match");
            return expTy(NULL, Ty_Void());
        }

        // check result
        return expTy(NULL, actual_ty(x->u.fun.result));
    }
    else
    {
        EM_error(a->pos, "not find function define");
        return expTy(NULL, Ty_Void());
    }
}

struct expty transExp_opExp(S_table venv, S_table tenv, A_exp a)
{
    A_oper oper = a->u.op.oper;
    struct expty left = transExp(venv, tenv, a->u.op.left);
    struct expty right = transExp(venv, tenv, a->u.op.right);
    // 如果是record类型，还可以和nil比较
    // todo
    /*
        A_plusOp,
    A_minusOp,
    A_timesOp,
    A_divideOp,
    A_eqOp,
    A_neqOp,
    A_ltOp,
    A_leOp,
    A_gtOp,
    A_geOp
    */
    // 暂时只支持int类型,后续加上string类型
    if (oper >= A_plusOp && oper <= A_geOp)
    {
        if (left.ty->kind == Ty_record && right.ty->kind == Ty_nil)
        {
            return expTy(NULL, Ty_Int());
        }
        if (left.ty->kind != right.ty->kind)
        {
            EM_error(a->pos, "type not equal, left = %d, right = %d",
                     left.ty->kind, right.ty->kind);
            return expTy(NULL, Ty_Void());
        }

        if (left.ty->kind != Ty_int && left.ty->kind != Ty_string)
        {
            EM_error(a->pos, "only support int / string, left = %d, right = %d",
                     left.ty->kind, right.ty->kind);
            return expTy(NULL, Ty_Void());
        }

        /*
        if (left.ty->kind != Ty_int)
        {
            EM_error(a->u.op.left->pos, "integer required");
        }
        if (right.ty->kind != Ty_int)
        {
            EM_error(a->u.op.right->pos, "integer required");
        }
        */
        return expTy(NULL, Ty_Int());
    }
    else
    {
        EM_error(a->u.op.left->pos, "cant know oper type %d", oper);
        assert(0);
        return expTy(NULL, Ty_Int());
    }
}

struct expty transExp_recordExp(S_table venv, S_table tenv, A_exp a)
{
    // todo
    Ty_ty typ = S_look(tenv, a->u.record.typ);
    if (!typ)
    {
        EM_error(a->pos, "undefined type");
        return expTy(NULL, Ty_Record(NULL));
    }
    if (typ->kind != Ty_record)
    {
        EM_error(a->pos, "%s is not a record type", S_name(a->u.record.typ));
    }
    return expTy(NULL, typ);
}

struct expty transExp_seqExp(S_table venv, S_table tenv, A_exp a)
{
    struct expty result = expTy(NULL, Ty_Void());
    A_expList list = a->u.seq;
    while (list != NULL)
    {
        result = transExp(venv, tenv, list->head);
        list = list->tail;
    }
    return result;
}

struct expty transExp_assignExp(S_table venv, S_table tenv, A_exp a)
{
    struct expty var = transVar(venv, tenv, a->u.assign.var);
    struct expty exp = transExp(venv, tenv, a->u.assign.exp);
    if (!is_equal_ty(var.ty, exp.ty))
    {
        EM_error(a->u.assign.var->pos, "type not equal");
        return expTy(NULL, Ty_Void());
    }
    return expTy(NULL, Ty_Void());
}

struct expty transExp_ifExp(S_table venv, S_table tenv, A_exp a)
{
    struct expty test = transExp(venv, tenv, a->u.iff.test);
    struct expty then = transExp(venv, tenv, a->u.iff.then);

    if (test.ty->kind != Ty_int)
    {
        EM_error(a->u.iff.test->pos, "integer required");
    }

    if (a->u.iff.elsee)
    {
        struct expty elsee = transExp(venv, tenv, a->u.iff.elsee);
        if (!is_equal_ty(then.ty, elsee.ty))
        {
            EM_error(a->u.iff.elsee->pos,
                     "if-then-else branches must return same type");
        }
    }
    return expTy(NULL, then.ty);
}

struct expty transExp_whileExp(S_table venv, S_table tenv, A_exp a)
{
    struct expty test = transExp(venv, tenv, a->u.whilee.test);
    struct expty body = transExp(venv, tenv, a->u.whilee.body);
    if (test.ty->kind != Ty_int)
    {
        EM_error(a->u.whilee.test->pos, "integer required");
    }
    return expTy(NULL, Ty_Void());
}

struct expty transExp_forExp(S_table venv, S_table tenv, A_exp a)
{
    // todo
    S_beginScope(venv);
    S_enter(venv, a->u.forr.var, E_VarEntry(Ty_Int()));

    struct expty lo = transExp(venv, tenv, a->u.forr.lo);
    if (lo.ty->kind != Ty_int)
    {
        EM_error(a->u.forr.lo->pos, "integer required");
    }
    struct expty hi = transExp(venv, tenv, a->u.forr.hi);
    if (hi.ty->kind != Ty_int)
    {
        EM_error(a->u.forr.hi->pos, "integer required");
    }
    struct expty body = transExp(venv, tenv, a->u.forr.body);
    S_endScope(venv);
    return expTy(NULL, Ty_Void());
}

struct expty transExp_breakExp(S_table venv, S_table tenv, A_exp a)
{
    return expTy(NULL, Ty_Void());
}

struct expty transExp_letExp(S_table venv, S_table tenv, A_exp a)
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

struct expty transExp_arrayExp(S_table venv, S_table tenv, A_exp a)
{
    Ty_ty typ = S_look(tenv, a->u.array.typ);
    if (!typ)
    {
        EM_error(a->pos, "undefined type");
        return expTy(NULL, Ty_Int());
    }

    struct expty size = transExp(venv, tenv, a->u.array.size);
    struct expty init = transExp(venv, tenv, a->u.array.init);
    if (size.ty->kind != Ty_int)
    {
        EM_error(a->u.array.size->pos, "integer required");
        return expTy(NULL, Ty_Void());
    }

    if (!is_equal_ty(typ->u.array, init.ty))
    {
        EM_error(a->u.array.init->pos,
                 "initialisation expression should be same type");
        return expTy(NULL, Ty_Void());
    }

    return expTy(NULL, typ);
}