cmake_minimum_required(VERSION 3.14)
project(AxUdioEngine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# Перечисляем все модули проекта:
set(SOURCE_FILES
    AxUdioSO.cpp
    AxUdioZxZipl.cpp
    AxUdioDek.cpp
    AxUdioBitrate.cpp
    AxAudioxx.cpp
    AxUdioPipeline.cpp
)

# Создаём ЕДИНЫЙ файл библиотеки (.dll на Windows, .so на Android)
add_library(AxUdioEngine SHARED ${SOURCE_FILES})

if(ANDROID)
    find_library(aaudio-lib aaudio)
    find_library(log-lib log)
    target_link_libraries(AxUdioEngine ${aaudio-lib} ${log-lib})
endif()