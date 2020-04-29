#ifndef MAIN_H
#define MAIN_H

#define DEBUG

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
typedef BYTE*   PBYTE;
#else
#include <Windows.h>
#endif

namespace BuildSanOne {




}

#include "memory.h"
#include "stack.h"
#include "opc_processor.h"
#include "interpretator.h"
#include "vmcode_compiler.h"

#endif // MAIN_H
