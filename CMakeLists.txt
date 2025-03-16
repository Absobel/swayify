cmake_minimum_required(VERSION 3.16)
project(swayify C)


set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-protocols)
pkg_check_modules(CAIRO cairo)

if(NOT CAIRO_FOUND)
    include(ExternalProject)
    set(CAIRO_PREFIX ${CMAKE_BINARY_DIR}/external/cairo)
    ExternalProject_Add(cairo_external
        GIT_REPOSITORY https://gitlab.freedesktop.org/cairo/cairo.git
        GIT_TAG 1.16.0
        PREFIX ${CAIRO_PREFIX}
        CONFIGURE_COMMAND ${CAIRO_PREFIX}/src/cairo_external/configure --prefix=${CAIRO_PREFIX} --enable-static --disable-shared
        BUILD_COMMAND make -j4
        INSTALL_COMMAND make install
    )
    set(CAIRO_INCLUDE_DIRS ${CAIRO_PREFIX}/include)
    set(CAIRO_LIBRARIES
        ${CAIRO_PREFIX}/lib/libcairo.a
        ${CAIRO_PREFIX}/lib/libpixman-1.a)
    
    set(CAIRO_DEPENDS cairo_external)
endif()

add_executable(swayify src/swayify.c)

target_include_directories(swayify PRIVATE 
    ${WAYLAND_INCLUDE_DIRS}
    ${CAIRO_INCLUDE_DIRS}
)

target_link_libraries(swayify PRIVATE 
    ${WAYLAND_LIBRARIES}
    ${CAIRO_LIBRARIES}
    m # Math library
)

if(DEFINED CAIRO_DEPENDS)
    add_dependencies(swayify ${CAIRO_DEPENDS})
endif()