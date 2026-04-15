cmake_minimum_required(VERSION 3.20)
project(reblum VERSION 2.0 LANGUAGES CXX)

set(APP_NAME        "Reblum")
set(APP_ORG         "Reblum")
set(APP_VERSION_STR "${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/config.h
)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(SOURCES
    src/main.cpp
    src/app/Application.cpp
    src/ui/main_window/MainWindow.cpp
)

set(HEADERS
    src/app/Application.h
    src/ui/main_window/MainWindow.h
)

add_executable(reblum
    ${SOURCES}
    ${HEADERS}
    resources/resources.qrc
)

target_include_directories(reblum PRIVATE src ${CMAKE_CURRENT_BINARY_DIR}/generated)

target_link_libraries(reblum PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

if(APPLE)
    set_target_properties(reblum PROPERTIES
        MACOSX_BUNDLE TRUE
    )
endif()
