#include "Log.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "imgui.h"
#include "Renderer.h"


Log& Log::GetInstance()
{
    static Log instance;
    return instance;
}

Log::Log()
    {
        AutoScroll = true;
        Clear();
    }

void Log::Clear()
{
    Buf.clear();
}

void Log::AddLogGlError(const GLErrorInfo& gl_error_info)
{
    std::stringstream ss;

    ss << std::fixed;
    ss << std::setprecision(2);

    ss << "[" << gl_error_info.time << "] [" << gl_error_info.severity << "] (" << gl_error_info.type << ") severity, raised from "<< gl_error_info.source;
    LogLine ll(ss.str(), gl_error_info.msg);
    Buf.push_back(ll);
}
void Log::AddLog(std::string str) IM_FMTARGS(2)
{
    std::stringstream ss;

    ss << std::fixed;
    ss << std::setprecision(2);

    ss << "[" << ImGui::GetTime() << "] [ LOG ] " << str;
    LogLine ll( ss.str( ), "");
    Buf.push_back(ll);
}

void Log::Draw(const char* title, bool* p_open)
{
    if (!ImGui::Begin(title, p_open))
    {
        ImGui::End();
        return;
    }

    // Options menu
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &AutoScroll);
        ImGui::EndPopup();
    }

    // Main window
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    bool clear = ImGui::Button("Clear");
    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");

    ImGui::Separator();
    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    if (clear)
        Clear();
    if (copy)
        ImGui::LogToClipboard();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    for (uint32_t i = 0;i < Buf.size();i++)
    {
        if(Buf[i].line2 != "")
        {
	        if (ImGui::TreeNode((void*)i,Buf[i].line.c_str()))
	        {
	            ImGui::Text(Buf[i].line2.c_str());
	            ImGui::TreePop();
                std::cout << Buf[i].line2 << std::endl;
	        }
        }
        else
        {
            ImGui::BulletText(Buf[i].line.c_str());
        }
    }

    ImGui::PopStyleVar();

    if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
    ImGui::End();
}
