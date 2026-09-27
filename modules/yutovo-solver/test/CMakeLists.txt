cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(WIN32)
    set(CMAKE_PREFIX_PATH "${INSTALL_PATH}")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)
    pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)
    pkg_check_modules(gtest REQUIRED gtest)
endif()

enable_testing()

if(EMSCRIPTEN)
    set(CMAKE_FIND_ROOT_PATH ${INSTALL_PATH}/wasm/cmake/)
else()
    set(CMAKE_FIND_ROOT_PATH ${INSTALL_PATH}/lib/cmake/)
endif()

find_package(yutovo-calculator REQUIRED)
find_package(yutovo-logger REQUIRED)

add_executable(${PROJECT_NAME}_tests
    main.cpp
    mock.cpp
    calculator.cpp
    session.cpp
    result_types.cpp
    solvers.cpp
    exceptions.cpp
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME}_tests PUBLIC
        ${PROJECT_NAME}
        yutovo-calculator
        yutovo-logger
    )
    target_link_libraries(${PROJECT_NAME}_tests PRIVATE GTest::gtest GTest::gtest_main)
    target_include_directories(${PROJECT_NAME}_tests PRIVATE
        ../src
        ${INSTALL_PATH}/include
    )
else()
    target_link_libraries(${PROJECT_NAME}_tests PUBLIC
        ${PROJECT_NAME}
        yutovo-calculator
        yutovo-logger
        gtest
        gtest_main
        pthread
        dl
    )
    target_include_directories(${PROJECT_NAME}_tests PRIVATE
        ../src
        ${gtest_INCLUDE_DIRS}
    )
endif()

if(NOT WIN32)
    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME}_tests)
endif()

#a worker build that cannot interrupt its own giac evaluations, used by tests of the parent kill path
if(NOT EMSCRIPTEN AND NOT REMOTE_MODE)
    #the postfix variable lives in the src directory scope, mirror it for this target
    set(WORKER_TEST_DEBUG_POSTFIX d)
    add_executable(${PROJECT_NAME}-calculator-worker-test
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/calculator_worker.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/service_solver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/service_config.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/solver_process.cpp
    )

    set_target_properties(${PROJECT_NAME}-calculator-worker-test PROPERTIES DEBUG_POSTFIX ${WORKER_TEST_DEBUG_POSTFIX})
    target_compile_definitions(${PROJECT_NAME}-calculator-worker-test PRIVATE YUTOVO_SOLVER_TEST_NO_INTERRUPT)

    if(WIN32)
        find_package(Boost REQUIRED COMPONENTS iostreams)
        target_link_libraries(${PROJECT_NAME}-calculator-worker-test PRIVATE
            yutovo-calculator
            yutovo-logger
            Boost::iostreams
        )
    else()
        target_link_libraries(${PROJECT_NAME}-calculator-worker-test PRIVATE
            stdc++
            stdc++fs
            pthread
            yutovo-calculator
            yutovo-logger
        )
    endif()
    
    target_include_directories(${PROJECT_NAME}-calculator-worker-test PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        $<$<CONFIG:Debug>:${INSTALL_PATH}/include>
        $<$<CONFIG:Release>:${INSTALL_PATH}/include>
    )
endif()

install(
    TARGETS ${PROJECT_NAME}_tests
    EXPORT ${PROJECT_NAME}
    DESTINATION ${INSTALL_PATH}/bin
    COMPONENT ${PROJECT_NAME}
)
