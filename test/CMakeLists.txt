cmake_minimum_required(VERSION 3.15)

# Setup testing
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()



include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)



###add_subdirectory(googletest EXCLUDE_FROM_ALL)


add_executable(tests test_main.cpp
                     ${CMAKE_SOURCE_DIR}/src/Bulk.cpp
                     ${CMAKE_SOURCE_DIR}/src/CmdInterpreter.cpp
                     ${CMAKE_SOURCE_DIR}/src/CmdProcessContext.cpp
                     ${CMAKE_SOURCE_DIR}/src/ver.cpp
                     ${CMAKE_SOURCE_DIR}/inc/Bulk.h
                     ${CMAKE_SOURCE_DIR}/inc/CmdInterpreter.h
                     ${CMAKE_SOURCE_DIR}/inc/CmdProcessContext.h
                     ${CMAKE_SOURCE_DIR}/inc/IObservable.h
                     ${CMAKE_SOURCE_DIR}/inc/IStreamWriter.h
                     ${CMAKE_SOURCE_DIR}/inc/ver.h)

target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/inc)

if (MSVC)
    set_target_properties(tests PROPERTIES
                                CXX_STANDARD 17
                                CXX_STANDARD_REQUIRED ON
    )
else ()
    set_target_properties(tests PROPERTIES
                                CXX_STANDARD 17
                                CXX_STANDARD_REQUIRED ON
                                COMPILE_OPTIONS -Wpedantic -Wall -Wextra
    )
endif ()

target_link_libraries(tests gtest)
target_include_directories(tests PRIVATE "${CMAKE_PROJECT_DIR}/inc")

enable_testing()

add_test(ver_test_case tests)
add_test(context_test_case tests)
add_test(bulk_test_case tests)
