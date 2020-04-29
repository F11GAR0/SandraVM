#ifndef FILE_H
#define FILE_H
#include "main.h"

enum Mode {
    M_OPEN_WRITE_APPEND = std::ios_base::app,
    M_OPEN_READ = std::ios_base::in,
    M_OPEN_WRITE = std::ios_base::out,
    M_OPEN_WRITE_ATE = std::ios_base::ate,
    M_TRUNC = std::ios_base::trunc,
    M_BINARY = std::ios_base::binary
};

class FileWriter {
public:
    static void Delete(std::string path) {
        remove(path.c_str());
    }
    void Open(std::string path, Mode mode) {
        buff.open(path, (std::ios_base::openmode)mode);
    }
    void Close() {
        buff.close();
    }
    void Write(std::string message) {
        buff << message;
    }
    void Write(PBYTE byte, DWORD len){
        for(DWORD i = 0; i < len; i++)
            buff << byte[i];
    }
    template <typename T>
    void Write(const T& var) {
        buff << var;
    }
private:
    std::ofstream buff;
};

class FileReader {
public:
    void Open(std::string path) {
        m_ifBuffer.open(path, std::ios_base::binary);
    }
    void Close() {
        m_ifBuffer.close();
    }
    template <typename T>
    T Read() const {
        T ret;
        m_ifBuffer >> ret;
        return ret;
    }
    BYTE ReadByte(){
        BYTE ret;
        ret = m_ifBuffer.get();
        //m_ifBuffer >> ret;
        return ret;
    }
    DWORD ReadDword(){
        //just >> not work so
        PBYTE ret = (PBYTE)malloc(sizeof(DWORD));
        for(int i = 0; i < sizeof(DWORD); i++)
            ret[i] = ReadByte();
        DWORD val = *(DWORD*)ret;
        free(ret);
        return val;
    }
    void Skip(unsigned int bytes){
        for(int i = 0; i < bytes; i++)
            ReadByte();
    }
    /// <param name = "outbuff"> Out buffer. </param>
    /// <param name = "length"> If length is -1, function will return all text from file. </param>
    void ReadText(char *outbuff, int length = -1) {
        std::string out;
        if (length == -1) {
            while (!m_ifBuffer.eof()) {
                char one_char = '\0';
                m_ifBuffer >> one_char;
                out += one_char;
            }
        }
        else {
            for (int i = 0; i < length; i++) {
                char one_char = '\0';
                m_ifBuffer >> one_char;
                out += one_char;
            }
        }
        strcpy(outbuff, out.c_str());
    }
private:
    std::ifstream m_ifBuffer;
};

#endif // FILE_H
