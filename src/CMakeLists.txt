cmake_minimum_required(VERSION 3.15)

set(MAJOR_VERSION "1" CACHE INTERNAL "Major version")
set(MINOR_VERSION "1" CACHE INTERNAL "Minor version")
set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
project(async VERSION ${PROJECT_VERSION})
configure_file(version.h.in ${PROJECT_SOURCE_DIR}/version.h)

find_package(Threads REQUIRED)

file(GLOB_RECURSE SOURCES *.cpp *.h)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
                                          CXX_STANDARD 17
                                          CXX_STANDARD_REQUIRED ON
    )
else ()
    set_target_properties(${PROJECT_NAME} PROPERTIES
                                          CXX_STANDARD 17
                                          CXX_STANDARD_REQUIRED ON
                                          COMPILE_OPTIONS -Wpedantic -Wall -Wextra
    )
endif ()

target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(${PROJECT_NAME} PUBLIC .)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES "${PROJECT_SOURCE_DIR}/inc/async.h" DESTINATION include)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_CONTACT agar10@yandex.ru)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
include (CPack)
