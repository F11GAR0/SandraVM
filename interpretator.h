#ifndef INTERPRETATOR_H
#define INTERPRETATOR_H
#include "main.h"


/* Lets think about how will look our code
 * .section_variables
 *  _global a = 2
 *  _global b = -7
 * .section_code
 *  mov sanax, a
 *  mov sanbx, b
 *  push sanax
 *  mov sanax, sanbx
 *  pop sanbx
 * :out
 *  out sanax
 *  out sanbx
 */

struct stInterpretEntity{
    eOpcTable opc;
    PVOID data;
    /*
     * optional
     * Example:
     * if opcode use variables then we just put variable's names here
     * mov sanax, a
     * -> var1_name = "a";
     */
    unsigned short argvs_count;
    std::string var1_name;
    eArgvType   var1_type;
    std::string var2_name;
    eArgvType   var2_type;
    union{
        struct so{
            bool is_var_section : 1;
            bool is_code_section : 1;
            bool is_label : 1;
            bool is_variable : 1;
        } _so;
        bool is_not_just_code;
    } info;
    stInterpretEntity(){
        opc = eOpcTable::NULL_RESERVED;
        data = nullptr;
        var1_name.clear();
        var2_name.clear();
        var1_type = eArgvType::TUNK;
        argvs_count = 0;
        var2_type = eArgvType::TUNK;
        info.is_not_just_code = false;
    }
#ifdef DEBUG
    void print_info(){
        printf("opc: 0x%X Data: 0x%p Argvs: %d Var1: %s Var2: %s Type of var1: %d Type of var2: %d\n", opc, data, argvs_count, var1_name.c_str(), var2_name.c_str(), var1_type, var2_type);
        printf("inf: is_var_sec: %d is_code_sec: %d is_label: %d is_var: %d\n", info._so.is_var_section, info._so.is_code_section, info._so.is_label, info._so.is_variable);
    }
#endif
};

#define INTERPRET_BUFF_RESERVE 8

