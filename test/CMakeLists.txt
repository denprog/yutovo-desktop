cmake_minimum_required(VERSION 3.17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
	set(CMAKE_PREFIX_PATH "${INSTALL_PATH}")
else()
	find_package(GTest)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)
	pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)
endif()

enable_testing()
find_package(GTest REQUIRED PATHS ${INSTALL_PATH}/share/gtest)

if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /bigobj")
	find_package(PkgConfig)
	find_package(ZLIB REQUIRED)
	find_package(yutovo-calculator REQUIRED PATHS ${INSTALL_PATH}/lib/cmake/)
	find_package(Boost 1.81.0 REQUIRED PATHS ${INSTALL_PATH}/lib/cmake/)
	find_package(RapidJSON CONFIG REQUIRED)
	find_package(Boost REQUIRED COMPONENTS iostreams)
endif()

if(EMSCRIPTEN)
    set(CMAKE_FIND_ROOT_PATH ${INSTALL_PATH}/wasm/cmake/)
else()
    set(CMAKE_FIND_ROOT_PATH ${INSTALL_PATH}/lib/cmake/)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(yutovo-logger REQUIRED PATHS ${INSTALL_PATH}/lib/cmake/)

if(NOT DEFINED REMOTE_SOLVER)
    find_package(yutovo-solver REQUIRED)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
else()
    add_compile_definitions(RELEASE)
endif()

find_package(ZLIB REQUIRED)

add_executable(${PROJECT_NAME}_tests
	main.cpp
	mock.cpp
	str.cpp
	link.cpp
	paragraph.cpp
	caret.cpp
	files.cpp
	clipboard.cpp
	code.cpp
	plus.cpp
	minus.cpp
	multiply.cpp
	division.cpp
	power.cpp
	nth_root.cpp
	square_root.cpp
	brackets.cpp
	formulas.cpp
	solver_auto.cpp
	solver_real.cpp
	solver_integer.cpp
	solver_rational.cpp
	solver_complex.cpp
	solver_array_real.cpp
	solver_symbolic.cpp
	assignment.cpp
	unit.cpp
	functions.cpp
	subscript.cpp
	variables.cpp
	image.cpp
	sum.cpp
	product.cpp
	definite_integral.cpp
	indefinite_integral.cpp
	derivative.cpp
	evaluation_bar.cpp
	solver_derivative.cpp
	graph.cpp
	arrays.cpp
	pdf.cpp
)

if (NOT WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		add_compile_options(-Og -g -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer)
        add_definitions(-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_EXTENSIVE)
        add_definitions(-D_LIBCPP_ENABLE_ASSERTIONS)
		target_link_options(${PROJECT_NAME}_tests PRIVATE -fsanitize=undefined -fsanitize=address)
    endif()
endif()

if(WIN32)
	target_link_libraries(${PROJECT_NAME}_tests PUBLIC
		${PROJECT_NAME}
	    Qt5::Widgets
		yutovo-solver
	    yutovo-logger
	)
	target_link_libraries(${PROJECT_NAME}_tests PRIVATE yutovo-calculator)
	target_link_libraries(${PROJECT_NAME} PRIVATE Boost::iostreams)
	target_link_libraries(${PROJECT_NAME}_tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
    target_include_directories(${PROJECT_NAME}_tests PRIVATE
        ../src
    	${INSTALL_PATH}/include
	)
else()
	target_link_libraries(${PROJECT_NAME}_tests PUBLIC
		stdc++
	    pthread
		gtest
		gtest_main
		gmock
		gmock_main
		m
		dl
		${PROJECT_NAME}
	    Qt5::Widgets
	    yutovo-logger
		libboost_iostreams.a
		poppler-cpp
		fontconfig
	)
    target_include_directories(${PROJECT_NAME}_tests PRIVATE
        ../src
	)
endif()

if(NOT WIN32)
    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME}_tests)
endif()

#the calculator worker executable must always sit next to the main executable, the solver looks it up there first
if(NOT REMOTE_SOLVER)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(YUTOVO_SOLVER_WORKER_BIN "${INSTALL_PATH}/bin/yutovo-solver-calculator-workerd")
    else()
        set(YUTOVO_SOLVER_WORKER_BIN "${INSTALL_PATH}/bin/yutovo-solver-calculator-worker")
    endif()
    if(WIN32)
        set(YUTOVO_SOLVER_WORKER_BIN "${YUTOVO_SOLVER_WORKER_BIN}.exe")
    endif()
    if(EXISTS "${YUTOVO_SOLVER_WORKER_BIN}")
        add_custom_command(TARGET ${PROJECT_NAME}_tests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${YUTOVO_SOLVER_WORKER_BIN}" "$<TARGET_FILE_DIR:${PROJECT_NAME}_tests>"
        )
    else()
        message(WARNING "yutovo-solver-calculator-worker not found in ${INSTALL_PATH}/bin, run make install in yutovo-solver first")
    endif()
endif()

install(
    TARGETS ${PROJECT_NAME}_tests
    EXPORT ${PROJECT_NAME}
    DESTINATION ${INSTALL_PATH}/bin
    COMPONENT ${PROJECT_NAME}
)
