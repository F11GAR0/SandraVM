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
    BuildSanOne::eOpcTable opc;
    PVOID data;
    /*
     * optional
     * Example:
     * if opcode use variables then we just put variable's names here
     * mov sanax, a
     * -> var1_name = "a";
     */
    std::string var1_name;
    std::string var2_name;
    bool is_section : 1;
    bool is_label : 1;
    bool is_variable : 1;
    bool is_just_code : 1;
};

class Interpretator{
public:
    Interpretator(){}
private:
    stInterpretEntity interpret_line(std::string line){
        auto splitted = split(line, ' ');
        //first should be opcode or label or section
        std::string first = splitted[0];
        if(first[0] == '.'){
            //section
            if(first == ".section_variables"){
                //TODO: complete this and add checks
            }
        }
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

   Interpretator( const Interpretator& ) = delete;
   void operator=( const Interpretator& ) = delete;

    std::vector<stInterpretEntity> m_vCode;
};

Interpretator g_Interpretator;

#endif // INTERPRETATOR_H
