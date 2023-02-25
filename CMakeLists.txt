cmake_minimum_required(VERSION 3.17)

project(yutovo_desktop
    VERSION 0.1.0 
    LANGUAGES CXX
)

add_compile_definitions(DEBUG)
set(CMAKE_DEBUG_POSTFIX d)

set(INSTALL_PATH "$ENV{YUTOVO_DEPLOY}")

add_subdirectory(src)
