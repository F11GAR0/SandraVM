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

void out(PVOID argvs){
    std::cout<<"reg: "<<g_Registers.get_reg_name_by_byte(((PBYTE)argvs)[0])<<" = "<<g_Registers[((PBYTE)argvs)[0]]<<std::endl;
}

void initMachine(){
    g_Machine.registerOpcode(eOpcTable::MOV_REG_DWORD, "mov", mov_reg_dword, 9, 2, eArgvType::TREG, eArgvType::TDWORD);
    g_Machine.registerOpcode(eOpcTable::MOV_REG_PDWORD, "mov", mov_reg_pdword, 9, 2, eArgvType::TREG, eArgvType::TPDWORD);
    g_Machine.registerOpcode(eOpcTable::MOV_REG_REG, "mov", mov_reg_reg, 2, 2, eArgvType::TREG, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::PUSH, "push", push, 1, 1, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::POP, "pop", pop, 1, 1, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::ADD_REG_REG, "add", add_reg_reg, 2, 2, eArgvType::TREG, eArgvType::TREG);
    g_Machine.registerOpcode(eOpcTable::ADD_REG_DWORD, "add", add_reg_dword, 9, 2, eArgvType::TREG, eArgvType::TDWORD);
    g_Machine.registerOpcode(eOpcTable::OUT, "out", out, 1, 1, eArgvType::TREG);
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
        VMCMessage::show("file created");
        std::cout<<"Outfile: "<<g_sDefaultFilePath<<" size: "<< len / 1024 << "kb"<< std::endl;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    //test build
    g_sDefaultFilePath = app.applicationDirPath().toStdString();
    g_sDefaultFilePath += "/app.san";
    BuildSanOne::initMachine();
    command_processor::register_buffer_line_hook(buff_proc);
    command_processor::register_command("start_write", start_write);
    command_processor::register_command("stop_write", stop_write);
    command_processor::register_command("show_buffer", show_buffer);
    command_processor::register_command("compile", compile);
    command_processor::buffer_proc();
    std::cin.get();
    return app.exec();
}
