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
            return;
        }
        case A_typeDec:
        {
            A_nametyList types = d->u.type;
            while (types->head)
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
            A_fundec f = funList->head;
            while (f)
            {
                Ty_ty resultTy = S_look(tenv, f->result);
                if(!resultTy)
                {
                    EM_error(f->pos, "undefined result type");
                    return;
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
                transExp(venv, tenv, f->body);
                S_endScope(venv);

                funList = funList->tail;
                f = funList->head;
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
            }
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
                return expTy(NULL, Ty_Int());
            }
        }
        if (args != NULL || argsType != NULL)
        {
            EM_error(a->pos, "args num not match");
        }
        else
        {
            return expTy(NULL, actual_ty(x->u.fun.result));
        }
    }
    return expTy(NULL, Ty_Int());
}

struct expty transExp_opExp(S_table venv, S_table tenv, A_exp a)
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
}

struct expty transExp_recordExp(S_table venv, S_table tenv, A_exp a)
{
    // todo
    return expTy(NULL, Ty_Int());
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
    return expTy(NULL, Ty_Void());
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

    if (!is_equal_ty(typ, init.ty))
    {
        EM_error(a->u.array.init->pos,
                 "initialisation expression should be same type");
        return expTy(NULL, Ty_Void());
    }

    return expTy(NULL, Ty_Void());
}