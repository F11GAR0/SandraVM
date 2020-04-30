#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H
#include "main.h"


namespace command_processor {
    //typedef section
    typedef void(*func)(std::string argvs);

    std::vector<std::pair<std::string, func>> cmds;
    std::vector<func> just_buff_callbacks;

    void register_command(std::string cmd, func callback) {
        cmds.push_back(std::pair<std::string, func>(cmd, callback));
    }

    void register_buffer_line_hook(func callback){
        just_buff_callbacks.push_back(callback);
    }

    std::string remove_first(const std::string& in) {
        std::string ret;
        for (int i = 1, len = in.length(); i < len; i++) {
            ret += in[i];
        }
        return ret;
    }

    std::string compress(const std::vector<std::string>& in) {
        std::string ret;
        for (int i = 0, len = in.size(); i < len; i++) {
            ret += in[i];
            ret += " ";
        }
        return ret;
    }

    std::vector<std::string> split(std::string in, char splitter) {
        std::vector<std::string> ret;
        std::string temp;
        for (int i = 0; i < in.length(); i++) {
            if (in[i] == splitter) {
                if (temp.length() > 0) {
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

    void buffer_proc() {
        auto show_cmds = [](std::string) {
            std::cout << "All registered commands:" << std::endl;
            for (int i = 0, size = cmds.size(); i < size; i++) {
                std::cout << cmds[i].first << std::endl;
            }
        };
        register_command("cmds", show_cmds);
        while (true) {
            char* in = (char*)malloc(512);
            std::cin.getline(in, 512);
#ifdef DEBUG
            printf("\ncommand entered: %s\n", in);
#endif
            if (strlen(in) > 0) {
                if (in[0] == '/') {
                    auto ret = split(in, ' ');
                    std::string cmd = remove_first(ret[0]);
                    ret.erase(ret.begin());
                    for (int i = 0; i < cmds.size(); i++) {
                        if (cmds[i].first == cmd) {
                            cmds[i].second(compress(ret));
                        }
                    }
                } else {
                    for(int i = 0; i < just_buff_callbacks.size(); i++){
                        just_buff_callbacks[i](in);
                    }
                }
            }
            free(in);
        }
    }

}

#endif // COMMAND_PROCESSOR_H
