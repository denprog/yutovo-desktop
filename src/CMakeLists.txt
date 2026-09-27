cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

if(WIN32)
    find_package(PkgConfig)
	set(CMAKE_PREFIX_PATH "${INSTALL_PATH}")
    pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /bigobj /EHsc /MP")
elseif(NOT EMSCRIPTEN)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)
    pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)
endif()

if(EMSCRIPTEN)
    set(Boost_INCLUDE_DIR ${INSTALL_PATH}/wasm)
    if (NOT EXISTS ${Boost_INCLUDE_DIR})
        message(FATAL_ERROR "Boost headers not found in ${Boost_INCLUDE_DIR}")
    endif()
    set(GIAC_LIBRARY ${INSTALL_PATH}/wasm/libgiac.a)
    set(GIAC_INCLUDE_DIR ${INSTALL_PATH}/include)
    if (NOT EXISTS ${GIAC_LIBRARY} OR NOT EXISTS ${GIAC_INCLUDE_DIR}/giac/giac.h)
        message(FATAL_ERROR "giac library not found in ${INSTALL_PATH}/wasm. Run ./build_giac.sh first.")
    endif()
    message(STATUS "Found wasm giac: ${GIAC_LIBRARY}")
    add_library(giac_imported STATIC IMPORTED)
    set_target_properties(giac_imported PROPERTIES
        IMPORTED_LOCATION ${GIAC_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${GIAC_INCLUDE_DIR}
    )
else()
    find_package(Boost 1.83.0 REQUIRED)
    find_path(GIAC_INCLUDE_DIR NAMES giac/giac.h PATHS ${INSTALL_PATH}/include NO_DEFAULT_PATH)
    find_library(GIAC_LIBRARY_RELEASE NAMES giac PATHS ${INSTALL_PATH}/lib NO_DEFAULT_PATH)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR DEFINED CMAKE_CONFIGURATION_TYPES)
        find_library(GIAC_LIBRARY_DEBUG NAMES giacd giac PATHS ${INSTALL_PATH}/lib NO_DEFAULT_PATH)
    else()
        set(GIAC_LIBRARY_DEBUG ${GIAC_LIBRARY_RELEASE})
    endif()

    if(NOT GIAC_LIBRARY_RELEASE OR NOT GIAC_LIBRARY_DEBUG)
        message(FATAL_ERROR "giac library not found in ${INSTALL_PATH}/lib. Run ./build_giac.sh first.")
    endif()

    message(STATUS "giac release: ${GIAC_LIBRARY_RELEASE}")
    message(STATUS "giac debug:   ${GIAC_LIBRARY_DEBUG}")

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
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        if (NOT WIN32)
            if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                add_compile_options(-O0 -g -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer)
                add_definitions(-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_EXTENSIVE)
                add_definitions(-D_LIBCPP_ENABLE_ASSERTIONS)
                add_link_options(-fsanitize=undefined)
            elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fhardened -Wimplicit-fallthrough -U_FORTIFY_SOURCE -fstrict-flex-arrays=3 -Wtrampolines \
                    -Werror=format-security -Wno-hardened")
            endif()
        endif()
    elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebugInfo")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    endif()
endif()

if(EMSCRIPTEN)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -s USE_PTHREADS=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s DISABLE_EXCEPTION_THROWING=0")
endif()

set(CMAKE_DEBUG_POSTFIX d)

set(CALCULATOR_SOURCES
    parser.cpp
    expression.cpp
    integer.cpp
    math_helper.cpp
    rational.cpp
    real.cpp
    complex.cpp
    script.cpp
    solver.cpp
    utils.cpp
    giac_utils.cpp
    unit.cpp
    export.cpp

    stdafx.h
    utils.h
    giac_utils.h
    script.h
    solver.h
    unit.h
    real.h
    rational.h
    parser.h
    parser_exception.h
    math_helper.h
    integer.h
    custom_unit.h
    expression.h
    error_handler.h
    definition.h
    complex.h
    symbolic.h
    ast.h
    annotation.h
    export.h
    array.h
    graph.h
)

list(APPEND CALCULATOR_SOURCES symbolic.cpp)

add_library(${PROJECT_NAME} ${CALCULATOR_SOURCES})

if(WIN32)
    set_source_files_properties(solver.cpp PROPERTIES COMPILE_FLAGS "/Zc:inline-")
endif()

