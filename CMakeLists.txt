cmake_minimum_required(VERSION 3.17)

project(yutovo-desktop
    VERSION 0.1.0 
    LANGUAGES CXX
)

#add_compile_definitions(REMOTE_SOLVER)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()

set(CMAKE_DEBUG_POSTFIX d)

if(WIN32)
    set(INSTALL_PATH "c:/Lang/vcpkg/installed/x64-windows")
else()
    set(INSTALL_PATH "$ENV{YUTOVO_DEPLOY}")
endif()

add_subdirectory(src)
