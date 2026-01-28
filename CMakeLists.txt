cmake_minimum_required(VERSION 3.20)
project(Kadence LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)

add_subdirectory(src/libkadence)
add_subdirectory(src/daemon)
add_subdirectory(src/ui)
add_subdirectory(src/overlay)
