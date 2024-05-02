#include "resources.h"
#include "message.cpp"
#include "connection.cpp"

#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_glfw.h"
#include "../imgui/backends/imgui_impl_opengl3.h"
#include <stdio.h>

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif



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
    boost::asio::io_context ioContext;
    std::thread thrContext;
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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


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
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
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

/*
    try
    {
        networking::Connection connection(ioContext, port);

        thrContext = std::thread([&ioContext]() { ioContext.run(); });

        while(true){
            std::string input;
            std::cin >> input;

            if(input == "help"){
                std::cout << "exit\nsend [receiver] [message]\nsendwport [receiver] [message] [port]\nsession [accept/reject]\n";
            }
            else if(input == "exit") {
                ioContext.stop();
                break;
            }
            if(true){
                if(input == "send"){
                    std::string receiver;
                    std::string message;
                    std::cin >> receiver;
                    std::cin >> message;

                    networking::Message msg;
                    msg.header.id = MessageType::eText;
                    msg.header.session_type = SessionType::eDisabled;
                    std::array<char, MAX_MESSAGE_SIZE> buffer = {0};
                    std::copy(message.begin(), message.end(), buffer.data());
                    msg.add(buffer);
                    connection.sendMessage(receiver, DEFAULT_PORT, msg);
                }
                else if(input == "sendwport"){
                    std::string receiver;
                    std::string message;
                    std::string port;
                    std::cin >> receiver;
                    std::cin >> message;
                    std::cin >> port;

                    networking::Message msg;
                    msg.header.id = MessageType::eText;
                    msg.header.session_type = SessionType::eDisabled;
                    std::array<char, MAX_MESSAGE_SIZE> buffer = {0};
                    std::copy(message.begin(), message.end(), buffer.data());
                    msg.add(buffer);
                    connection.sendMessage(receiver, std::stoi(port), msg);
                }
                else if(input == "session"){
                    std::string command;
                    std::cin >> command;
                    if(command == "accept") {
                    }
                    if(command == "reject") {

                    }
                }
            } else {
                if(input == "session"){

                } else {
                }

            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << '\n';

    }
    thrContext.join();
    */


    return 0;
}

