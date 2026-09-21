cmake_minimum_required(VERSION 3.21)

project(
	MMOHotbar
	VERSION 1.0.0
	DESCRIPTION "SKSE add-on for STB Hotkey System that draws an MMORPG-style hotbar"
	LANGUAGES CXX
)

# ---------------------------------------------------------------------------
# Plugin metadata: substituted into cmake/Version.h.in, cmake/version.rc.in and the
# generated plugin declaration (__MMOHotbarPlugin.cpp).
# ---------------------------------------------------------------------------
set(MMOHOTBAR_AUTHOR "MMO Hotbar" CACHE STRING "Author reported in the plugin declaration.")
set(MMOHOTBAR_DISPLAY_NAME "MMO Hotbar" CACHE STRING "Human readable plugin name; also the name reported to SKSE.")
set(MMOHOTBAR_LOG_NAME "MMOHotbar" CACHE STRING "Log file stem, written to SKSE/<name>.log.")
set(MMOHOTBAR_COPYRIGHT "GPL-3.0-only" CACHE STRING "Copyright notice used by the version resource.")

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(ENABLE_TESTS "Build the MMO Hotbar unit tests (needs Catch2)." ON)
option(MMOHOTBAR_COPY_BUILD "Copy the built plugin and its PDB into MMOHOTBAR_DEPLOY_DIR." OFF)
set(MMOHOTBAR_DEPLOY_DIR "" CACHE PATH "Destination directory for MMOHOTBAR_COPY_BUILD.")

# One DLL for every supported runtime: Skyrim SE 1.5.97, AE 1.6.x / 1.7.x and VR.
# A published tag (not a branch) is what lets CI take the prebuilt bundle instead of
# compiling ~16 minutes of CommonLibSSE-NG source.
set(MMOHOTBAR_COMMONLIB_TAG "v9.0.0" CACHE STRING "CommonLibSSE-NG tag used when COMMONLIB_SSE_FOLDER is unset.")

# ---------------------------------------------------------------------------
# Globals
# ---------------------------------------------------------------------------
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(FATAL_ERROR "In-source builds are not allowed; configure into a build directory.")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)

if(MSVC)
	add_compile_options(/MP)
endif()

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	message(STATUS "No CMAKE_TOOLCHAIN_FILE given: expecting spdlog/rapidcsv/directxtk to be reachable already.")
endif()

# ---------------------------------------------------------------------------
# CommonLibSSE-NG
#
# A local checkout is used when COMMONLIB_SSE_FOLDER (CMake or environment variable) is
# set, otherwise the pinned tag is fetched. Both paths provide the same
# CommonLibSSE::CommonLibSSE target and, once cmake/CommonLibSSE.cmake is included,
# add_commonlibsse_plugin(), which emits the SKSE plugin declaration.
#
# CommonLibSSE-NG bakes its runtime support in at configure time and the prebuilt bundle
# only matches a build that enables all three runtimes. The bundle also refuses to load
# unless the CRT is the dynamic one, so the vcpkg triplet has to be x64-windows (/MD)
# rather than x64-windows-static (/MT).
# ---------------------------------------------------------------------------
if(NOT DEFINED COMMONLIB_SSE_FOLDER AND DEFINED ENV{COMMONLIB_SSE_FOLDER})
	set(COMMONLIB_SSE_FOLDER "$ENV{COMMONLIB_SSE_FOLDER}")
endif()

set(ENABLE_SKYRIM_SE ON CACHE BOOL "" FORCE)
set(ENABLE_SKYRIM_AE ON CACHE BOOL "" FORCE)
set(ENABLE_SKYRIM_VR ON CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)   # CommonLibSSE's own tests, not ours

if(COMMONLIB_SSE_FOLDER AND EXISTS "${COMMONLIB_SSE_FOLDER}/CMakeLists.txt")
	set(MMOHOTBAR_COMMONLIB_SRC "${COMMONLIB_SSE_FOLDER}")
	message(STATUS "CommonLibSSE-NG: local checkout at ${MMOHOTBAR_COMMONLIB_SRC}")
	add_subdirectory("${MMOHOTBAR_COMMONLIB_SRC}" "${CMAKE_BINARY_DIR}/_deps/commonlibsse-build")
else()
	set(MMOHOTBAR_COMMONLIB_SRC "")
	include(FetchContent)
	message(STATUS "CommonLibSSE-NG: fetching ${MMOHOTBAR_COMMONLIB_TAG}")
	FetchContent_Declare(
		CommonLibSSE
		GIT_REPOSITORY https://github.com/alandtse/CommonLibSSE-NG.git
		GIT_TAG "${MMOHOTBAR_COMMONLIB_TAG}"
		GIT_SHALLOW TRUE
	)
	FetchContent_MakeAvailable(CommonLibSSE)
	foreach(candidate IN ITEMS commonlibsse_SOURCE_DIR CommonLibSSE_SOURCE_DIR)
		if(NOT MMOHOTBAR_COMMONLIB_SRC AND DEFINED ${candidate})
			set(MMOHOTBAR_COMMONLIB_SRC "${${candidate}}")
		endif()
	endforeach()
	if(NOT MMOHOTBAR_COMMONLIB_SRC)
		message(FATAL_ERROR "CommonLibSSE-NG was fetched but its source directory could not be determined.")
	endif()
