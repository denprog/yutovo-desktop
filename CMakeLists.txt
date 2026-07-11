cmake_minimum_required(VERSION 3.17)

project(yutovo-desktop LANGUAGES CXX C)

set(PROJECT_LICENSE "GPL-3.0-only")
message(STATUS "CXX Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Project license: ${PROJECT_LICENSE}")

#add_compile_definitions(REMOTE_SOLVER)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()

set(CMAKE_DEBUG_POSTFIX d)

if(WIN32)
    set(INSTALL_PATH "$ENV{VCPKG_ROOT}/installed/x64-windows")
else()
    set(INSTALL_PATH "$ENV{YUTOVO_DEPLOY}")
endif()

#giac imported library required by yutovo-calculator
find_library(GIAC_LIBRARY_RELEASE NAMES giac PATHS ${INSTALL_PATH}/lib NO_DEFAULT_PATH)
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR DEFINED CMAKE_CONFIGURATION_TYPES)
    find_library(GIAC_LIBRARY_DEBUG NAMES giacd giac PATHS ${INSTALL_PATH}/lib NO_DEFAULT_PATH)
else()
    set(GIAC_LIBRARY_DEBUG ${GIAC_LIBRARY_RELEASE})
endif()
if(NOT GIAC_LIBRARY_RELEASE OR NOT GIAC_LIBRARY_DEBUG)
    message(FATAL_ERROR "giac library not found in ${INSTALL_PATH}/lib. Run ./build_giac.sh first.")
endif()
set(GIAC_INCLUDE_DIR ${INSTALL_PATH}/include)

add_library(giac_imported STATIC IMPORTED)
set_target_properties(giac_imported PROPERTIES
    IMPORTED_CONFIGURATIONS "DEBUG;RELEASE"
    INTERFACE_INCLUDE_DIRECTORIES ${GIAC_INCLUDE_DIR}
    IMPORTED_LOCATION_DEBUG ${GIAC_LIBRARY_DEBUG}
    IMPORTED_LOCATION_RELEASE ${GIAC_LIBRARY_RELEASE}
    IMPORTED_LOCATION_RELWITHDEBINFO ${GIAC_LIBRARY_RELEASE}
    IMPORTED_LOCATION_MINSIZEREL ${GIAC_LIBRARY_RELEASE}
    IMPORTED_LOCATION ${GIAC_LIBRARY_RELEASE}
)

option(BUILD_TESTS "Build unit tests" ON)

add_subdirectory(src)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
