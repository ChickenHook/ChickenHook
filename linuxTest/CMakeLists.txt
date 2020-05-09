# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

add_executable (linuxTest main.cpp)

target_link_libraries(linuxTest
        ${PROJECT_NAME}
        dl
        ${BEAENGINE_LIBRARIES}
)

target_include_directories(linuxTest PRIVATE
        ${BEAENGINE_INCLUDE_DIRS}
        )