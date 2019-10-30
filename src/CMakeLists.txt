# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source _original_code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        ${PROJECT_NAME}

        # Sets the library as a shared library.
        STATIC

        # Provides a relative path to your source file(s).
        chickenhook.cpp
        trampoline.cpp)

target_include_directories(${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/chickenHook>
        $<INSTALL_INTERFACE:include/chickenHook>  # <prefix>/include/mylib
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        include/
        )