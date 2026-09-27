cmake_minimum_required(VERSION 3.17)

set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
if(EMSCRIPTEN)
    set(CMAKE_FIND_ROOT_PATH ${INSTALL_PATH}/wasm/cmake/)
endif()

if(WIN32)
	cmake_policy(SET CMP0144 NEW)
	set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)
    find_package(RapidJSON REQUIRED)
    find_package(BZip2 REQUIRED)
endif()

if(NOT DEFINED REMOTE_SOLVER)
    find_package(yutovo-solver REQUIRED PATHS ${INSTALL_PATH}/lib/cmake)
    find_package(yutovo-calculator REQUIRED PATHS ${INSTALL_PATH}/lib/cmake)
    find_package(yutovo-logger REQUIRED PATHS ${INSTALL_PATH}/lib/cmake)
if(NOT DEFINED EMSCRIPTEN)
    find_path(GIAC_INCLUDE_DIR NAMES giac/giac.h PATHS ${INSTALL_PATH}/include NO_DEFAULT_PATH)
    find_library(GIAC_LIBRARY_RELEASE NAMES giac PATHS ${INSTALL_PATH}/lib NO_DEFAULT_PATH)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR DEFINED CMAKE_CONFIGURATION_TYPES)
        find_library(GIAC_LIBRARY_DEBUG NAMES giacd giac PATHS ${INSTALL_PATH}/lib NO_DEFAULT_PATH)
    else()
        set(GIAC_LIBRARY_DEBUG ${GIAC_LIBRARY_RELEASE})
    endif()
    if(NOT GIAC_LIBRARY_RELEASE OR NOT GIAC_LIBRARY_DEBUG OR NOT GIAC_INCLUDE_DIR)
        message(FATAL_ERROR "giac library not found in ${INSTALL_PATH}. Run ./build_giac.sh first.")
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
endif()
endif()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /bigobj /MP")
    pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET GLOBAL mpfr)
else()
    if(NOT EMSCRIPTEN)
        pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)
        pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)
    endif()
    set(Boost_INCLUDE_DIR "${INSTALL_PATH}/deploy/include")
    if (NOT DEFINED EMSCRIPTEN)
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                add_compile_options(-Og -g -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer)
                add_definitions(-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_EXTENSIVE)
                add_definitions(-D_LIBCPP_ENABLE_ASSERTIONS)
                add_link_options(-fsanitize=undefined -fsanitize=address)
            elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fhardened -Wimplicit-fallthrough -U_FORTIFY_SOURCE -fstrict-flex-arrays=3 -Wtrampolines \
                    -Werror=format-security -Wno-hardened")
            endif()
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O2")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
        endif()
    endif()
endif()

if(EMSCRIPTEN)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s DISABLE_EXCEPTION_THROWING=0")
    add_compile_definitions(FMT_CONSTEVAL=)
endif()

