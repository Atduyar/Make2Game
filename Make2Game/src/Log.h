#pragma once
#include <string>
#include <vector>

#include "imgui.h"
#include "Renderer.h"


class LogLine
{
public:
    std::string line;
    std::string line2;
    inline LogLine(std::string l, std::string l2) : line(l), line2(l2) {};
};

class Log
{
public:
    std::vector<LogLine>    Buf;
    bool                    AutoScroll;

    static Log& GetInstance();

    Log(Log const&) = delete;
    void operator=(Log const&) = delete;
    
    void Clear();
    void AddLog(std::string str);
    void AddLogGlError(const GLErrorInfo& gl_error_info);
    void Draw(const char* title, bool* p_open = nullptr);
private:
    Log();
};
