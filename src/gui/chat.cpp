#include "imgui_resources.hpp"
#include <boost/regex.h>
#include "../connection.cpp"
#include "../resources.h"
#include <queue>

struct ChatWindow
{
private:
    std::string InputBuf;
    std::string addressBuf;
    std::vector<std::string> Logs;
    bool AutoScroll;
    bool ScrollToBottom;

    std::thread mThrContext;

    networking::Connection mConnection;
    std::string mRemoteAddress;
    u_short mRemotePort = DEFAULT_PORT;

    GLFWwindow* mWindow = nullptr;

    std::queue<networking::Message> messageQueue;


public:
    ChatWindow(boost::asio::io_context& aContext, const u_short &port, GLFWwindow* aWindow ) : mConnection(aContext, port, messageQueue),
    mWindow(aWindow)
    {
        AutoScroll = true;
        ScrollToBottom = false;
        AddLog("Welcome to p2p Chat!");
        InputBuf.clear();

        mThrContext = std::thread([&aContext](){ aContext.run(); });
    }
    ~ChatWindow()
    {
        mThrContext.join();
    }

    void AddLog(std::string aLog)
    {
        Logs.push_back(aLog);
    }

    void Draw(const char* title, bool* p_open)
    {


        if(!messageQueue.empty()){
            networking::Message msg = messageQueue.front();
            char buffer[MAX_MESSAGE_SIZE];
            msg.read(buffer);
            AddLog(mRemoteAddress + ": " + std::string(buffer));
            messageQueue.pop();
        }

        int width, height;
        glfwGetWindowSize(mWindow, &width, &height);

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(width, height));
        if (!ImGui::Begin(title, p_open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar ))
        {
            ImGui::End();
            return;
        }

        // TODO: display items starting from the bottom

        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::EndPopup();
        }

        // Options, Filter
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");

        ImGuiInputTextFlags address_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;
        if(ImGui::InputText("Address", &addressBuf, address_text_flags))
        {
            mRemoteAddress = addressBuf.c_str();
        }

        ImGui::Separator();

        // Reserve enough left-over height for 1 separator + 1 input text
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
        {

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
            for (std::string log : Logs)
            {
                ImVec4 color;
                bool has_color = false;

                ImGui::TextUnformatted(log.c_str());
            }

            if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
                ImGui::SetScrollHereY(1.0f);
            ScrollToBottom = false;

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
        ImGui::Separator();

        // Command-line
        bool reclaim_focus = false;
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        if (ImGui::InputText("Input", &InputBuf, input_text_flags))
        {
            std::string s = InputBuf;
            if (s.length() > 0){
                Send(s);
            }
            reclaim_focus = true;
            InputBuf.clear();
        }

        ImGui::SetItemDefaultFocus();
        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1);

        ImGui::End();
    }

    void Send(std::string akMsg)
    {
        AddLog("me: "+ akMsg);

        try{
            std::string message;
            message = akMsg;

            networking::Message msg;
            msg.header.id = MessageType::eText;
            msg.header.session_type = SessionType::eDisabled;
            std::array<char, MAX_MESSAGE_SIZE> buffer = {0};
            std::copy(message.begin(), message.end(), buffer.data());
            msg.add(buffer);
            mConnection.sendMessage(mRemoteAddress, mRemotePort, msg);

        }
        catch (std::exception& e)
        {
            AddLog(e.what());
        }

        ScrollToBottom = true;
    }



};
