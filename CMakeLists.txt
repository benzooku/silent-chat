#boost
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

#INCLUDE_DIRECTORIES(${BOOST_LIBRARY_INCLUDES})
#include_directories(asio INTERFACE ${boost_asio_SOURCE_DIR}/include)

# Add source to this project's executable.
add_executable (p2p_client "src/main.cpp" "src/resources.h")


#target_include_directories(p2p_chat PRIVATE ${BOOST_LIBRARY_INCLUDES})

target_link_libraries(p2p_client
  PRIVATE
  Boost::asio
  Boost::filesystem
  Boost::thread
  Boost::program_options
)


set_property(TARGET p2p_client PROPERTY CXX_STANDARD 20)
