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

find_package(PkgConfig REQUIRED)

if(WIN32)
	set(CMAKE_PREFIX_PATH "${INSTALL_PATH}")
endif()

if(NOT REMOTE_SOLVER)
    find_package(yutovo-calculator REQUIRED)
    find_package(yutovo-solver REQUIRED)
endif()

find_package(yutovo-editor REQUIRED PATHS ${INSTALL_PATH}/lib/cmake/)
find_package(yutovo-logger REQUIRED PATHS ${INSTALL_PATH}/lib/cmake/)

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

add_executable(${PROJECT_NAME} WIN32 
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

if(WIN32)
    set(APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/images/mainicon.ico")
    set(APP_RC_FILE "${CMAKE_CURRENT_BINARY_DIR}/icon.rc")
    file(WRITE "${APP_RC_FILE}" "1 ICON \"${APP_ICON_RESOURCE}\"\n")
    target_sources(${PROJECT_NAME} PRIVATE "${APP_RC_FILE}")
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

if(NOT WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE -fPIC -static-libstdc++)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${INSTALL_PATH}/include
)

if(REMOTE_SOLVER)
    target_link_libraries(${PROJECT_NAME} PUBLIC
        yutovo-editor
        yutovo-logger
        Qt5::Widgets
		libboost_iostreams.a
        z
    )
elseif(NOT WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC
        libstdc++.a
        yutovo-editor
        yutovo-logger
        yutovo-calculator
        yutovo-solver
        Qt5::Widgets
		libboost_iostreams.a
        z
    )
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC
        yutovo-editor
        yutovo-logger
        yutovo-calculator
        yutovo-solver
        Qt5::Widgets
    )
endif()

set(TS_FILES
    translations/yutovo-editor_ru.ts
    translations/yutovo-desktop_ru.ts
    translations/yutovo-editor_en.ts
    translations/yutovo-desktop_en.ts
)

find_program(LRELEASE_EXECUTABLE NAMES lrelease lrelease-qt5 lrelease-qt6)

if(NOT LRELEASE_EXECUTABLE)
    message(FATAL_ERROR "lrelease not found.")
endif()

set(QM_FILES)
foreach(TS_FILE ${TS_FILES})
    get_filename_component(TS_NAME ${TS_FILE} NAME_WE)
    set(QM_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TS_NAME}.qm)
    list(APPEND QM_FILES ${QM_FILE})

    add_custom_command(
        OUTPUT ${QM_FILE}
        COMMAND ${LRELEASE_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${TS_FILE} -qm ${QM_FILE}
        DEPENDS ${TS_FILE}
        COMMENT "Generating ${QM_FILE}"
        VERBATIM
    )
endforeach()

add_custom_target(translate ALL
    DEPENDS ${QM_FILES}
)

if(NOT WIN32)
    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        DESTINATION ${INSTALL_PATH}/bin
        COMPONENT ${PROJECT_NAME}
    )
endif()
