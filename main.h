#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <vector>
#include <list>

#ifndef WIN32
typedef size_t DWORD;
typedef int16_t WORD;
typedef int8_t  BYTE;
typedef void*   PVOID;
#else
#include <Windows.h>
#endif

#include "memory.h"



#endif // MAIN_H
