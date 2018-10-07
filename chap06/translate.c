#include "translate.h"

static struct Tr_level_ outer = {NULL, NULL, NULL, NULL};

Tr_level Tr_outermost(void) { return &outer; }

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals)
{
    Tr_level level = checked_malloc(sizeof(*level));
    level->parent = parent;
    level->name = name;
    level->frame = F_newFrame(name, U_boolList(true, formals));
    level->formals = makeAccessList(level, formals);
    return level;
}

Tr_access Tr_allocLocal(Tr_level level, bool escape)
{
    Tr_access local = checked_malloc(sizeof(*local));
    local->level = level;
    local->access = F_allocLocal(level->frame, escape);
    return local;
}

Tr_accessList Tr_formals(Tr_level level) { return level->formals; }

Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail)
{
    Tr_accessList l = checked_malloc(sizeof(*l));
    l->head = head;
    l->tail = tail;
    return l;
}

Tr_accessList makeAccessList(Tr_level level, U_boolList formals)
{
    U_boolList fmls;
    Tr_accessList headList, tailList = NULL;
    for (fmls = formals; fmls; fmls = fmls->tail)
    {
        Tr_access access = Tr_allocLocal(level, fmls->head);
        if (headList)
        {
            tailList->tail = Tr_AccessList(access, NULL);
            tailList = tailList->tail;
        }
        else
        {
            headList = Tr_AccessList(access, NULL);
            tailList = headList;
        }
    }
    return headList;
}