endif()

include("${MMOHOTBAR_COMMONLIB_SRC}/cmake/CommonLibSSE.cmake")

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
file(
	GLOB_RECURSE MMOHOTBAR_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/include/Version.h" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in" "${CMAKE_CURRENT_BINARY_DIR}/version.rc" @ONLY)

# ---------------------------------------------------------------------------
# Plugin target
#
# USE_ADDRESS_LIBRARY without COMPATIBLE_RUNTIMES: the two are mutually exclusive and the
# version list is a strict whitelist, which is exactly how a plugin ends up refusing to
# load on a newer AE release. Declaring address-library independence is what lets one DLL
# load on SE 1.5.97 through AE 1.7.x.
#
# STRUCT_DEPENDENT is deliberately not passed: CommonLibSSE-NG supports both the
# pre-1.6.629 and the post-1.6.629 struct layouts in a single binary, so the plugin
# reports StructCompatibility::Independent and SKSE lets it load on either side.
# ---------------------------------------------------------------------------
add_commonlibsse_plugin(
	${PROJECT_NAME}
	USE_ADDRESS_LIBRARY
	NAME "${MMOHOTBAR_DISPLAY_NAME}"
	AUTHOR "${MMOHOTBAR_AUTHOR}"
	VERSION ${PROJECT_VERSION}
	SOURCES
		${MMOHOTBAR_SOURCES}
		"${CMAKE_CURRENT_BINARY_DIR}/include/Version.h"
		"${CMAKE_CURRENT_BINARY_DIR}/version.rc"
)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		"${CMAKE_CURRENT_BINARY_DIR}/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/src"
		"${CMAKE_CURRENT_SOURCE_DIR}/third_party"
)

# CommonLibSSE.cmake ends with
#   target_include_directories(<target> PUBLIC CommonLibSSE_INCLUDE_DIRS)
# with that token unexpanded, so the line adds a non-existent relative path instead of the
# library's include directory. Today the headers still resolve, because the
# CommonLibSSE::CommonLibSSE target carries its own INTERFACE include directory; adding the
# checkout's include/ explicitly keeps our include path intact if that ever changes.
target_include_directories(${PROJECT_NAME} PRIVATE "${MMOHOTBAR_COMMONLIB_SRC}/include")

target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/PCH.h")

if(MSVC)
	target_compile_options(
		${PROJECT_NAME}
		PRIVATE
			/utf-8
			/permissive-
			/Zc:__cplusplus
			/Zc:preprocessor
			/Zc:externConstexpr
			/Zc:lambda
			/external:anglebrackets
			/external:W0
			/W4
	)

	target_link_options(
		${PROJECT_NAME}
		PRIVATE
			"$<$<CONFIG:Debug>:/INCREMENTAL>"
			"$<$<CONFIG:Release>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>"
	)
endif()

# build/Release/MMOHotbar.dll is what .github/workflows/build.yml uploads, so pin the
# output layout instead of relying on the generator default.
set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
		RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/RelWithDebInfo"
		RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/MinSizeRel"
)

if(MMOHOTBAR_COPY_BUILD)
	if(NOT MMOHOTBAR_DEPLOY_DIR)
		message(FATAL_ERROR "MMOHOTBAR_COPY_BUILD is ON but MMOHOTBAR_DEPLOY_DIR is empty.")
	endif()
	add_custom_command(
		TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory "${MMOHOTBAR_DEPLOY_DIR}"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${PROJECT_NAME}> "${MMOHOTBAR_DEPLOY_DIR}"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PDB_FILE:${PROJECT_NAME}> "${MMOHOTBAR_DEPLOY_DIR}"
	)
endif()

# ---------------------------------------------------------------------------
# Unit tests
#
# The code worth testing (slot assignment, hotkey labels, icon mapping, co-save codec)
# is plain C++ with no RE/SKSE types, so these run without a game process and are
# deliberately not linked against CommonLibSSE.
# ---------------------------------------------------------------------------
file(GLOB MMOHOTBAR_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")

if(ENABLE_TESTS AND MMOHOTBAR_TEST_SOURCES)
	find_package(Catch2 3 CONFIG REQUIRED)
	include(CTest)
	include(Catch)

	add_executable(${PROJECT_NAME}Tests ${MMOHOTBAR_TEST_SOURCES})
	target_include_directories(
		${PROJECT_NAME}Tests
		PRIVATE
			"${CMAKE_CURRENT_SOURCE_DIR}/include"
			"${CMAKE_CURRENT_SOURCE_DIR}/src"
			"${CMAKE_CURRENT_SOURCE_DIR}/third_party"
	)
	target_link_libraries(${PROJECT_NAME}Tests PRIVATE Catch2::Catch2WithMain)
	target_compile_features(${PROJECT_NAME}Tests PRIVATE cxx_std_23)

	if(MSVC)
		target_compile_options(${PROJECT_NAME}Tests PRIVATE /utf-8 /permissive- /W4)
	endif()

	catch_discover_tests(${PROJECT_NAME}Tests)
elseif(ENABLE_TESTS)
	message(STATUS "ENABLE_TESTS is ON but tests/ holds no .cpp files yet; skipping the test target.")
endif()
