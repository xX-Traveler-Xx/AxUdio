cmake_minimum_required(VERSION 3.15)
project(AxUdio CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(AxUdioEngine SHARED
    AxUdioCore.h
    AxUdioSO.cpp
    AxUdioZxZipl.cpp
    AxUdioDek.cpp
    AxUdioBitrate.cpp
    AxUdioPipeline.cpp
    AxAudioxx.cpp
)

# Вот эта строка обязана быть для DLL на Windows:
target_compile_definitions(AxUdioEngine PRIVATE AXUDIO_EXPORTS)

if(ANDROID)
    find_library(aaudio-lib aaudio)
    find_library(log-lib log)
    target_link_libraries(AxUdioEngine ${aaudio-lib} ${log-lib})
endif()