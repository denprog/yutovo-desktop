cmake_minimum_required(VERSION 3.17)

project(yutovo_desktop
    VERSION 0.1.0 
    LANGUAGES CXX
)

#add_compile_definitions(REMOTE_SOLVER)
add_compile_definitions(DEBUG)
set(CMAKE_DEBUG_POSTFIX d)

if(WIN32)
    set(INSTALL_PATH "c:/Lang/vcpkg/installed/x64-windows")
else()
    set(INSTALL_PATH "$ENV{YUTOVO_DEPLOY}")
    set(THIRD_PARTY_PATH "$ENV{YUTOVO_DEPLOY}/../third_party/")
endif()

add_subdirectory(src)
