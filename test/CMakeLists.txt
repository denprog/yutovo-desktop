cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Test)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS PrintSupport REQUIRED)

if(WIN32)
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /bigobj /EHsc /MP")
    find_package(PkgConfig REQUIRED)
    find_package(Boost REQUIRED COMPONENTS property_tree)
    find_package(BZip2 REQUIRED)
    find_package(RapidJSON CONFIG REQUIRED)
    find_package(unofficial-libharu CONFIG REQUIRED)
    pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)
    pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)
endif()

find_package(yutovo-logger REQUIRED PATHS ${INSTALL_PATH}/lib/cmake/)
find_package(yutovo-editor REQUIRED PATHS ${INSTALL_PATH}/lib/cmake/)
find_package(yutovo-calculator REQUIRED)
find_package(yutovo-solver REQUIRED)

set(APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/link_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/graph_settings_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/terms_of_use_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/privacy_policy_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/plot_format_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/export_pdf_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/about_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/settings_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/system_settings_form.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/interface_settings_form.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/result_settings_form.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/set_uint_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/set_unit_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/colors_settings_form.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/documents_settings_form.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/fonts_settings_form.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/computation_settings_form.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/properties_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/prompt_form.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/whats_new_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/qt_window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/document_window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/command_map.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/document_widget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/qt_pdf_window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/feedback_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/update_available_dialog.cpp
)

add_executable(${PROJECT_NAME}_test
    main.cpp
    files.cpp
    toolbar.cpp
    ${APP_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/resources.qrc
)

target_compile_definitions(${PROJECT_NAME}_test PRIVATE TEST_APP)

if(BUILD_TESTS)
    target_compile_definitions(${PROJECT_NAME}_test PRIVATE BUILD_TESTS)
endif()

target_include_directories(${PROJECT_NAME}_test PRIVATE
    ${INSTALL_PATH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(${PROJECT_NAME}_test PRIVATE
    Qt5::Core
    Qt5::Network
    Qt5::Widgets
    Qt5::Test
    Qt5::PrintSupport
    yutovo-logger
    yutovo-editor
    yutovo-calculator
    yutovo-solver
)

if(WIN32)
    find_library(MGL_LIB NAMES mgld mgl PATHS ${INSTALL_PATH}/debug/lib ${INSTALL_PATH}/lib)
    find_library(ZLIB_LIB NAMES zlibd zlib PATHS ${INSTALL_PATH}/debug/lib ${INSTALL_PATH}/lib)
    target_link_libraries(${PROJECT_NAME}_test PRIVATE
        ${MGL_LIB}
        Boost::property_tree
        BZip2::BZip2
        unofficial::libharu::hpdf
        ${ZLIB_LIB}
    )
else()
    target_link_libraries(${PROJECT_NAME}_test PRIVATE
        z
        fontconfig
    )
endif()

if(QM_FILES)
    add_custom_command(TARGET ${PROJECT_NAME}_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${QM_FILES}
            $<TARGET_FILE_DIR:${PROJECT_NAME}_test>
        COMMENT "Copying .qm files to test output directory"
    )
    add_dependencies(${PROJECT_NAME}_test translate)
endif()

enable_testing()
add_test(NAME files COMMAND ${PROJECT_NAME}_test)
