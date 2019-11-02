cmake_minimum_required(VERSION 3.4.1)

project(ChickenHook
        VERSION 1.0
        LANGUAGES CXX C
        )


add_subdirectory(src)
add_subdirectory(test)
IF (ANDROID)
    add_subdirectory(androidTest) # with linux we include linuxTest here!
else (ANDROID)
    add_subdirectory(linuxTest)
endif (ANDROID)