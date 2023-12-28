cmake_minimum_required(VERSION 3.15)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(async VERSION ${PROJECT_VERSION})
configure_file(version.h.in version.h)

add_library(${PROJECT_NAME} SHARED async.cpp)

add_executable("${PROJECT_NAME}_cli" main.cpp)
target_link_libraries("${PROJECT_NAME}_cli" async)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

install(TARGETS "${PROJECT_NAME}_cli" RUNTIME DESTINATION bin)
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT agar10@yandex.ru)
include(CPack)


