#ifndef MAIN_H
#define MAIN_H

#define DEBUG

#define SANDRA_MINOR_VER 0
#define SANDRA_MAJOR_VER 1
#define SANDRA_SIGNATURE "\x10\x10\x2f\x5asandra\xfc\xf0\x03\x8d\xf5\xc0"

#include <cstring>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ostream>
#include <vector>
#include <list>
//TODO: change it to custom map container
#include <map>

#ifndef WIN32
typedef size_t DWORD, *PDWORD;
typedef int16_t WORD;
typedef int8_t  BYTE, *PBYTE;
typedef void*   PVOID;
#else
#include <Windows.h>
#endif

std::vector<std::string> split(std::string in, char separator);

#include "file.h"

#include "memory.h"
#include "stack.h"
#include "opc_processor.h"
#include "interpretator.h"
#include "pe.h"
#include "vmcode_compiler.h"
#include "command_processor.h"

#endif // MAIN_H
