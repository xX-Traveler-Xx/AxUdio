cmake_minimum_required(VERSION 3.14)
project(AxUdioEngine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# Полный список ваших файлов из обозревателя решений:
set(SOURCE_FILES
    AxUdioCore.h
    AxAudioxx.cpp
    
    AxUdioBitrate.cpp
    AxUdioDek.cpp
    AxUdioPipeline.cpp
    AxUdioSO.cpp
    AxUdioZxZipl.cpp
)

add_library(AxUdioEngine SHARED ${SOURCE_FILES})

target_include_directories(AxUdioEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(ANDROID)
    find_library(aaudio-lib aaudio)
    find_library(log-lib log)
    target_link_libraries(AxUdioEngine ${aaudio-lib} ${log-lib})
endif()