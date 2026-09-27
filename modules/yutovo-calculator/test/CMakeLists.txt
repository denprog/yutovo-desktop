cmake_minimum_required(VERSION 3.17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

if(WIN32)
	find_package(GTest CONFIG REQUIRED)
else()
	find_package(GTest REQUIRED PATHS ${INSTALL_PATH}/share/gtest)
endif()

add_executable(${PROJECT_NAME}_tests
	main.cpp
	integer.cpp
	real.cpp
	rational.cpp
	complex.cpp
	symbolic_real.cpp
	symbolic_rational.cpp
	symbolic_complex.cpp
	array.cpp
)

if (NOT WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		add_compile_options(-O0 -g -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer)
		target_link_options(${PROJECT_NAME}_tests PRIVATE -fsanitize=undefined -fsanitize=address)
    endif()
endif()

if(WIN32)
	target_link_libraries(${PROJECT_NAME}_tests PRIVATE
		${PROJECT_NAME}
	)
	target_link_libraries(${PROJECT_NAME}_tests PRIVATE GTest::gtest GTest::gmock)
else()
	target_link_libraries(${PROJECT_NAME}_tests PRIVATE
		stdc++
	    pthread
		gtest
		gtest_main
		gmock
		gmock_main
		m
		dl
		${PROJECT_NAME}
	)
endif()

target_include_directories(${PROJECT_NAME}_tests PRIVATE
    ../src
)

if(NOT WIN32)
	include(GoogleTest)
	gtest_discover_tests(${PROJECT_NAME}_tests)
endif()

install(
    TARGETS ${PROJECT_NAME}_tests
    EXPORT ${PROJECT_NAME}
    DESTINATION ${INSTALL_PATH}/bin
    COMPONENT ${PROJECT_NAME}
)
