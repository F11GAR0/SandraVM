#ifndef VMCODE_COMPILER_H
#define VMCODE_COMPILER_H
#include "main.h"

//lets throw some compile errors!
class VMCError{
public:
    static void show(int offset, int line,std::string message){
        std::cout<<"Error at ["<<offset<<";"<<line<<"]: "<<message;
    }
};

class VMCodeArea{
public:
    VMCodeArea(std::vector<stInterpretEntity> lex_set){
        //TODO: Initializate variables and labels
    }
private:
    void loadVariables(){
        //obviously, load the fucking variables
    }
    std::pair<int, int> findSectionIndexes(std::vector<stInterpretEntity> lex_set){
        int var_sect = -1;
        int code_sect = -1;
        for(int i = 0, len = lex_set.size(); i < len; i++){
            if(lex_set[i].info._so.is_var_section){
                if(var_sect != -1){
                    VMCError::show(0, i, "finded another one .var section");
                    continue; //skip why not
                }
                var_sect = i;
            }
            if(lex_set[i].info._so.is_code_section){
                if(code_sect != -1){
                    VMCError::show(0, i, "finded another one .code section");
                    continue; //skip why not
                }
                code_sect = i;
            }
        }
        return std::make_pair(var_sect, code_sect);
    }
    std::map<std::string, var<DWORD>> m_mVariables;
    std::map<std::string, DWORD> m_mLabels; // "name", RVA (relative to the start of code area
};

class VMCodeCompiler{
public:
    explicit VMCodeCompiler(){
        m_sLoadedCode.clear();
    }
    void loadCode(std::string code){
        m_sLoadedCode += code;
    }
private:
    VMCodeCompiler( const VMCodeCompiler& ) = delete;
    void operator=( const VMCodeCompiler& ) = delete;
    std::string m_sLoadedCode;
};

VMCodeCompiler g_VMCodeCompiler;

#endif // VMCODE_COMPILER_H
