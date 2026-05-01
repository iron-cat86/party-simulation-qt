cmake_minimum_required(VERSION 3.14)

project(PartySimulationUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем автоматическую генерацию файлов для сигналов и слотов (MOC), ресурсов и форм
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(PartySimulationUI
    main.cpp
    mainwindow.cpp
    mainwindow.h
    simulation.cpp
    simulation.h
)

target_link_libraries(PartySimulationUI PRIVATE Qt5::Widgets)

