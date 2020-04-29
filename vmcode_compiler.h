#ifndef VMCODE_COMPILER_H
#define VMCODE_COMPILER_H
#include "main.h"

//lets throw some compile errors!
class VMCError{
public:
    static void show(int offset, int line,std::string message){
        std::cout<<"[SANDRA]: Error at ["<<offset<<";"<<line<<"]: "<<message<<std::endl;
    }
};

class VMCMessage{
public:
    static void show(std::string message){
        std::cout<<"[SANDRA]: "<<message<<std::endl;
    }
};

class VMCodeArea{
public:
    explicit VMCodeArea(std::vector<stInterpretEntity> lex_set){
        //TODO: Initializate variables and labels
        std::pair<int, int> sections = findSectionIndexes(lex_set);
        loadVariables(lex_set, sections.first, sections.second);
        std::cout<<"Variables found: "<<m_mVariables.size()<<std::endl;
        loadLabels(lex_set, sections.second, lex_set.size());
        std::cout<<"Labels found: "<<m_mLabels.size()<<std::endl;
        clearLables(&lex_set);
        linkByteCode(lex_set, &m_pResultCode, &m_dwCodeLen, sections.second);
    }
    void getCode(PBYTE *out, PDWORD out_len){
        *out = m_pResultCode;
        *out_len = m_dwCodeLen;
    }
    void getData(PBYTE *out, PDWORD out_len){
        int p = 0;
        for(auto& item : m_mVariables)
        {
            *(DWORD*)((DWORD)*out + p) = item.second;
            p += sizeof(DWORD);
        }
        *out_len = p;
    }
private:
    void linkByteCode(std::vector<stInterpretEntity> lex_set, PBYTE *out, PDWORD code_len, unsigned int code_sect_start){
        std::vector<BYTE> vdata;
        auto push_dword = [](std::vector<BYTE> *v, DWORD in){
            PBYTE data = (PBYTE)malloc(sizeof(DWORD));
            *(DWORD*)data = in;
            for(int i = 0; i < sizeof(PBYTE); i++){
                v->push_back(data[i]);
            }
            free(data);
        };
        auto push_byte = [](std::vector<BYTE> *v, BYTE in){
            v->push_back(in);
        };
        for(int i = code_sect_start + 1, len = lex_set.size(); i < len; i++){
            if(!lex_set[i].info.is_not_just_code){
                stInterpretEntity ent = lex_set[i];
                push_byte(&vdata, (BYTE)ent.opc);
                if(ent.argvs_count >= 1){
                    switch(ent.var1_type){
                    case eArgvType::TDWORD:
                    case eArgvType::TPDWORD:
                        push_dword(&vdata, *(DWORD*)ent.data);
                        break;
                    case eArgvType::TREG:
                        push_byte(&vdata, (BYTE)g_Registers.get_reg_id_by_name(ent.var1_name));
                        break;
                    case eArgvType::TVAR: //its like pdword
                        push_dword(&vdata, m_mVariables[ent.var1_name]);
                        break;
                    }
                    if(ent.argvs_count > 1){
                        switch(ent.var2_type){
                        case eArgvType::TDWORD:
                        case eArgvType::TPDWORD:
                            push_dword(&vdata, *(DWORD*)ent.data);
                            break;
                        case eArgvType::TREG:
                            push_byte(&vdata, (BYTE)g_Registers.get_reg_id_by_name(ent.var2_name));
                            break;
                        case eArgvType::TVAR: //its like pdword
                            push_dword(&vdata, m_mVariables[ent.var2_name]);
                            break;
                        }
                    }
                }
            }
        }

        *out = (PBYTE)malloc(vdata.size());
        for(int i = 0; i < vdata.size(); i++){
            (*out)[i] = vdata[i];
        }
        *code_len = vdata.size();
    }
    void loadVariables(std::vector<stInterpretEntity> lex_set, unsigned int start, unsigned int end){
        //obviously, load the fucking variables
        DWORD p = 0;
        for(int i = start; i < end; i++){
            if(lex_set[i].info._so.is_variable){
                if(m_mVariables.find(lex_set[i].var1_name) != m_mVariables.end()){
                    std::string mess = "multiplie definition of '";
                    mess += lex_set[i].var1_name;
                    mess += "' variable";
                    VMCError::show(0, i, mess);
                    continue;
                }
                m_mVariables[lex_set[i].var1_name] = p;
                p += sizeof(DWORD);
            }
        }
    }
    void loadLabels(std::vector<stInterpretEntity> lex_set, unsigned int start_code_sect, unsigned int end_code_sect){
        for(int i = start_code_sect; i < end_code_sect; i++){
            if(lex_set[i].info._so.is_label){
                if(m_mLabels.find(lex_set[i].var1_name) != m_mLabels.end()){
                    std::string mess = "multiplie definition of '";
                    mess += lex_set[i].var1_name;
                    mess += "' label";
                    VMCError::show(0, i, mess);
                    continue;
                }
                m_mLabels[lex_set[i].var1_name] = i;
            }
        }
    }
    void clearLables(std::vector<stInterpretEntity> *lex_set){
        for(auto i = lex_set->begin(); i != lex_set->end(); *i++){
            if((*i).info._so.is_label){
                lex_set->erase(i);
            }
        }
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
    std::map<std::string, DWORD> m_mVariables; //"name", RVA (relative to the start of data area)
    std::map<std::string, DWORD> m_mLabels; // "name", RVA (relative to the start of code area
    PBYTE m_pResultCode;
    DWORD m_dwCodeLen;
    PBYTE m_pResultData;
    DWORD m_dwDataLen;
};

class VMCodeCompiler{
public:
    explicit VMCodeCompiler(){
        m_sLoadedCode.clear();
    }
    void loadCode(std::string code){
        VMCMessage::show("loading code.");
        m_sLoadedCode += code;
    }

    void compile(VMPE *out){
        if(m_sLoadedCode.size() > 0){
            auto lines = split(m_sLoadedCode, '\n');
            std::vector<stInterpretEntity> lex_set;
            VMCMessage::show("parsing.");
            for(int i = 0, len = lines.size(); i < len; i++){
                lex_set.push_back(g_Interpretator.interpret_line(lines[i]));
            }
            VMCMessage::show("linking.");
            PBYTE out;
            DWORD out_len;
            VMCodeArea area(lex_set);
            area.getCode(&out, &out_len);
            VMPE pe;
            pe.loadCode(out, out_len);
            area.getCode(&out, &out_len);
            pe.loadData(out, out_len);
        }
    }

private:
    VMCodeCompiler( const VMCodeCompiler& ) = delete;
    void operator=( const VMCodeCompiler& ) = delete;
    std::string m_sLoadedCode;
};

VMCodeCompiler g_VMCodeCompiler;

#endif // VMCODE_COMPILER_H
