#include "resources.h"
#include "message.cpp"
#include "connection.cpp"


#include "gui/imgui_resources.hpp"
#include "gui/chat.cpp"


static void glfw_error_callback(int error, const char* description)

{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}
bool isDigits(const std::string& akStr)

{
    return akStr.find_first_not_of("0123456789") == std::string::npos;
}
enum ArgFlag {
    eHelp,
    ePort,
    eNone
};
ArgFlag getFlag(const char* akArg)
{
  if (strcmp(akArg, "--help") == 0 || strcmp(akArg, "-h") == 0) {
    return eHelp;
  }
  if (strcmp(akArg, "--port") == 0 || strcmp(akArg, "-p") == 0) {
    return ePort;
  }
  return eNone;
}

bool validatePort(const char* akPort)
{
  return akPort != nullptr && isDigits(akPort);
}
int main(int argc, char* argv[])
{
    u_short port = DEFAULT_PORT;

    #pragma unroll 5
    for(int i=1; i<argc; i++)
    {
        if(getFlag(argv[i]) == eHelp) {
            std::cerr << "Usage: p2p_client <host> /n Flags:\n--port (-p)\tDefines the Port to use (default: 48932)\n--help (-h)\tShows this message" << std::endl;
            return 1;
        }
        if(getFlag(argv[i]) == ePort) {
            if(validatePort(argv[i+1])) {
                port = std::stoi(argv[i+1]);
            } else {
                std::cerr << "Invalid use of port flag" << std::endl;
                return 1;
            }
        }
    }

#pragma region GLFWinit
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Test", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync if 1

#pragma endregion

    boost::asio::io_context ioContext;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    io.Fonts->AddFontFromFileTTF("../imgui/misc/fonts/Roboto-Medium.ttf", 16.0f);

    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ChatWindow chatWindow(ioContext, port, window);



    while (!glfwWindowShouldClose(window))
    {

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        {
            bool open = true;
            chatWindow.Draw("Chat", &open);
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    glfwDestroyWindow(window);
    glfwTerminate();

    ioContext.stop();




    return 0;
}

