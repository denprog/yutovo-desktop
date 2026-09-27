cmake_minimum_required(VERSION 3.10)

project(yutovo-solver LANGUAGES CXX C)

set(PROJECT_LICENSE "GPL-3.0-only")

message(STATUS "Project license: ${PROJECT_LICENSE}")
message(STATUS "CXX Compiler ID: ${CMAKE_CXX_COMPILER_ID}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()

if(WIN32)
    set(INSTALL_PATH "$ENV{VCPKG_ROOT}/installed/x64-windows")
else()
    set(INSTALL_PATH "$ENV{YUTOVO_DEPLOY}")
endif()

# set(REMOTE_MODE true)
# add_compile_definitions(REMOTE_MODE)

option(BUILD_TESTS "Build unit tests" ON)

if(NOT WIN32 AND NOT EMSCRIPTEN)
    set(CMAKE_FIND_ROOT_PATH ${INSTALL_PATH}/lib/cmake/)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)
    pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)
    find_package(yutovo-calculator REQUIRED)
    get_target_property(_calc_interface_libs yutovo-calculator INTERFACE_LINK_LIBRARIES)
    if(_calc_interface_libs)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(_giac_library "${INSTALL_PATH}/lib/libgiacd.a")
        else()
            set(_giac_library "${INSTALL_PATH}/lib/libgiac.a")
        endif()
        string(REPLACE "giac_imported" "${_giac_library}" _calc_interface_libs "${_calc_interface_libs}")
        set_target_properties(yutovo-calculator PROPERTIES INTERFACE_LINK_LIBRARIES "${_calc_interface_libs}")
    endif()
endif()

add_subdirectory(src)
if((NOT DEFINED EMSCRIPTEN) AND BUILD_TESTS)
    add_subdirectory(test)
endif()
