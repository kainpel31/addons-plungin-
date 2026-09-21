cmake_minimum_required(VERSION 3.21)

if(POLICY CMP0092)
    cmake_policy(SET CMP0092 NEW)
endif()
if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

option(COPY_BUILD "Copy the build output to the Skyrim directory." TRUE)

macro(set_from_environment VARIABLE)
    if (NOT DEFINED ${VARIABLE} AND DEFINED ENV{${VARIABLE}})
        set(${VARIABLE} $ENV{${VARIABLE}})
    endif ()
endmacro()

set_from_environment(VCPKG_ROOT)
if (NOT DEFINED VCPKG_ROOT)
    message(FATAL_ERROR "Variable VCPKG_ROOT is not set.")
endif ()

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed.")
endif()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "")
set(MMO_HOTBAR_VERSION "1.0.0" CACHE STRING "Project version override")

set(AUTHOR_NAME "Ortriniaz")
set(PRODUCT_NAME "MMOHotbar")
set(BEAUTIFUL_NAME "MMO Hotbar")

project(${PRODUCT_NAME} VERSION ${MMO_HOTBAR_VERSION} LANGUAGES CXX)

if (MSVC)
    add_compile_definitions(_UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
    add_compile_options(/MP)
endif ()

include(FetchContent)

set(BUILD_TESTS OFF CACHE BOOL "Disable tests" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Disable tests" FORCE)
set(ENABLE_TESTS OFF CACHE BOOL "Disable tests" FORCE)

# Mencegah SKSE Menu Framework mencoba memproses target/contoh yang memicu error source_group
set(SMF_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SMF_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SKSEMenuFrameworkSDK
    GIT_REPOSITORY https://github.com/QTR-Modding/SKSE-Menu-Framework-3.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SKSEMenuFrameworkSDK)

find_package(spdlog REQUIRED CONFIG)
find_package(nlohmann_json REQUIRED CONFIG)
find_package(xbyak REQUIRED CONFIG)
find_package(rapidcsv REQUIRED CONFIG)
find_package(nanosvg REQUIRED CONFIG)
find_package(ZLIB REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_sources(${PROJECT_NAME} PRIVATE
    "${sksemenuframeworksdk_SOURCE_DIR}/imgui/imgui.cpp"
    "${sksemenuframeworksdk_SOURCE_DIR}/imgui/imgui_draw.cpp"
    "${sksemenuframeworksdk_SOURCE_DIR}/imgui/imgui_tables.cpp"
    "${sksemenuframeworksdk_SOURCE_DIR}/imgui/imgui_widgets.cpp"
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${sksemenuframeworksdk_SOURCE_DIR}/imgui"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    CommonLibSSE::CommonLibSSE
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    xbyak::xbyak
    nanosvg::nanosvg
    ZLIB::ZLIB
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /sdl /utf-8 /Zi /permissive- /W4 /WX /external:anglebrackets /external:W0)
    target_link_options(${PROJECT_NAME} PRIVATE /WX "$<$<CONFIG:RELEASE>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>")
endif ()
