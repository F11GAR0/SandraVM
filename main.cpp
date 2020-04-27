#include "main.h"
#include <QCoreApplication>

#define DEBUG

namespace BuildSanOne{
enum eOpcTable{
    MOV_REG_DWORD = 0x71,
    MOV_REG_PDWORD,
    MOV_DWORD_REG,
    MOV_REG_REG,
    PUSH,
    POP,
    JMP,
    CALL,
    ADD,
    CMP
};
void mov_reg_dword(PVOID argvs){
    //so first, we need to know what register we need
    int p = 0;
    BYTE reg = ((BYTE*)argvs)[p++];
    //then we need to put value to register
    g_Registers[reg] = *(DWORD*)((DWORD)argvs + p);

#ifdef DEBUG
    std::cout<<"mov_reg_dword( "<<(int)reg<<", "<<g_Registers[reg]<<" );"<<std::endl;
#endif
}
void mov_reg_reg(PVOID argvs){
    BYTE dest = ((BYTE*)argvs)[0];
    BYTE src = ((BYTE*)argvs)[1];
    g_Registers[dest] = g_Registers[src];

#ifdef DEBUG
    std::cout<<"mov_reg_reg( "<<(int)dest<<", "<<(int)src<<" );"<<std::endl;
#endif
}
void push(PVOID argvs){
    g_Stack.push(g_Registers[((BYTE*)argvs)[0]]);

#ifdef DEBUG
    std::cout<<"push( "<<(int)(((BYTE*)argvs)[0])<<" );"<<std::endl;
#endif
}
void pop(PVOID argvs){
    g_Registers[((BYTE*)argvs)[0]] = g_Stack.pop();

#ifdef DEBUG
    std::cout<<"pop( "<<(int)(((BYTE*)argvs)[0])<<" );"<<std::endl;
#endif
}

OpcodeProcessor g_Machine;

void initMachine(){
    g_Machine.registerOpcode(eOpcTable::MOV_REG_DWORD, mov_reg_dword, 9);
    g_Machine.registerOpcode(eOpcTable::MOV_REG_REG, mov_reg_reg, 2);
    g_Machine.registerOpcode(eOpcTable::PUSH, push, 1);
    g_Machine.registerOpcode(eOpcTable::POP, pop, 1);
}

}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    //test build
    BuildSanOne::initMachine();

    var<std::vector<BYTE>> code_sec;

    //lets do simple value swap

    code_sec.getValue().push_back(BuildSanOne::eOpcTable::MOV_REG_DWORD);
    code_sec.getValue().push_back(eRegByte::SANAX);
    code_sec.getValue().push_back(0x10); //SANAX = 10
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);

    code_sec.getValue().push_back(BuildSanOne::eOpcTable::MOV_REG_DWORD);
    code_sec.getValue().push_back(eRegByte::SANBX);
    code_sec.getValue().push_back(0x5); //SANBX = 5
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);

    code_sec.getValue().push_back(BuildSanOne::eOpcTable::PUSH);
    code_sec.getValue().push_back(eRegByte::SANAX); //

    code_sec.getValue().push_back(BuildSanOne::eOpcTable::MOV_REG_REG);
    code_sec.getValue().push_back(eRegByte::SANAX);
    code_sec.getValue().push_back(eRegByte::SANBX);

    code_sec.getValue().push_back(BuildSanOne::eOpcTable::POP);
    code_sec.getValue().push_back(eRegByte::SANBX); //

    BuildSanOne::g_Machine.process((PVOID)code_sec.getValue().data(), code_sec.getValue().size());

    std::cout << "sanax: " << g_Registers.sanax << std::endl;
    std::cout << "sanbx: " << g_Registers.sanbx << std::endl;

    std::cin.get();
    return app.exec();
}
