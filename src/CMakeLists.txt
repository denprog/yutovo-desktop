cmake_minimum_required(VERSION 3.17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#set(QT_SELECT qt5.14)

find_package(yutovo_editor REQUIRED PATHS ${INSTALL_PATH}/cmake/)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

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
    command_map.cpp
    about_dialog.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)

target_include_directories(${PROJECT_NAME} PRIVATE
    ../third_party
    ${INSTALL_PATH}/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    yutovo_editor
    Qt5::Widgets
)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}
    DESTINATION ${INSTALL_PATH}/bin
    COMPONENT ${PROJECT_NAME}
)
