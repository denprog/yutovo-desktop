cmake_minimum_required(VERSION 3.17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#set(QT_SELECT qt5.14)

find_package(yutovo_editor REQUIRED PATHS ${INSTALL_PATH}/lib/cmake/)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(yutovo_logger REQUIRED PATHS ${INSTALL_PATH}/lib/cmake/)

if (Qt5Widgets_FOUND)
    if (Qt5Widgets_VERSION VERSION_LESS 5.14.0)
        message(FATAL_ERROR "Minimum supported Qt5 version is 5.14")
    endif()
else()
    message(SEND_ERROR "The Qt5Widgets library could not be found!")
endif(Qt5Widgets_FOUND)

add_executable(${PROJECT_NAME}
    main.cpp
    document_widget.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    about.ui
    resources.qrc
    qt_window.cpp
    document_window.cpp
    command_map.cpp
    about_dialog.cpp
    settings_dialog.ui
    settings_dialog.cpp
    system_settings_form.ui
    system_settings_form.cpp
    interface_settings_form.ui
    interface_settings_form.cpp
    result_settings_form.ui
    result_settings_form.cpp
    set_uint_dialog.ui
    set_uint_dialog.cpp
    set_unit_dialog.ui
    set_unit_dialog.cpp
    colors_settings_form.ui
    colors_settings_form.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)

target_include_directories(${PROJECT_NAME} PRIVATE
    ../third_party
    ${INSTALL_PATH}/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    yutovo_editor
    yutovo_logger
    Qt5::Widgets
)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}
    DESTINATION ${INSTALL_PATH}/bin
    COMPONENT ${PROJECT_NAME}
)
