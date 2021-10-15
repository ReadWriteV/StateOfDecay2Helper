cmake_minimum_required(VERSION 3.19)

project(StateOfDecay2Helper VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Tesseract 5.0.0 REQUIRED)

add_executable(${PROJECT_NAME} WIN32 main.cpp widget.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Tesseract::libtesseract)
