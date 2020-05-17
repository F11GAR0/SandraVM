#include "main.h"
#include <QCoreApplication>

std::vector<std::string> split(std::string in, char separator) {
    std::vector<std::string> ret;
    std::string temp;
    for (int i = 0, len = in.length(); i < len; i++) {
        if (in[i] == separator) {
            if (temp.length() != 0) {
                ret.push_back(temp);
                temp.clear();
            }
        }
        else {
            temp += in[i];
        }
    }
    if (temp.length() > 0) {
        ret.push_back(temp);
    }
    return ret;
}


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
    DWORD pointer = *(DWORD*)((DWORD)argvs + p);
    DWORD real_addr = g_Memory.GetRealAddr(pointer);
    g_Registers[reg] = *(DWORD*)real_addr;

#ifdef DEBUG
    std::cout<<"mov_reg_pdword( "<<(int)reg<<", "<<g_Registers[reg]<<" );"<<std::endl;
#endif
}
void mov_pdword_reg(PVOID argvs){
    int p = 0;
    //virtual pointer
    DWORD pointer = *(DWORD*)((DWORD)argvs + p);
    p += sizeof(DWORD);
    BYTE reg = ((BYTE*)argvs)[p++];
    //g_Memory.MemSet(pointer, sizeof(DWORD), (PVOID)&g_Registers[reg], sizeof(DWORD));
    *(DWORD*)g_Memory.GetRealAddr(pointer) = g_Registers[reg];

#ifdef DEBUG
    std::cout<<"mov_pdword_reg( "<<g_Registers[reg]<<", "<<(int)reg<<" );"<<std::endl;
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
    DWORD val = *(DWORD*)((DWORD)argvs + 1);
    g_Registers[dest] += val;

#ifdef DEBUG
    std::cout<<"add_reg_dword( "<<(int)dest<<", "<<(int)val<<" );"<<std::endl;
#endif
}

void out(PVOID argvs){
    //std::cout<<"reg: "<<g_Registers.get_reg_name_by_byte(((PBYTE)argvs)[0])<<" = "<<(int)g_Registers[((PBYTE)argvs)[0]]<<std::endl;
    std::cout<<(int)g_Registers[((PBYTE)argvs)[0]]<<std::endl;
}

void jmp_rva(PVOID argvs){
    DWORD rva = *(DWORD*)argvs;
    g_Machine.jump(rva);
}

void cmp_reg_reg(PVOID argvs){
    BYTE reg1 = ((PBYTE)argvs)[0];
    BYTE reg2 = ((PBYTE)argvs)[1];
    if(g_Registers[reg1] == g_Registers[reg2]) g_Registers.sancmp = 0;
    else
    if(g_Registers[reg1] > g_Registers[reg2]) g_Registers.sancmp = 1;
    else
    if(g_Registers[reg1] < g_Registers[reg2]) g_Registers.sancmp = -1;
}

void cmp_reg_dword(PVOID argvs){
    BYTE reg1 = ((PBYTE)argvs)[0];
    DWORD val = *(DWORD*)((DWORD)argvs + 1);
    if(g_Registers[reg1] == val) g_Registers.sancmp = 0;
    else
    if(g_Registers[reg1] > val) g_Registers.sancmp = 1;
    else
    if(g_Registers[reg1] < val) g_Registers.sancmp = -1;
}

void je_rva(PVOID argvs){
    if(g_Registers.sancmp != 0){
        DWORD rva = *(DWORD*)argvs;
        g_Machine.jump(rva);
    }
}

void inc(PVOID argvs){
    BYTE reg = ((PBYTE)argvs)[0];
    g_Registers[reg] += 1;
}

void dec(PVOID argvs){
    BYTE reg = ((PBYTE)argvs)[0];
    g_Registers[reg] -= 1;
}

void in_reg(PVOID argvs){
    BYTE reg = ((PBYTE)argvs)[0];
    std::cin >> g_Registers[reg];
}

void in_pdword(PVOID argvs){
    DWORD pointer = *(DWORD*)argvs;
    std::cin >> *(DWORD*)g_Memory.GetRealAddr(pointer);
}

void mul_reg_reg(PVOID argvs){
    BYTE reg1 = ((PBYTE)argvs)[0];
    BYTE reg2 = ((PBYTE)argvs)[1];
    g_Registers[reg1] *= g_Registers[reg2];
}

void mul_reg_dword(PVOID argvs){
    BYTE reg1 = ((PBYTE)argvs)[0];
    DWORD val = *(DWORD*)((DWORD)argvs + 1);
    g_Registers[reg1] *= val;
}

void call_label(PVOID argvs){
    g_Stack.push(g_Machine.getCurrentPointerAddr() + sizeof(DWORD) + 1);
    DWORD rva = *(DWORD*)argvs;
    g_Machine.jump(rva);
}

void ret(PVOID argvs){
    DWORD ret_rva = g_Stack.pop();
    g_Machine.jump(ret_rva);
}

void nop(PVOID argvs){

}

void ts(PVOID argvs){
    std::cout << "Current ts: " << clock() / (double)CLOCKS_PER_SEC << " sec" << std::endl;
}

