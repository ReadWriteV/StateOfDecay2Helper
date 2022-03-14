cmake_minimum_required(VERSION 3.0.0)

project(StateOfDecay2Helper VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Tesseract 5.0.0 REQUIRED)

add_executable(${PROJECT_NAME} main.cpp main_window.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE true)
target_compile_definitions(${PROJECT_NAME} PUBLIC UNICODE _UNICODE)
target_link_libraries(${PROJECT_NAME} PRIVATE Tesseract::libtesseract)
