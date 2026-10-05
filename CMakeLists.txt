cmake_minimum_required(VERSION 3.20)
set(NAME "ExamplePlugin" CACHE STRING "")
set(VERSION 1.0.0 CACHE STRING "")
set(RUNTIME "SE" CACHE STRING "")

option(COPY_BUILD "Copy the build output to the mods folder." TRUE)
option(BUILD_SKYRIMAE "Build for Skyrim AE" OFF)

macro(set_from_environment VARIABLE)
	if (NOT DEFINED ${VARIABLE} AND DEFINED ENV{${VARIABLE}})
		set(${VARIABLE} $ENV{${VARIABLE}})
	endif ()
endmacro()

set_from_environment(VCPKG_ROOT)
set_from_environment(CommonLibSSEPath)
set_from_environment(SKYRIM_MODS_FOLDER)

if(BUILD_SKYRIMAE)
	add_compile_definitions(SKYRIM_AE)
	add_compile_definitions(SKYRIM_SUPPORT_AE)
endif()

message(
	STATUS
	"Building ${NAME} ${VERSION} for ${RUNTIME} with CommonLibSSE at ${CommonLibSSEPath}."
)

if (DEFINED VCPKG_ROOT)
	set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
	set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "")
else ()
	message(
		WARNING
		"Variable VCPKG_ROOT is not set. Continuing without vcpkg."
	)
endif ()

if(EXISTS "${CommonLibSSEPath}/cmake/ports/commonlib-shared/portfile.cmake")
	set(VCPKG_OVERLAY_PORTS "${CommonLibSSEPath}/cmake/ports")
else()
	message(FATAL_ERROR "commonlib-shared overlay port not found in ${CommonLibSSEPath}")
endif()

list(APPEND VCPKG_OVERLAY_PORTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ports")

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "")

project(
	${NAME}
	VERSION ${VERSION}
	LANGUAGES CXX
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/Version.h.in
	${CMAKE_CURRENT_BINARY_DIR}/include/Version.h
	@ONLY
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
	${CMAKE_CURRENT_BINARY_DIR}/version.rc
	@ONLY
)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(
		FATAL_ERROR
			"In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
	)
endif()

if (MSVC)
	if (NOT ${CMAKE_GENERATOR} STREQUAL "Ninja")
		add_compile_options(
			/MP
		)
	endif ()
endif ()

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)

add_subdirectory(${CommonLibSSEPath} CommonLibSSE)

find_path(SKSE_MCP_INCLUDE_DIRS "SKSEMCP/utils.hpp")
find_path(FLICK_API_INCLUDE_DIRS "FUCK_API.h")

file(GLOB_RECURSE headers CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

source_group(
	TREE
		${CMAKE_CURRENT_SOURCE_DIR}
	FILES
		${headers}
		${sources}
)

source_group(
	TREE
		${CMAKE_CURRENT_BINARY_DIR}
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/include/Version.h
)

add_library(
	${PROJECT_NAME}
	SHARED
	${headers}
	${sources}
	${CMAKE_CURRENT_BINARY_DIR}/include/Version.h
	${CMAKE_CURRENT_BINARY_DIR}/version.rc
	.clang-format
)

target_compile_features(
	${PROJECT_NAME}
	PRIVATE
		cxx_std_23
)

target_compile_definitions(
	${PROJECT_NAME}
	PRIVATE
		_UNICODE
)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${SKSE_MCP_INCLUDE_DIRS}
		${FLICK_API_INCLUDE_DIRS}
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
		CommonLibSSE::CommonLibSSE
)

target_precompile_headers(
	${PROJECT_NAME}
	PRIVATE
		src/PCH.h
)

if (MSVC)
	target_compile_options(
		${PROJECT_NAME}
		PRIVATE
			/sdl
			/utf-8
			/Zi
			/permissive-
			/Zc:preprocessor
			/wd4200
			"$<$<CONFIG:DEBUG>:>"
			"$<$<CONFIG:RELEASE>:/Zc:inline;/JMC-;/Ob3>"
	)

	target_link_options(
		${PROJECT_NAME}
		PRIVATE
			"$<$<CONFIG:DEBUG>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF>"
			"$<$<CONFIG:RELEASE>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>"
	)
endif ()

if (COPY_BUILD)
	if (DEFINED SKYRIM_MODS_FOLDER)
		add_custom_command(
			TARGET ${PROJECT_NAME}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory ${SKYRIM_MODS_FOLDER}/${NAME}/${RUNTIME}/SKSE/Plugins
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${SKYRIM_MODS_FOLDER}/${NAME}/${RUNTIME}/SKSE/Plugins/
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> ${SKYRIM_MODS_FOLDER}/${NAME}/${RUNTIME}/SKSE/Plugins/
		)
	else ()
		message(
			WARNING
			"Variable SKYRIM_MODS_FOLDER is not defined. Skipping post-build copy command."
		)
	endif ()
endif ()
