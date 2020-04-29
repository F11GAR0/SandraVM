#include "main.h"
#include <QCoreApplication>



namespace BuildSanOne{

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
void mov_reg_pdword(PVOID argvs){
    //so first, we need to know what register we need
    int p = 0;
    BYTE reg = ((BYTE*)argvs)[p++];
    //then we need to put value to register
    DWORD pointer = (DWORD)argvs + p;
    g_Registers[reg] = *(DWORD*)g_Memory.GetRealAddr(pointer);

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

//math operations


void add_reg_reg(PVOID argvs){
    BYTE dest = ((BYTE*)argvs)[0];
    BYTE src = ((BYTE*)argvs)[1];
    g_Registers[dest] += g_Registers[src];

#ifdef DEBUG
    std::cout<<"add_reg_reg( "<<(int)dest<<", "<<(int)src<<" );"<<std::endl;
#endif
}


void add_reg_dword(PVOID argvs){
    BYTE dest = ((BYTE*)argvs)[0];
    DWORD val = *(DWORD*)((DWORD)argvs + 1);;
    g_Registers[dest] += val;

#ifdef DEBUG
    std::cout<<"add_reg_dword( "<<(int)dest<<", "<<(int)val<<" );"<<std::endl;
#endif
}

void initMachine(){
    g_Machine.registerOpcode(eOpcTable::MOV_REG_DWORD, "mov", mov_reg_dword, 9, 2, eArgvType::TREG, eArgvType::TDWORD);
    g_Machine.registerOpcode(eOpcTable::MOV_REG_PDWORD, "mov", mov_reg_pdword, 9, 2, eArgvType::TREG, eArgvType::TPDWORD);
    g_Machine.registerOpcode(eOpcTable::MOV_REG_REG, "mov", mov_reg_reg, 2, 2, eArgvType::TREG, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::PUSH, "push", push, 1, 1, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::POP, "pop", pop, 1, 1, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::ADD_REG_REG, "add", add_reg_reg, 2, 2, eArgvType::TREG, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::ADD_REG_DWORD, "add", add_reg_dword, 9, 2, eArgvType::TREG, eArgvType::TDWORD);
}

}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    //test build
    BuildSanOne::initMachine();

    var<std::vector<BYTE>> code_sec;

    //lets do simple value swap
    const char *simple_code = ".var\n _global a\n.code\n mov sanax, a\n mov sanbx, a\n mov sanax, 12\npush sanax";

    std::string temp_line;
    for(int i = 0; i < strlen(simple_code); i++){
        if(simple_code[i] != '\n') temp_line += simple_code[i];
        else {
#ifdef DEBUG
            g_Interpretator.interpret_line(temp_line).print_info();
#endif
            temp_line.clear();
        }
    }
    if(temp_line.length() > 0){
#ifdef DEBUG
            g_Interpretator.interpret_line(temp_line).print_info();
#endif
    }
    code_sec.getValue().push_back(eOpcTable::MOV_REG_DWORD);
    code_sec.getValue().push_back(eRegByte::SANAX);
    code_sec.getValue().push_back(0x10); //SANAX = 10
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);

    code_sec.getValue().push_back(eOpcTable::MOV_REG_DWORD);
    code_sec.getValue().push_back(eRegByte::SANBX);
    code_sec.getValue().push_back(0x5); //SANBX = 5
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);

    code_sec.getValue().push_back(eOpcTable::PUSH);
    code_sec.getValue().push_back(eRegByte::SANAX); //

    code_sec.getValue().push_back(eOpcTable::MOV_REG_REG);
    code_sec.getValue().push_back(eRegByte::SANAX);
    code_sec.getValue().push_back(eRegByte::SANBX);

    code_sec.getValue().push_back(eOpcTable::POP);
    code_sec.getValue().push_back(eRegByte::SANBX); //

    code_sec.getValue().push_back(eOpcTable::ADD_REG_DWORD);
    code_sec.getValue().push_back(eRegByte::SANBX);
    code_sec.getValue().push_back(0x7); //SANBX = 5
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);
    code_sec.getValue().push_back(0x0);

    code_sec.getValue().push_back(eOpcTable::ADD_REG_REG);
    code_sec.getValue().push_back(eRegByte::SANAX);
    code_sec.getValue().push_back(eRegByte::SANBX);

    g_Machine.process((PVOID)code_sec.getValue().data(), code_sec.getValue().size());

    std::cout << "sanax: " << g_Registers.sanax << std::endl;
    std::cout << "sanbx: " << g_Registers.sanbx << std::endl;

    std::cin.get();
    return app.exec();
}
