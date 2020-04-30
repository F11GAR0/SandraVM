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
    bool is_reg_name(const std::string& reg_name){
        if(reg_name == "sanax"){
            return true;
        }
        if(reg_name == "sanbx"){
            return true;
        }
        if(reg_name == "sancx"){
            return true;
        }
        if(reg_name == "sandx"){
            return true;
        }
        return false;
    }
    eRegByte get_reg_id_by_name(const std::string& reg_name){
        if(reg_name == "sanax"){
            return eRegByte::SANAX;
        }
        if(reg_name == "sanbx"){
            return eRegByte::SANBX;
        }
        if(reg_name == "sancx"){
            return eRegByte::SANCX;
        }
        if(reg_name == "sandx"){
            return eRegByte::SANDX;
        }
        throw new std::exception();
    }
    std::string get_reg_name_by_byte(const BYTE& value){
        eRegByte b = (eRegByte)value;
        switch (b) {
        case eRegByte::SANAX:
            return "sanax";
        case eRegByte::SANBX:
            return "sanbx";
        case eRegByte::SANCX:
            return "sancx";
        case eRegByte::SANDX:
            return "sandx";
        default:
            return "unk";
        }
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
  unsigned short argv_count;
  stCallbackData(opc_callback _cb, unsigned short _argv_count){
      cb = _cb;
      argv_count = _argv_count;
  }
  stCallbackData(){
      argv_count = 0;
      cb = nullptr;
  }
};

enum eOpcTable{
    NULL_RESERVED = 0x0,
    MOV_REG_DWORD = 0x71,
    MOV_REG_PDWORD,
    MOV_PDWORD_REG,
    MOV_REG_REG,
    PUSH,
    POP,
    JMP_RVA,
    CALL,
    RET,
    ADD_REG_REG,
    ADD_REG_DWORD,
    CMP,
    OUT
};

enum eArgvType{
    TUNK = 0,
    TVAR,
    TREG,
    TDWORD,
    TPDWORD,
    TLABEL
};

struct OpcInfo{
    unsigned short argvs_count; //in bytes
    std::string opcode_family; //like mov, add and etc.
    eOpcTable opc;
    unsigned short factical_arguments_count; //2, 1 or 0
    eArgvType atype_first;
    eArgvType atype_second;
    OpcInfo(unsigned short _ac,
            unsigned short _fac,
            std::string _of,
            eOpcTable _op,
            eArgvType _atfirst = eArgvType::TUNK,
            eArgvType _atsec = eArgvType::TUNK
            ){
        argvs_count = _ac;
        opcode_family = _of;
        opc = _op;
        atype_second = _atsec;
        atype_first = _atfirst;
        factical_arguments_count = _fac;
    }
};

class Opcodes{
public:
    std::vector<OpcInfo> getOpcByFamily(std::string family){
        std::vector<OpcInfo> ret;
        if(m_mOpcodeTable.find(family) != m_mOpcodeTable.end()){
            return m_mOpcodeTable[family];
        }
        return ret;
    }
    void registerOpcodeInfo(eOpcTable opc,
                            std::string opcode_family,
                            unsigned short argvs_count,
                            unsigned short factical_argument_count,
                            eArgvType arg_type_first = eArgvType::TUNK,
                            eArgvType arg_type_second = eArgvType::TUNK
                            ){
        m_mOpcodeTable[opcode_family].push_back(OpcInfo(argvs_count, factical_argument_count, opcode_family, opc, arg_type_first, arg_type_second));
    }
private:
    std::map<std::string, std::vector<OpcInfo>> m_mOpcodeTable;
};

class OpcodeProcessor : public Opcodes{
public:
    void registerOpcode(BYTE value,
                        std::string family,
                        opc_callback callback,
                        unsigned short argv_count,
                        unsigned short factical_arguments_count,
                        eArgvType at_first = eArgvType::TUNK,
                        eArgvType at_second = eArgvType::TUNK
                        ){
        m_mCallbackTable[value] = stCallbackData(callback, argv_count);
        registerOpcodeInfo((eOpcTable)value, family, argv_count, factical_arguments_count, at_first, at_second);
    }
    DWORD getOpcArgvsBytes(BYTE opc){
        return m_mCallbackTable[opc].argv_count;
    }

    void process(PVOID code_section, int len){
        for(int i = 0; i < len; i++){
            BYTE opc = ((BYTE*)code_section)[i];
            if(m_mCallbackTable.find(opc) != m_mCallbackTable.end()){
                m_mCallbackTable[opc].cb((PVOID*)((DWORD)code_section + ++i));
                if(!m_bNeedJump)
                    i += m_mCallbackTable[opc].argv_count - 1;
                else {
                    i = m_dwJumpRVA - 1; //++ in end of cycle
                    m_bNeedJump = false;
                }
            }
        }
    }
    void jump(DWORD rva){
        m_bNeedJump = true;
        m_dwJumpRVA = rva;
    }
    OpcodeProcessor() {}
private:
    OpcodeProcessor( const OpcodeProcessor& ) = delete;
    void operator=( const OpcodeProcessor& ) = delete;
    bool m_bNeedJump;
    DWORD m_dwJumpRVA;
    std::map<BYTE, stCallbackData> m_mCallbackTable;
};

OpcodeProcessor g_Machine;

#endif // OPC_PROCESSOR_H
