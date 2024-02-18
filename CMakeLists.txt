cmake_minimum_required(VERSION 3.16)

set(CMAKE_GENERATOR_PLATFORM Win32 CACHE STRING "" FORCE)
set(CMAKE_CONFIGURATION_TYPES Release CACHE STRING "" FORCE)

project(GenomeScript)

# Your Gothic 3 installation folder
set(GOTHIC3_DIR "C:/Program Files (x86)/Steam/steamapps/common/Gothic 3")

# All source and header files
set(SOURCES
	src/common.hpp
	src/log.cpp
	src/log.hpp
	src/main.cpp
	src/system.cpp
	src/system.hpp
)

# The library with all headers and source files
add_library(GenomeScript SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "include")
target_compile_options(${PROJECT_NAME} PUBLIC /Gz)

# How to group the files in the Solution Explorer
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/src" PREFIX "src" FILES ${SOURCES})
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/include" PREFIX "include" FILES ${HEADERS})
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/detours" PREFIX "detours" FILES "detours/CMakeLists.txt")

# Add Microsoft Detours library (MIT License)
add_subdirectory(detours)
target_compile_options(detours INTERFACE /Gz)
target_link_libraries(${PROJECT_NAME} PUBLIC detours::detours)

# Import the spdlog library (MIT License)
add_subdirectory(spdlog)
target_compile_options(spdlog PUBLIC /Gz)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
target_compile_definitions(${PROJECT_NAME} PUBLIC SPDLOG_COMPILED_LIB)

# Import gothic3sdk
add_subdirectory(gothic3sdk)
target_compile_options(gothic3sdk PUBLIC /Gz)
target_link_libraries(${PROJECT_NAME} PUBLIC gothic3sdk)

# Import Lua scripting engine
add_subdirectory(Lua)
add_subdirectory(LuaBridge3)
target_compile_options(lua_static PUBLIC /Gz)
target_compile_options(LuaBridge INTERFACE /Gz)
target_link_libraries(${PROJECT_NAME} PUBLIC lua_static LuaBridge)

# Properties of the DLL
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)			# C++17
target_compile_options(${PROJECT_NAME} PUBLIC /Od)					# Disable optimizations to make Detours work
#target_compile_options(${PROJECT_NAME} PUBLIC /Zc:preprocessor)	    # Force the standards-compliant preprocessor
#target_precompile_headers(${PROJECT_NAME} PRIVATE "include/pch.h")  # Precompiled headers to speed up compilation
target_link_libraries(${PROJECT_NAME} PUBLIC dbghelp)

# How to handle CMake-generated targets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PREDEFINED_TARGETS_FOLDER "CMakePredefinedTargets")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# Where the DLL is written to
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${GOTHIC3_DIR}")

# Catch wrong compilers
if (NOT WIN32 OR NOT MSVC)
	message(FATAL_ERROR "This project can only built using Visual Studio on Windows!")
endif ()

