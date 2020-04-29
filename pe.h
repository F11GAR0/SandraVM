#ifndef PE_H
#define PE_H
#include "main.h"

//pseudo header
class VMPE{
public:
    explicit VMPE(){
        m_pVMSignature = (PBYTE)malloc(16);
        for(int i = 0; i < 16; i++)
            m_pVMSignature[i] = SANDRA_SIGNATURE[i];
        m_bMajorVMVer = SANDRA_MAJOR_VER;
        m_bMinorVMVer = SANDRA_MINOR_VER;
    }
    void loadCode(PBYTE code, unsigned int size){
        m_pCodeSection = code;
        m_dwCodeSectionSize = size;
    }
    void loadData(PBYTE data, unsigned int size){
        m_pDataSection = data;
        m_dwDataSectionSize = size;
    }
    void getCode(PBYTE *out, PDWORD out_size){
        *out = m_pCodeSection;
        *out_size = m_dwCodeSectionSize;
    }
    void getData(PBYTE *out, PDWORD out_size){
        *out = m_pDataSection;
        *out_size = m_dwDataSectionSize;
    }
    void getBuilded(PBYTE *out_pe, PDWORD out_len){

        m_dwCodeSectionOffset = 18 + sizeof(DWORD) * 5;
        m_dwDataSectionOffset = m_dwCodeSectionSize + 0x1000 + m_dwCodeSectionSize;
        m_dwEntryPoint = 18 + sizeof(DWORD) * 5 + 0x1000;

        int p = 0;
        const unsigned int out_size = 18 + sizeof(DWORD) * 5 + 0x1000 + m_dwCodeSectionSize + m_dwDataSectionSize;
        *out_len = out_size;
        PBYTE ret = (PBYTE)malloc(out_size);
        for(int i = 0; i < 16; i++)
            *(BYTE*)((DWORD)ret + p++) = m_pVMSignature[i];
        *(BYTE*)((DWORD)ret + p++) = m_bMajorVMVer;
        *(BYTE*)((DWORD)ret + p++) = m_bMinorVMVer;
        *(DWORD*)((DWORD)ret + p++) = m_dwCodeSectionOffset;
        *(DWORD*)((DWORD)ret + p++) = m_dwDataSectionOffset;
        *(DWORD*)((DWORD)ret + p++) = m_dwCodeSectionSize;
        *(DWORD*)((DWORD)ret + p++) = m_dwDataSectionSize;

        *(DWORD*)((DWORD)ret + p++) = m_dwEntryPoint;

        for(int i = 0; i < 1000; i++){
            *(BYTE*)((DWORD)ret + p++) = 0x00;
        }

        for(int i = 0; i < m_dwCodeSectionSize; i++){
            *(BYTE*)((DWORD)ret + p++) = m_pCodeSection[i];
        }

        for(int i = 0; i < m_dwDataSectionSize; i++){
            *(BYTE*)((DWORD)ret + p++) = m_pDataSection[i];
        }

        //return
        *out_pe = (PBYTE)malloc(out_size);
        for(int i = 0; i < out_size; i++){
            (*out_pe)[i] = ret[i];
        }
    }
private:
    PBYTE m_pVMSignature; //should occup 16 bytes

    BYTE  m_bMinorVMVer;
    BYTE  m_bMajorVMVer;

    DWORD m_dwCodeSectionOffset; //starting from m_pReserved
    DWORD m_dwDataSectionOffset;
    DWORD m_dwCodeSectionSize;
    DWORD m_dwDataSectionSize;

    DWORD m_dwEntryPoint;

    BYTE  m_pReserved[0x1000];
    PBYTE m_pCodeSection;
    PBYTE m_pDataSection;
};

#endif // PE_H