void initMachine(){
    g_Machine.registerOpcode(eOpcTable::MOV_REG_DWORD, "mov", mov_reg_dword, 1 + sizeof(DWORD), 2, eArgvType::TREG, eArgvType::TDWORD);
    g_Machine.registerOpcode(eOpcTable::MOV_REG_PDWORD, "mov", mov_reg_pdword, 1 + sizeof(DWORD), 2, eArgvType::TREG, eArgvType::TPDWORD);
    g_Machine.registerOpcode(eOpcTable::MOV_PDWORD_REG, "mov", mov_pdword_reg, 1 + sizeof(DWORD), 2, eArgvType::TPDWORD, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::MOV_REG_REG, "mov", mov_reg_reg, 2, 2, eArgvType::TREG, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::PUSH, "push", push, 1, 1, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::POP, "pop", pop, 1, 1, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::ADD_REG_REG, "add", add_reg_reg, 2, 2, eArgvType::TREG, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::ADD_REG_DWORD, "add", add_reg_dword, 1 + sizeof(DWORD), 2, eArgvType::TREG, eArgvType::TDWORD);
    g_Machine.registerOpcode(eOpcTable::OUT, "out", out, 1, 1, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::JMP_RVA, "jmp", jmp_rva, sizeof(DWORD), 1, eArgvType::TLABEL);
    g_Machine.registerOpcode(eOpcTable::JE_RVA, "je", je_rva, sizeof(DWORD), 1, eArgvType::TLABEL);
    g_Machine.registerOpcode(eOpcTable::CMP_REG_REG, "cmp", cmp_reg_reg, 2, 2, eArgvType::TREG, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::CMP_REG_DWORD, "cmp", cmp_reg_dword, 1 + sizeof(DWORD), 2, eArgvType::TREG, eArgvType::TDWORD);
    g_Machine.registerOpcode(eOpcTable::INC, "inc", inc, 1, 1, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::DEC, "dec", dec, 1, 1, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::IN_REG, "in", in_reg, 1, 1, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::IN_PDWORD, "in", in_pdword, sizeof(DWORD), 1, eArgvType::TPDWORD);
    g_Machine.registerOpcode(eOpcTable::MUL_REG_REG, "mul", mul_reg_reg, 2, 2, eArgvType::TREG, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::MUL_REG_DWORD, "mul", mul_reg_dword, 1 + sizeof(DWORD), 2, eArgvType::TREG, eArgvType::TDWORD);
    g_Machine.registerOpcode(eOpcTable::CALL_LABEL, "call", call_label, sizeof(DWORD), 1, eArgvType::TLABEL);
    g_Machine.registerOpcode(eOpcTable::RET, "ret", ret, 0, 0);
    g_Machine.registerOpcode(eOpcTable::NOP, "nop", nop, 0, 0);
    g_Machine.registerOpcode(eOpcTable::TS, "ts", ts, 0, 0);
}
}



bool g_bWriteAction = false;
std::string g_sCode = "";
void start_write(std::string){
    g_bWriteAction = true;
    std::cout<<"Write code here!"<<std::endl;
}

void stop_write(std::string){
    g_bWriteAction = false;
    std::cout<<"Write proc stoped."<<std::endl;
}

void show_buffer(std::string){
    std::cout<<g_sCode;
}

void clear_buffer(std::string){
    g_sCode.clear();
}

void buff_proc(std::string in){
    if(g_bWriteAction){
        g_sCode += in;
        g_sCode += "\n";
    }
}

std::string g_sDefaultFilePath;

void compile(std::string){
    if(g_sCode.size()){
        g_VMCodeCompiler.loadCode(g_sCode);
        VMPE pe;
        g_VMCodeCompiler.compile(&pe);
        FileWriter writer;
        writer.Open(g_sDefaultFilePath, Mode::M_OPEN_WRITE);
        PBYTE builded;
        DWORD len;
        pe.getBuilded(&builded, &len);
        VMCMessage::show("creating file...");
        writer.Write(builded, len);
        writer.Close();
        VMCMessage::show("Succesfuly compiled.");
        std::cout<<"Outfile: "<<g_sDefaultFilePath<<" size: "<< len / 1024 << "kb"<< std::endl;
    }
}

void load(std::string in){
    try{
    VMPE pe(g_sDefaultFilePath);
    PBYTE data;
    DWORD data_len;
    pe.getData(&data, &data_len);
#ifdef DEBUG
            printf("data: \n");
            for(int i = 0; i < data_len; i++){
                printf("%X ", data[i]);
            }
            printf("\n");
#endif
    for(DWORD i = 0; i < data_len; i += sizeof(DWORD)){
        g_Memory.MallocAtAddr(*(DWORD*)((DWORD)data + i), sizeof(DWORD));
    }
    free(data);
    pe.getCode(&data, &data_len);
#ifdef DEBUG
            printf("code: \n");
            for(int i = 0; i < data_len; i++){
                printf("%X ", data[i]);
            }
            printf("\n");
#endif
    g_Machine.process(data, data_len);
    //code executed so lets free memory
    g_Memory.FreeAll();
    }
    catch(std::exception &e){
        //
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    //test build
    g_sDefaultFilePath = app.applicationDirPath().toStdString();
    g_sDefaultFilePath += "/app.san";
    BuildSanOne::initMachine();
    ConsoleColor::logo();
    command_processor::register_buffer_line_hook(buff_proc);
    command_processor::register_command("start_write", start_write);
    command_processor::register_command("stop_write", stop_write);
    command_processor::register_command("show_buffer", show_buffer);
    command_processor::register_command("compile", compile);
    command_processor::register_command("load", load);
    command_processor::buffer_proc();
    std::cin.get();
    return app.exec();
}
/*
.var
    _global a
    _global b
.code
    in a
    in b
    mov sanax, a
    mov sanbx, b
    call @summ
    out sancx
    jmp @end
    :summ
    mov sancx, 0
    add sancx, sanax
    add sancx, sanbx
    ret
    :end
    nop
*/

/*
.var
.code
    mov sanax, 0
    ts
    :loop
    add sanax, 1
    cmp sanax, 65535
    je @loop
    ts
*/
