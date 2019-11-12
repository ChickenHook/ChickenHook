cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(ChickenHook
        VERSION 1.0
        LANGUAGES CXX C
        )
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()

add_subdirectory(src)
IF (ANDROID)
    add_subdirectory(androidTest)
elseif (LINUX)
    add_subdirectory(linuxTest)
elseif (MACOSX)
    add_subdirectory(macTest)
elseif(WINDOWS)
    add_subdirectory(windowsTest)
endif (ANDROID)