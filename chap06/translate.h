#pragma once
#include "temp.h"
#include "frame.h"
#include "util.h"

typedef struct Tr_level_ *Tr_level;
typedef struct Tr_access_ *Tr_access;
typedef struct Tr_accessList_ *Tr_accessList;

struct Tr_level_
{
    Tr_level parent;
    Temp_label name;
    F_frame frame;
    Tr_accessList formals;
};

struct Tr_access_
{
    Tr_level level;
    F_access access;
};

struct Tr_accessList_
{
    Tr_access head;
    Tr_accessList tail;
};

Tr_level Tr_outermost(void);
Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals);
Tr_access Tr_allocLocal(Tr_level level, bool escape);
Tr_accessList Tr_formals(Tr_level level);
Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail);

Tr_accessList makeAccessList(Tr_level level, U_boolList formals);
