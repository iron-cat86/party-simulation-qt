cmake_minimum_required(VERSION 3.14)

project(PartySimulationUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем автоматическую генерацию файлов для сигналов и слотов (MOC), ресурсов и форм
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Gui REQUIRED)
qt5_add_resources(PartySimulationUI_RESOURCES app.qrc)

add_executable(PartySimulationUI
    main.cpp
    mainwindow.cpp
    mainwindow.h
    simulation.cpp
    simulation.h
    simulationwidget.cpp
    simulationwidget.h
    app.cpp
    app.h
    qrc_app.cpp
    ${PartySimulationUI_RESOURCES}
)

target_link_libraries(PartySimulationUI PRIVATE Qt5::Widgets Qt5::Gui)

find_package(GTest REQUIRED)

add_executable(AppTests
    tests/app_tests.cpp
    app.cpp
    app.h
    simulation.cpp
    simulation.h
    qrc_app.cpp
    ${PartySimulationUI_RESOURCES}
)
target_link_libraries(AppTests GTest::gtest_main Qt5::Core Qt5::Widgets Qt5::Gui)

