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

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Concurrent ShaderTools Svg)

set(SOURCES
    src/main.cpp
    src/app/Application.cpp
    src/app/AppController.cpp
    src/app/ThemeManager.cpp
    src/app/ImageController.cpp
    src/app/EffectsController.cpp
    src/rendering/Renderer.cpp
    src/ui/main_window/MainWindow.cpp
    src/ui/widgets/ImageView.cpp
    src/ui/widgets/GradientSlider.cpp
    src/ui/widgets/EyeButton.cpp
    src/ui/panels/EffectPanel.cpp
    src/ui/panels/RightPanel.cpp
)

set(HEADERS
    src/app/Application.h
    src/app/AppController.h
    src/app/ThemeManager.h
    src/app/ImageController.h
    src/app/EffectsController.h
    src/core/image/ImageModel.h
    src/core/effects/EffectSettings.h
    src/core/viewport/ViewState.h
    src/rendering/Renderer.h
    src/ui/main_window/MainWindow.h
    src/ui/widgets/ImageView.h
    src/ui/widgets/GradientSlider.h
    src/ui/widgets/EyeButton.h
    src/ui/panels/EffectPanel.h
    src/ui/panels/RightPanel.h
)

add_executable(reblum
    ${SOURCES}
    ${HEADERS}
    resources/resources.qrc
)

qt6_add_shaders(reblum "reblum_shaders"
    GLSL "440"
    PREFIX "/shaders"
    FILES
        assets/shaders/fullscreen.vert
        assets/shaders/image.frag
)

target_include_directories(reblum PRIVATE src ${CMAKE_CURRENT_BINARY_DIR}/generated)

target_link_libraries(reblum PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::GuiPrivate
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Svg
)

if(APPLE)
    set_target_properties(reblum PROPERTIES
        MACOSX_BUNDLE TRUE
    )
endif()
