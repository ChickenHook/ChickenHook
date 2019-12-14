# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

add_executable (macTest main.cpp)

target_link_libraries(macTest
        ${PROJECT_NAME}
        dl
)

target_include_directories(macTest PRIVATE
        ${BEAENGINE_INCLUDE_DIRS}
        )