if(EMSCRIPTEN)
    target_include_directories(${PROJECT_NAME} PUBLIC 
        ${Boost_INCLUDE_DIR}
        ${INSTALL_PATH}/include
        ${INSTALL_PATH}/include/giac
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        stdc++
        giac_imported
        ${INSTALL_PATH}/wasm/libmpfr.a
        ${INSTALL_PATH}/wasm/libgmp.a
        ${INSTALL_PATH}/wasm/libgmpxx.a
    )
    target_link_options(${PROJECT_NAME} PUBLIC "-fexceptions -s DISABLE_EXCEPTION_THROWING=0")
    target_compile_options(${PROJECT_NAME} PUBLIC -Wno-overloaded-shift-op-parentheses)
else()
    if(WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::mpfr)
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE 
            stdc++
            PkgConfig::mpfr
            PkgConfig::gmp
            giac_imported
            pthread
        )
    endif()
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

if(NOT EMSCRIPTEN)
    target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${GIAC_INCLUDE_DIR}>
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    # ${INSTALL_PATH}/include
    # ${GIAC_INCLUDE_DIR}
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    HAVE_NO_HOME_DIRECTORY
    GIAC_GGB
    IN_GIAC
    GIAC_GENERIC_CONSTANTS
    TIMEOUT
    HAVE_LIBMPFR
    $<$<BOOL:${EMSCRIPTEN}>:HAVE_CONFIG_H>
    $<$<BOOL:${EMSCRIPTEN}>:HAVE_UNISTD_H>
    $<$<BOOL:${EMSCRIPTEN}>:HAVE_LIBPTHREAD>
    $<$<BOOL:${EMSCRIPTEN}>:EMCC2>
    $<$<BOOL:${EMSCRIPTEN}>:gammaf=tgammaf>
    $<$<NOT:$<BOOL:${EMSCRIPTEN}>>:SIZEOF_VOID_P=8>
    $<$<NOT:$<BOOL:${EMSCRIPTEN}>>:HAVE_LIBPTHREAD>
    $<$<NOT:$<BOOL:${EMSCRIPTEN}>>:HAVE_SYS_TIMES_H>
    $<$<NOT:$<BOOL:${EMSCRIPTEN}>>:HAVE_UNISTD_H>
    $<$<NOT:$<BOOL:${EMSCRIPTEN}>>:HAVE_SYS_TIME_H>
    $<$<NOT:$<BOOL:${EMSCRIPTEN}>>:HAVE_SYSCONF>
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<INSTALL_INTERFACE:${INSTALL_PATH}/include>
)

if(EMSCRIPTEN)
    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        DESTINATION ${INSTALL_PATH}/wasm
        COMPONENT ${PROJECT_NAME}
    )
else()
    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        DESTINATION ${INSTALL_PATH}/lib
        COMPONENT ${PROJECT_NAME}
    )
endif()

install(
    FILES 
        annotation.h
        ast.h
        definition.h
        error_handler.h
        expression.h
        integer.h
        math_helper.h
        parser_exception.h
        parser.h
        rational.h
        real.h
        complex.h
        symbolic.h
        array.h
        custom_unit.h
        script.h
        solver.h
        stdafx.h
        utils.h
        giac_utils.h
        unit.h
        export.h
        graph.h
    DESTINATION ${INSTALL_PATH}/include/${PROJECT_NAME}
    COMPONENT ${PROJECT_NAME}
)

if(EMSCRIPTEN)
    install(
        EXPORT ${PROJECT_NAME}
        DESTINATION ${INSTALL_PATH}/wasm/cmake/${PROJECT_NAME}
        COMPONENT ${PROJECT_NAME}
        FILE ${PROJECT_NAME}.cmake
    )
else()
    install(
        EXPORT ${PROJECT_NAME}
        DESTINATION ${INSTALL_PATH}/lib/cmake/${PROJECT_NAME}
        COMPONENT ${PROJECT_NAME}
        FILE ${PROJECT_NAME}.cmake
    )
endif()

if(EMSCRIPTEN)
    install(
        FILES ${PROJECT_NAME}-config.cmake
        DESTINATION ${INSTALL_PATH}/wasm/cmake/${PROJECT_NAME}
        COMPONENT ${PROJECT_NAME}
    )
else()
    install(
        FILES ${PROJECT_NAME}-config.cmake
        DESTINATION ${INSTALL_PATH}/lib/cmake/${PROJECT_NAME}
        COMPONENT ${PROJECT_NAME}
    )
endif()
