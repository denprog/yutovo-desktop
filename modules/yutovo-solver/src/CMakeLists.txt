cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(WIN32)
    set(SPDLOG_FMT_EXTERNAL ON)
endif()

if(EMSCRIPTEN)
    set(CMAKE_FIND_ROOT_PATH ${INSTALL_PATH}/wasm/cmake/)
elseif(NOT WIN32)
    set(CMAKE_FIND_ROOT_PATH ${INSTALL_PATH}/lib/cmake/)
endif()

if(WIN32)
	set(CMAKE_PREFIX_PATH "${INSTALL_PATH}")
endif()

find_package(yutovo-calculator REQUIRED)
find_package(yutovo-logger REQUIRED)

if(WIN32)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)
    find_package(RapidJSON CONFIG REQUIRED)
    #solver_process.cpp uses boost::process for the worker process
    find_package(Boost REQUIRED COMPONENTS iostreams)
elseif(NOT EMSCRIPTEN)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)
    pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)
endif()

if(REMOTE_MODE)
    add_executable(${PROJECT_NAME}
        main.cpp
        session.cpp
        service_solver.cpp
        service_config.cpp
        solver_process.cpp
    )
else()
    add_library(${PROJECT_NAME}
        session.cpp
        service_solver.cpp
        service_config.cpp
        solver_process.cpp
    )
endif()

set(CMAKE_DEBUG_POSTFIX d)

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /bigobj /EHsc /MP")
else()
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
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
        endif()
    endif()
    target_compile_options(${PROJECT_NAME} PUBLIC -Wno-deprecated-declarations)
endif()

set(CMAKE_DEBUG_POSTFIX d)

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
    target_link_options(${PROJECT_NAME} PUBLIC "-fexceptions -s DISABLE_EXCEPTION_THROWING=0")
    target_compile_options(${PROJECT_NAME} PUBLIC -Wno-ambiguous-reversed-operator)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES 
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

#the worker process that owns the calculator parsers, not built for emscripten
if(NOT EMSCRIPTEN AND NOT REMOTE_MODE)
    add_executable(${PROJECT_NAME}-calculator-worker
        calculator_worker.cpp
        service_solver.cpp
        service_config.cpp
        solver_process.cpp
    )

    set_target_properties(${PROJECT_NAME}-calculator-worker PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    
    if(WIN32)
        target_link_libraries(${PROJECT_NAME}-calculator-worker PRIVATE
            yutovo-calculator
            yutovo-logger
            Boost::iostreams
        )
    else()
        target_link_libraries(${PROJECT_NAME}-calculator-worker PRIVATE
            stdc++
            stdc++fs
            pthread
            yutovo-calculator
            yutovo-logger
        )
    endif()
    target_include_directories(${PROJECT_NAME}-calculator-worker PUBLIC
        $<$<CONFIG:Debug>:${INSTALL_PATH}/include>
        $<$<CONFIG:Release>:${INSTALL_PATH}/include>
    )
    install(
        TARGETS ${PROJECT_NAME}-calculator-worker
        DESTINATION ${INSTALL_PATH}/bin
        COMPONENT ${PROJECT_NAME}
    )
endif()

if(REMOTE_MODE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        stdc++
        stdc++fs
        pthread
        ssl
        crypto
        yutovo-calculator
        yutovo-logger
    )
else()
    if(WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE
            yutovo-calculator
            yutovo-logger
            Boost::iostreams
        )
        target_link_libraries(${PROJECT_NAME} PRIVATE RapidJSON rapidjson)
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE
            stdc++
            pthread
            yutovo-calculator
            yutovo-logger
        )
    endif()
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:Debug>:${INSTALL_PATH}/include>
    $<$<CONFIG:Release>:${INSTALL_PATH}/include>
)

if(EMSCRIPTEN)
    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        DESTINATION ${INSTALL_PATH}/wasm
        COMPONENT ${PROJECT_NAME}
    )
elseif(REMOTE_MODE)
    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        DESTINATION ${INSTALL_PATH}/bin
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
        service_context.h
        session.h
        service_config.h
        types.h
        service_solver.h
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

if(NOT WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../keys/yutovo-solver.crt ${CMAKE_CURRENT_BINARY_DIR}/../
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../keys/yutovo-solver.key ${CMAKE_CURRENT_BINARY_DIR}/../
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../keys/yutovo-solver.pub ${CMAKE_CURRENT_BINARY_DIR}/../
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/yutovo-solver.ini ${CMAKE_CURRENT_BINARY_DIR}/../
    )
endif()
