cmake_minimum_required(VERSION 3.15)

file(GLOB_RECURSE SOURCES *.cpp *.h)

set(TARGET_NAME example)

add_executable(${TARGET_NAME} ${SOURCES})

if (MSVC)
    set_target_properties(${TARGET_NAME} PROPERTIES
                                         CXX_STANDARD 17
                                         CXX_STANDARD_REQUIRED ON
    )
else ()
    set_target_properties(${TARGET_NAME} PROPERTIES
                                         CXX_STANDARD 17
                                         CXX_STANDARD_REQUIRED ON
                                         COMPILE_OPTIONS -Wpedantic -Wall -Wextra
    )
endif ()

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(${TARGET_NAME} ${CMAKE_THREAD_LIBS_INIT} async)
