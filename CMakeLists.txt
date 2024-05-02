cmake_minimum_required(VERSION 3.20)
project(p2p_client)

set(BOOST_INCLUDE_LIBRARIES thread filesystem system program_options asio date_time)
set(BOOST_ENABLE_CMAKE ON)

include(FetchContent)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_PROGRESS TRUE
  GIT_TAG boost-1.85.0
)
FetchContent_MakeAvailable(Boost)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

set(IMGUI_SOURCES
    imgui/imconfig.h
    imgui/imgui_demo.cpp
    imgui/imgui.cpp
    imgui/imgui.h
    imgui/imgui_draw.cpp
    imgui/imgui_internal.h
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h

    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_glfw.h
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_opengl3.h
    imgui/misc/debuggers/imgui.natvis
    imgui/misc/debuggers/imgui.natstepfilter
    imgui/misc/cpp/imgui_stdlib.cpp
    imgui/misc/cpp/imgui_stdlib.h
)

set(SOURCES
    src/main.cpp
    src/connection.cpp
    src/session.cpp
    src/handler.cpp
    src/message.cpp
)



# Add source to this project's executable.
add_executable (p2p_client ${SOURCES} ${IMGUI_SOURCES})

target_include_directories(p2p_client PRIVATE imgui
  "${CMAKE_CURRENT_LIST_DIR}/source"
)

set(IMGUI_LIBRARIES ImGui;glfw)


target_link_libraries(p2p_client
  PRIVATE
  Boost::asio
  Boost::filesystem
  Boost::thread
  Boost::program_options
  glfw
  OpenGL::GL
  ${CMAKE_DL_LIBS}
)


set_property(TARGET p2p_client PROPERTY CXX_STANDARD 20)
