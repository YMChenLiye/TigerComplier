#pragma once
#include "temp.h"

typedef struct F_frame_ *F_frame;
typedef struct F_access_ *F_access;
typedef struct F_accessList_ *F_accessList;

struct F_frame_
{
    Temp_label name;
    F_accessList formals;
    int local_count;
};

struct F_access_
{
    enum
    {
        inFrame,
        inReg
    } kind;
    union {
        int offset;     // InFrame
        Temp_temp reg;  // InReg
    } u;
};

struct F_accessList_
{
    F_access head;
    F_accessList tail;
};

// public:
F_frame F_newFrame(Temp_label name, U_boolList formals);
Temp_label F_name(F_frame f);
F_accessList F_formals(F_frame f);
F_access F_allocLocal(F_frame f, bool escape);

F_access InFrame(int offset);
F_access InReg(Temp_temp reg);
F_accessList F_AccessList(F_access head, F_accessList tail);
F_accessList makeFormalAccessList(F_frame f, U_boolList formals);