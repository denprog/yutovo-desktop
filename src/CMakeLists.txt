cmake_minimum_required(VERSION 3.17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(WIN32)
    set(QT_SELECT qt5.15)
    find_package(PkgConfig REQUIRED)
endif()

set(CMAKE_FIND_ROOT_PATH ${INSTALL_PATH}/lib/cmake/)

if(NOT REMOTE_SOLVER)
    find_package(yutovo_calculator REQUIRED)
    find_package(yutovo_solver REQUIRED)
endif()

find_package(yutovo_editor REQUIRED PATHS ${INSTALL_PATH}/lib/cmake/)
find_package(yutovo_logger REQUIRED PATHS ${INSTALL_PATH}/lib/cmake/)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
if(WIN32)
    find_package(Boost REQUIRED COMPONENTS iostreams)
    find_package(RapidJSON CONFIG REQUIRED)
    pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /bigobj")
endif()

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
    documents_settings_form.ui
    documents_settings_form.cpp
    fonts_settings_form.ui
    fonts_settings_form.cpp
    computation_settings_form.ui
    computation_settings_form.cpp
    properties_dialog.ui
    properties_dialog.cpp
    link_dialog.ui
    link_dialog.cpp
    terms_of_use_dialog.ui
    terms_of_use_dialog.cpp
    privacy_policy_dialog.ui
    privacy_policy_dialog.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${THIRD_PARTY_PATH}
    ${INSTALL_PATH}/include
)

if(REMOTE_SOLVER)
    target_link_libraries(${PROJECT_NAME} PUBLIC
        yutovo_editor
        yutovo_logger
        Qt5::Widgets
    )
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC
        yutovo_editor
        yutovo_logger
        yutovo_calculator
        yutovo_solver
        Qt5::Widgets
    )
endif()

if(NOT WIN32)
    install(
        FILES
            translations/${PROJECT_NAME}_en.qm
            translations/${PROJECT_NAME}_ru.qm
            translations/yutovo_editor_en.qm
            translations/yutovo_editor_ru.qm
        DESTINATION ${INSTALL_PATH}/bin
        COMPONENT ${PROJECT_NAME}
    )

    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        DESTINATION ${INSTALL_PATH}/bin
        COMPONENT ${PROJECT_NAME}
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/translations/*.qm ${CMAKE_CURRENT_BINARY_DIR}/../
    )
endif()
