#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <vector>
#include <list>
//TODO: change it to custom map container
#include <map>

#ifndef WIN32
typedef size_t DWORD;
typedef int16_t WORD;
typedef int8_t  BYTE;
typedef void*   PVOID;
#else
#include <Windows.h>
#endif

namespace BuildSanOne {
enum eOpcTable{
    MOV_REG_DWORD = 0x71,
    MOV_REG_PDWORD,
    MOV_DWORD_REG,
    MOV_REG_REG,
    PUSH,
    POP,
    JMP,
    CALL,
    ADD_REG_REG,
    ADD_REG_DWORD,
    CMP
};
}

#include "memory.h"
#include "stack.h"
#include "opc_processor.h"
#include "interpretator.h"


#endif // MAIN_H
