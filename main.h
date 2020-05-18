/*
███████╗#█████╗#███╗###██╗██████╗#██████╗##█████╗#
██╔════╝██╔══██╗████╗##██║██╔══██╗██╔══██╗██╔══██╗
███████╗███████║██╔██╗#██║██║##██║██████╔╝███████║
╚════██║██╔══██║██║╚██╗██║██║##██║██╔══██╗██╔══██║
███████║██║##██║██║#╚████║██████╔╝██║##██║██║##██║
╚══════╝╚═╝##╚═╝╚═╝##╚═══╝╚═════╝#╚═╝##╚═╝╚═╝##╚═╝
##################################################
*/

#ifndef MAIN_H
#define MAIN_H

//#define DEBUG

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
#include <algorithm>
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

namespace ConsoleColor{
const char*  BOLD = "\033[1m";
const char*  red="\033[0;31m";
const char*  RED="\033[1;31m";
const char*  GREEN="\033[0;32m";
const char*  green="\033[1;32m";
const char*  YELLOW="\033[0;33m";
const char*  yellow="\033[1;33m";
const char*  blue="\033[0;34m";
const char*  BLUE="\033[1;34m";
const char*  MAGENTA="\033[0;35m";
const char*  magenta="\033[1;35m";
const char*  cyan="\033[0;36m";
const char*  CYAN="\033[1;36m";
const char*  NC="\033[0m"; // No Color
void logo(){


    /*
    "███████╗#█████╗#███╗###██╗██████╗#██████╗##█████╗#\n██╔════╝██╔══██╗████╗##██║██╔══██╗██╔══██╗██╔══██╗\n███████╗███████║██╔██╗#██║██║##██║██████╔╝███████║\n╚════██║██╔══██║██║╚██╗██║██║##██║██╔══██╗██╔══██║\n███████║██║##██║██║#╚████║██████╔╝██║##██║██║##██║\n╚══════╝╚═╝##╚═╝╚═╝##╚═══╝╚═════╝#╚═╝##╚═╝╚═╝##╚═╝\n##################################################\n";
    */
    //const char *lg2 = "███████╗#█████╗#███╗###██╗██████╗#██████╗##█████╗#\n██╔════╝██╔══██╗████╗##██║██╔══██╗██╔══██╗██╔══██╗\n███████╗███████║██╔██╗#██║██║##██║██████╔╝███████║\n╚════██║██╔══██║██║╚██╗██║██║##██║██╔══██╗██╔══██║\n███████║██║##██║██║#╚████║██████╔╝██║##██║██║##██║\n╚══════╝╚═╝##╚═╝╚═╝##╚═══╝╚═════╝#╚═╝##╚═╝╚═╝##╚═╝\n##################################################\n";
    const char *lg = "##########\n##SSSSSS#######\n##SS############\n##SSSSSS##SSSS###\n#######S##S##S####\n##SSSSSS##SSSSS####\n###################";
    for(int i = 0, len = strlen(lg); i < len; i++){
      std::cout<<(char*)((lg[i] == '#') ? BLUE : cyan)<<lg[i];
    }
    std::cout<<std::endl<<BLUE<<"Sandra loaded. Version: "<<MAGENTA<<SANDRA_MAJOR_VER<<"."<<SANDRA_MINOR_VER<<BLUE<<" by F11GAR0"<<NC<<std::endl;
    std::cout<<NC<<std::endl;
}
}

#include "file.h"

#include "memory.h"
#include "stack.h"
#include "opc_processor.h"
#include "parser.h"
#include "pe.h"
#include "vmcode_compiler.h"
#include "command_processor.h"

#endif // MAIN_H
