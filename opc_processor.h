#ifndef OPC_PROCESSOR_H
#define OPC_PROCESSOR_H
#include "main.h"

/*
 * Lets think how we'll call our registers
 * sanax //argument or function return value
 * sanbx //some shit for calculations
 * sancx //another crap for calculations
 * sandx //one more bullshit for calculations
 *
 * sancmp //result of comparation
 *
 * sanip
 */

enum eRegByte{
    SANAX = 0x44,
    SANBX,
    SANCX,
    SANDX
};

struct stVirtualRegisters{
public:
    DWORD sanax;
    DWORD sanbx;
    DWORD sancx;
    DWORD sandx;
    DWORD& operator[](const std::string& reg_name){
        return get_reg_by_name(reg_name);
    }
    DWORD& operator[](const BYTE& value){
        return get_reg_by_byte_value(value);
    }
    stVirtualRegisters(){
        sanax = 0;
        sanbx = 0;
        sancx = 0;
        sandx = 0;
        sancmp = 0;
        sanip = 0;
    }
private:
    DWORD sancmp;
    DWORD sanip;

    DWORD& get_reg_by_name(const std::string& reg_name){
        if(reg_name == "sanax"){
            return sanax;
        }
        if(reg_name == "sanbx"){
            return sanbx;
        }
        if(reg_name == "sancx"){
            return sancx;
        }
        if(reg_name == "sandx"){
            return sandx;
        }
        throw new std::exception();
    }

    DWORD& get_reg_by_byte_value(const BYTE& value){
        eRegByte b = (eRegByte)value;
        switch (b) {
        case eRegByte::SANAX:
            return sanax;
        case eRegByte::SANBX:
            return sanbx;
        case eRegByte::SANCX:
            return sancx;
        case eRegByte::SANDX:
            return sandx;
        default:
            throw new std::exception();
            break;
        }
    }

    stVirtualRegisters( const stVirtualRegisters& ) = delete;
    void operator=( const stVirtualRegisters& ) = delete;
} g_Registers;

//PVOID - arguments
typedef void(*opc_callback)(PVOID);

struct stCallbackData{
  opc_callback cb;
  unsigned int argv_count;
  stCallbackData(opc_callback _cb, int _argv_count){
      cb = _cb;
      argv_count = _argv_count;
  }
  stCallbackData(){
      argv_count = 0;
      cb = nullptr;
  }
};

class OpcodeProcessor{
public:
    void registerOpcode(BYTE value, opc_callback callback, int argv_count){
        m_mCallbackTable[value] = stCallbackData(callback, argv_count);
    }
    void process(PVOID code_section, int len){
        for(int i = 0; i < len; i++){
            BYTE opc = ((BYTE*)code_section)[i];
            if(m_mCallbackTable.find(opc) != m_mCallbackTable.end()){
                m_mCallbackTable[opc].cb((PVOID*)((DWORD)code_section + ++i));
                i += m_mCallbackTable[opc].argv_count - 1;
            }
        }
    }
    OpcodeProcessor() {}
private:
    OpcodeProcessor( const OpcodeProcessor& ) = delete;
    void operator=( const OpcodeProcessor& ) = delete;

    std::map<BYTE, stCallbackData> m_mCallbackTable;
};

#endif // OPC_PROCESSOR_H