class Interpretator{
public:
    Interpretator(){}
    stInterpretEntity interpret_line(std::string line){
        stInterpretEntity ret;

        auto splitted = split(line, ' ');
        //first should be opcode or label or section
        std::string first = splitted[0];
        if(first[0] == '\t') first = erase_start(first);
        if(first[0] == '.'){
            //section
            if(first == ".var"){
                ret.info._so.is_var_section = true;
                return ret; //no more lol
            }
            if(first == ".code"){
                ret.info._so.is_code_section = true;
                return ret; //and no more
            }
        } else {
            if(first[0] == ':'){
                //label
                first.erase(0, 1);
                ret.var1_name = first;
                ret.info._so.is_label = true;
                return ret;
            } else {
                //maybe its variable?
                if(first == "_global"){
                    ret.var1_name = splitted[1]; //variable name
                    ret.info._so.is_variable = true;
                    return ret;
                } else {
                    //ok no more variants. obviously, its opcode!!!
                    auto opc_info = g_Machine.getOpcByFamily(first);

                    auto argvs = get_vec_from_vec(splitted, 1);
                    std::string compressed_argvs = remove_spaces(compress(argvs, ' '));
                    argvs = split(compressed_argvs, ',');

                    int arguments_count = argvs.size();
                    //OpcInfo finded;
                    for(int i = 0; i < opc_info.size(); i++){
                        if(opc_info[i].factical_arguments_count == arguments_count){
                            ret.argvs_count = arguments_count;
                            if(arguments_count == 1){
                                if(opc_info[i].atype_first == type_is(argvs[0])){
                                    ret.opc = opc_info[i].opc;
                                    ret.info.is_not_just_code = false;
                                    ret.var1_type = type_is(argvs[0]);
                                    switch(ret.var1_type){
                                    case eArgvType::TREG:
                                        ret.var1_name = argvs[0];
                                        break;
                                    case eArgvType::TDWORD:
                                        ret.data = malloc(sizeof(DWORD));
                                        *(DWORD*)ret.data = std::stoi(argvs[0]);
                                        break;
                                    case eArgvType::TPDWORD:
                                        ret.data = malloc(sizeof(DWORD));
                                        *(DWORD*)ret.data = std::stoi(erase_start_end(argvs[0]));
                                        break;
                                    case eArgvType::TVAR:
                                        ret.var1_name = argvs[0];
                                        break;
                                    }
                                }
                                return ret;
                            }
                            if(arguments_count == 2){

                                eArgvType type_first = type_is(argvs[0]) == eArgvType::TVAR ? eArgvType::TPDWORD : type_is(argvs[0]);
                                eArgvType type_sec = type_is(argvs[1]) == eArgvType::TVAR ? eArgvType::TPDWORD : type_is(argvs[1]);

                                if(opc_info[i].atype_first == type_first && opc_info[i].atype_second == type_sec){
                                    ret.opc = opc_info[i].opc;
                                    ret.info.is_not_just_code = false;
                                    ret.var1_type = type_is(argvs[0]);
                                    switch(ret.var1_type){
                                    case eArgvType::TREG:
                                        ret.var1_name = argvs[0];
                                        break;
                                    case eArgvType::TDWORD:
                                        ret.data = malloc(sizeof(DWORD) + INTERPRET_BUFF_RESERVE);
                                        *(DWORD*)ret.data = std::stoi(argvs[0]);
                                        break;
                                    case eArgvType::TPDWORD:
                                        ret.data = malloc(sizeof(DWORD) + INTERPRET_BUFF_RESERVE);
                                        *(DWORD*)ret.data = std::stoi(erase_start_end(argvs[0]));
                                        break;
                                    case eArgvType::TVAR:
                                        ret.var1_name = argvs[0];
                                        break;
                                    }
                                    ret.var2_type = type_is(argvs[1]);
                                    switch(ret.var2_type){
                                    case eArgvType::TREG:
                                        ret.var2_name = argvs[1];
                                        break;
                                    case eArgvType::TDWORD:
                                        if(!ret.data){
                                            ret.data = malloc(sizeof(DWORD));
                                            *(DWORD*)ret.data = std::stoi(argvs[1]);
                                        } else {
                                            *(DWORD*)((DWORD)ret.data + sizeof(DWORD)) = std::stoi(argvs[1]);
                                        }
                                        break;
                                    case eArgvType::TPDWORD:
                                        if(!ret.data){
                                            ret.data = malloc(sizeof(DWORD));
                                            *(DWORD*)ret.data = std::stoi(argvs[1]);
                                        } else {
                                            *(DWORD*)((DWORD)ret.data + sizeof(DWORD)) = std::stoi(erase_start_end(argvs[1]));
                                        }
                                        break;
                                    case eArgvType::TVAR:
                                        ret.var2_name = argvs[1];
                                        break;
                                    }

                                    return ret;
                                }
                            }
                        }
                    } //for
                } //else var
            } //else gloabal
        }
        return ret;
    }
private:
    eArgvType type_is(std::string in){
        if(in[0] == '[' && in[in.length() - 1] == ']') return eArgvType::TPDWORD;
        if(is_reg_name(in)) return eArgvType::TREG;
        for(int i = 0, len = in.length(); i < len; i++){
            if(!std::isdigit(in[i])){
                return eArgvType::TVAR;
            }
        }
        return eArgvType::TDWORD;
    }

    std::string erase_start_end(const std::string& in){
        std::string ret;
        for(int i = 1, len = in.length() - 1; i < len; i++){
            ret += in[i];
        }
        return ret;
    }

    std::string erase_start(const std::string& in){
        std::string ret;
        for(int i = 1, len = in.length(); i < len; i++){
            ret += in[i];
        }
        return ret;
    }

    std::string remove_spaces(const std::string& in){
        std::string ret;
        for(int i = 0, len = in.length(); i < len; i++){
            if(in[i] != ' ') ret += in[i];
        }
        return ret;
    }

    bool is_reg_name(const std::string& in){
        return g_Registers.is_reg_name(in);
    }

    std::string compress(std::vector<std::string> in, char separator){
        std::string ret;
        for(auto& iter : in){
            ret += iter;
            ret += separator;
        }
        ret.pop_back();
        return ret;
    }

    std::vector<std::string> get_vec_from_vec(std::vector<std::string> in, int from, int _to = -1){
        std::vector<std::string> ret;
        for(int i = from, to = (_to == -1 ? in.size() : _to); i < to; i++){
            ret.push_back(in[i]);
        }
        return ret;
    }

   Interpretator( const Interpretator& ) = delete;
   void operator=( const Interpretator& ) = delete;

    std::vector<stInterpretEntity> m_vCode;
};

Interpretator g_Interpretator;

#endif // INTERPRETATOR_H
