cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Test)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS PrintSupport REQUIRED)
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/link_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/terms_of_use_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/privacy_policy_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/plot_format_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/export_pdf_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/prompt_form.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/qt_window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/document_window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/command_map.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/document_widget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/qt_pdf_window.cpp
)

add_executable(${PROJECT_NAME}_test
    files.cpp
    ${APP_SOURCES}
)

target_compile_definitions(${PROJECT_NAME}_test PRIVATE TEST_APP)

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
    z
    fontconfig
    yutovo-logger
    yutovo-editor
    yutovo-calculator
    yutovo-solver
)

enable_testing()
add_test(NAME files COMMAND files)
