cmake_minimum_required(VERSION 3.17)

project(yutovo-logger
    LANGUAGES CXX
)

set(PROJECT_LICENSE "GPL-3.0-only")

message(STATUS "Project license: ${PROJECT_LICENSE}")

add_compile_definitions(DEBUG)

if(WIN32)
    set(INSTALL_PATH "$ENV{VCPKG_ROOT}/installed/x64-windows")
else()
    set(INSTALL_PATH "$ENV{YUTOVO_DEPLOY}")
endif()

add_subdirectory(src)
