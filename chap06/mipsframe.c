#include "frame.h"

F_frame F_newFrame(Temp_label name, U_boolList formals)
{
    F_frame frame = checked_malloc(sizeof(*frame));
    frame->name = name;
    frame->formals = makeFormalAccessList(frame, formals);
    frame->local_count = 0;
    return frame;
}

Temp_label F_name(F_frame f) { return f->name; }

F_accessList F_formals(F_frame f) { return f->formals; }

F_access F_allocLocal(F_frame f, bool escape)
{
    f->local_count++;
    if (escape)
    {
        // todo
        return InFrame(0);
    }
    else
    {
        return InReg(Temp_newtemp());
    }
}

F_accessList makeFormalAccessList(F_frame f, U_boolList formals)
{
    U_boolList fmls;
    F_accessList headList, tailList = NULL;
    for (fmls = formals; fmls; fmls = fmls->tail)
    {
        /* Assume all variables escape for now */
        F_access access = F_allocLocal(f, TRUE);
        if (headList)
        {
            tailList->tail = F_AccessList(access, NULL);
            tailList = tailList->tail;
        }
        else
        {
            headList = F_AccessList(access, NULL);
            tailList = headList;
        }
    }
    return headList;
}

F_access InFrame(int offset)
{
    F_access access = checked_malloc(sizeof(*access));
    access->kind = inFrame;
    access->u.offset = offset;
    return access;
}

F_access InReg(Temp_temp reg)
{
    F_access access = checked_malloc(sizeof(*access));
    access->kind = inReg;
    access->u.reg = reg;
    return access;
}

F_accessList F_AccessList(F_access head, F_accessList tail)
{
    F_accessList list = checked_malloc(sizeof(*list));
    list->head = head;
    list->tail = tail;
    return list;
}
