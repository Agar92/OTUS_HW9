cmake_minimum_required(VERSION 3.15)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(async VERSION ${PROJECT_VERSION})
configure_file(version.h.in version.h)

find_package(Threads REQUIRED)

add_library(async SHARED
  async.hpp
  handler.cpp
  handler.hpp
)

add_executable(main main.cpp)

set_target_properties(async PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  PUBLIC_HEADER async.hpp
)

set_target_properties(main PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

target_include_directories(async
  PRIVATE "${CMAKE_BINARY_DIR}" .
)

target_link_libraries(async
	${CMAKE_THREAD_LIBS_INIT}
)

target_include_directories(main
  PRIVATE "${CMAKE_BINARY_DIR}" .
)

target_link_libraries(main PRIVATE
  async
)

if (MSVC)
  target_compile_options(async PRIVATE
    /W4
  )
  target_compile_options(main PRIVATE
    /W4
  )
else ()
  target_compile_options(async PRIVATE
    -Wall -Wextra -pedantic -Wl,--no-as-needed -lpthread
  )
  target_compile_options(main PRIVATE
    -Wall -Wextra -pedantic -Wl,--no-as-needed -lpthread
  )
endif()

include(GNUInstallDirs)
install(
  TARGETS async
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(TARGETS main RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT agar10@yandex.ru)

include(CPack)
