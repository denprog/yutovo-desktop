cmake_minimum_required(VERSION 3.17)

project(yutovo-editor LANGUAGES CXX C)

set(PROJECT_LICENSE "GPL-3.0-only")

message(STATUS "Project license: ${PROJECT_LICENSE}")
message(STATUS "CXX Compiler ID: ${CMAKE_CXX_COMPILER_ID}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
else()
    add_compile_definitions(RELEASE)
endif()

if ((NOT DEFINED EMSCRIPTEN) AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    add_compile_definitions(TEST)
endif()

if(WIN32)
	set(INSTALL_PATH "$ENV{VCPKG_ROOT}/installed/x64-windows")
    set(PKG_CONFIG_PATH "$ENV{VCPKG_ROOT}/installed/x64-windows/lib/pkgconfig")
else()
    set(INSTALL_PATH "$ENV{YUTOVO_DEPLOY}")
endif()

set(THIRD_PARTY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../third-party/")

# set(REMOTE_SOLVER true)
# add_compile_definitions(REMOTE_SOLVER)

option(BUILD_TESTS "Build unit tests" ON)

add_subdirectory(src)
if((NOT DEFINED EMSCRIPTEN) AND BUILD_TESTS)
    add_subdirectory(test)
endif()
