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
endif ()


######## DOWNLOAD && CONFIGURE BEA

# Download and unpack googletest at configure time
configure_file(${CMAKE_SOURCE_DIR}/beaengine-configure/CMakeLists.txt.in ${CMAKE_SOURCE_DIR}/beaengine/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "Unix Makefiles" .
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/beaengine"
        )
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/beaengine"
        )

include_directories(
        "${CMAKE_SOURCE_DIR}/beaengine/beaengine-src/include"
        "${CMAKE_SOURCE_DIR}/beaengine/beaengine-src/src/Includes"
)

set(BEA_TARGET BeaEngine)
# Add googletest directly to our build. This adds the following targets:
# gtest, gtest_main, gmock and gmock_main
add_subdirectory("${CMAKE_SOURCE_DIR}/beaengine/beaengine-src"
        "${CMAKE_SOURCE_DIR}/beaengine/beaengine-build"
        )


set(BEAENGINE_LIBRARIES ${CMAKE_BINARY_DIR}/beaengine/beaengine-src/src/libBeaEngine.a)
set(BEAENGINE_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/beaengine/beaengine-src/include"
        "${CMAKE_SOURCE_DIR}/beaengine/beaengine-src/src/Includes")


add_subdirectory("${CMAKE_SOURCE_DIR}/beaengine/beaengine-src/src")

install(TARGETS ${BEA_TARGET}
        ARCHIVE
        DESTINATION ${CMAKE_SOURCE_DIR}/artifactsOut/${CMAKE_LIBRARY_ARCHITECTURE}/lib/
        COMPONENT Libraries)
install(DIRECTORY ${BEAENGINE_INCLUDE_DIRS} DESTINATION ${CMAKE_SOURCE_DIR}/artifactsOut/${CMAKE_LIBRARY_ARCHITECTURE}/include)


# Now simply link your own targets against gtest, gmock,
# etc. as appropriate

#####################


add_subdirectory(src)
IF (ANDROID)
    add_subdirectory(androidTest)
elseif (LINUX)
    add_subdirectory(linuxTest)
elseif (MACOSX)
    add_subdirectory(macTest)
elseif (WINDOWS)
    add_subdirectory(windowsTest)
endif (ANDROID)