add_library(${PROJECT_NAME}
    caret.cpp
    caret_state.cpp
    selection.cpp
    editor_state.cpp
    task.cpp
    document.cpp
    element.cpp
    block.cpp
    paragraph.cpp
    row.cpp
    str.cpp
    image.cpp
    link.cpp
    style.cpp
    text.cpp
    window.cpp
    pdf_window.cpp
    editor_utils.cpp
    result_codes.cpp
    solver.cpp
    solver_task.cpp
    web_socket.cpp
    parser_string.cpp
    undo.cpp
    config.cpp
    formulas/formula.cpp
    formulas/code_block.cpp
    formulas/code_paragraph.cpp
    formulas/code_string.cpp
    formulas/code_row.cpp
    formulas/shape.cpp
    formulas/only_shape_formula.cpp
    formulas/middle_shape_formula.cpp
    formulas/plus.cpp
    formulas/minus.cpp
    formulas/multiply.cpp
    formulas/division.cpp
    formulas/power.cpp
    formulas/nth_root.cpp
    formulas/square_root.cpp
    formulas/equation.cpp
    formulas/result.cpp
    formulas/brackets.cpp
    formulas/assignment.cpp
    formulas/unit.cpp
    formulas/subscript.cpp
    formulas/exclamation.cpp
    formulas/and.cpp
    formulas/or.cpp
    formulas/xor.cpp
    formulas/not.cpp
    formulas/percent.cpp
    formulas/code_column.cpp
    formulas/iteration.cpp
    formulas/sum.cpp
    formulas/product.cpp
    formulas/definite_integral.cpp
    formulas/indefinite_integral.cpp
    formulas/comma.cpp
    formulas/graph.cpp
    formulas/code_paragraphs_block.cpp
    formulas/evaluation_bar.cpp

    caret.h
    caret_state.h
    selection.h
    editor_state.h
    task.h
    document.h
    element.h
    block.h
    paragraph.h
    row.h
    str.h
    image.h
    link.h
    style.h
    text.h
    window.h
    pdf_window.h
    editor_utils.h
    result_codes.h
    solver.h
    solver_task.h
    web_socket.h
    parser_string.h
    undo.h
    config.h
    formulas/formula.h
    formulas/code_block.h
    formulas/code_paragraph.h
    formulas/code_string.h
    formulas/code_row.h
    formulas/shape.h
    formulas/only_shape_formula.h
    formulas/middle_shape_formula.h
    formulas/plus.h
    formulas/minus.h
    formulas/multiply.h
    formulas/division.h
    formulas/power.h
    formulas/nth_root.h
    formulas/square_root.h
    formulas/equation.h
    formulas/result.h
    formulas/brackets.h
    formulas/assignment.h
    formulas/unit.h
    formulas/subscript.h
    formulas/exclamation.h
    formulas/and.h
    formulas/or.h
    formulas/xor.h
    formulas/not.h
    formulas/percent.h
    formulas/code_column.h
    formulas/iteration.h
    formulas/sum.h
    formulas/product.h
    formulas/definite_integral.h
    formulas/indefinite_integral.h
    formulas/comma.h
)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /bigobj /EHsc /MP")
else()
    target_compile_options(${PROJECT_NAME} PUBLIC -Wno-deprecated-declarations)
    target_compile_options(${PROJECT_NAME} PUBLIC -Wno-unused-command-line-argument)
    target_compile_options(${PROJECT_NAME} PUBLIC -Wno-ambiguous-reversed-operator)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC HAVE_LIBMPFR)

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	POSITION_INDEPENDENT_CODE ON
    CMAKE_INCLUDE_CURRENT_DIR ON
)

if(EMSCRIPTEN)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        stdc++
        pthread
    )
    #target_link_options(${PROJECT_NAME} PUBLIC "-fwasm-exceptions -s DISABLE_EXCEPTION_THROWING=0")
    target_link_options(${PROJECT_NAME} PUBLIC "-fexceptions -s DISABLE_EXCEPTION_THROWING=0")
    target_compile_options(${PROJECT_NAME} PUBLIC -Wno-deprecated-literal-operator)
    target_compile_options(${PROJECT_NAME} PUBLIC -Wno-deprecated-anon-enum-enum-conversion)
    target_compile_options(${PROJECT_NAME} PUBLIC -Wno-macro-redefined)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-overloaded-shift-op-parentheses)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-nontrivial-memcall)
elseif(DEFINED REMOTE_SOLVER)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        stdc++
        pthread
        ssl
        crypto
    )
else()
    if(WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE 
            yutovo-solver
            yutovo-logger
        )
        target_link_libraries(${PROJECT_NAME} PRIVATE BZip2::BZip2)
        target_link_libraries(${PROJECT_NAME} PRIVATE yutovo-calculator)
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE 
            stdc++
            pthread
            yutovo-solver
            yutovo-calculator
            yutovo-logger
            ${INSTALL_PATH}/lib/libmgl.a
            ${INSTALL_PATH}/lib/libhpdf.a
    		png
    		z
        )
    endif()
endif()

if(EXISTS "${THIRD_PARTY_PATH}")
    target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${THIRD_PARTY_PATH}>
    )
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    ${INSTALL_PATH}/include
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
        document.h
        caret_state.h
        editor_state.h
        selection.h
        result_codes.h
        caret.h
        element.h
        block.h
        text.h
        solver.h
        solver_task.h
        task.h
        window.h
        pdf_window.h
        style.h
        editor_utils.h
        config.h
        web_socket.h
        parser_string.h
        undo.h
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
