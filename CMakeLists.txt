cmake_minimum_required(VERSION 3.4.0)

project(StateOfDecay2Helper VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_STATIC "Use static MFC library" OFF)

message(STATUS "Use static MFC library: ${USE_STATIC}")

if(USE_STATIC)
    set(CMAKE_MFC_FLAG 1)
else(USE_STATIC)
    set(CMAKE_MFC_FLAG 2)
endif(USE_STATIC)

find_package(Tesseract REQUIRED)

add_executable(${PROJECT_NAME} MainApp.cpp MainFrame.cpp main.manifest)

if(USE_STATIC)
    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
else(USE_STATIC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _AFXDLL)
endif(USE_STATIC)

set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE true)
target_compile_definitions(${PROJECT_NAME} PUBLIC UNICODE _UNICODE)
target_link_libraries(${PROJECT_NAME} PRIVATE Tesseract::libtesseract)
