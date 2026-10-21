
cmake_minimum_required(VERSION 3.20)

project(Terraria LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Available build configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release"
	CACHE STRING "" FORCE
)

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3"
	CACHE STRING "" FORCE
)

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG"
	CACHE STRING "" FORCE
)

# SDL dependencies
find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL REQUIRED IMPORTED_TARGET sdl12_compat)
pkg_check_modules(SDL_IMAGE REQUIRED IMPORTED_TARGET SDL_image)
pkg_check_modules(SDL_GFX REQUIRED IMPORTED_TARGET SDL_gfx)

# Automatically discover all sources and headers
file(GLOB_RECURSE PROJECT_FILES
	CONFIGURE_DEPENDS
	RELATIVE "${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.c"
	"${PROJECT_SOURCE_DIR}/src/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.hpp"
)

add_executable(Terraria ${PROJECT_FILES})

# Symlink res/ into both build directories
foreach(CONFIG Debug Release)
	file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${CONFIG}")

	file(CREATE_LINK
		"${PROJECT_SOURCE_DIR}/res"
		"${CMAKE_BINARY_DIR}/${CONFIG}/res"
		SYMBOLIC
	)
endforeach()

target_include_directories(Terraria PRIVATE
	"${PROJECT_SOURCE_DIR}/src"
)

target_link_libraries(Terraria PRIVATE
	PkgConfig::SDL
	PkgConfig::SDL_IMAGE
	PkgConfig::SDL_GFX
)

# Strip Release symbols
target_link_options(Terraria PRIVATE
	$<$<CONFIG:Release>:-s>
)

# Place executables in bin/Debug and bin/Release
set_target_properties(Terraria PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --------------------------------------------------
# Generate Code::Blocks project
# --------------------------------------------------

if(CMAKE_GENERATOR STREQUAL "Ninja Multi-Config")

	set(CBP "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
	string(APPEND CBP "<CodeBlocks_project_file>\n")
	string(APPEND CBP "\t<FileVersion major=\"1\" minor=\"6\" />\n")
	string(APPEND CBP "\t<Project>\n")
	string(APPEND CBP "\t\t<Option title=\"Terraria\" />\n")
	string(APPEND CBP "\t\t<Option compiler=\"gcc\" />\n")
	string(APPEND CBP "\t\t<Option makefile_is_custom=\"1\" />\n")
	string(APPEND CBP "\t\t<Option default_target=\"Debug\" />\n")
	string(APPEND CBP "\t\t<Build>\n")

	foreach(CONFIG Debug Release)

		if(CONFIG STREQUAL "Debug")
			set(TARGET_TYPE 1)
			set(CONSOLE_RUNNER 1)
		else()
			set(TARGET_TYPE 0)
			set(CONSOLE_RUNNER 0)
		endif()

		set(BUILD_COMMAND
			"cmake --build bin --config ${CONFIG}"
		)

		string(APPEND CBP "\t\t\t<Target title=\"${CONFIG}\">\n")
		string(APPEND CBP "\t\t\t\t<Option output=\"bin/${CONFIG}/Terraria\" prefix_auto=\"0\" extension_auto=\"0\" />\n")
		string(APPEND CBP "\t\t\t\t<Option working_dir=\".\" />\n")
		string(APPEND CBP "\t\t\t\t<Option type=\"${TARGET_TYPE}\" />\n")
		string(APPEND CBP "\t\t\t\t<Option compiler=\"gcc\" />\n")
		string(APPEND CBP "\t\t\t\t<Option use_console_runner=\"${CONSOLE_RUNNER}\" />\n")
		string(APPEND CBP "\t\t\t\t<MakeCommands>\n")
		string(APPEND CBP "\t\t\t\t\t<Build command=\"${BUILD_COMMAND}\" />\n")
		string(APPEND CBP "\t\t\t\t\t<CompileFile command=\"${BUILD_COMMAND}\" />\n")
		string(APPEND CBP "\t\t\t\t\t<Clean command=\"${BUILD_COMMAND} --target clean\" />\n")
		string(APPEND CBP "\t\t\t\t\t<DistClean command=\"${BUILD_COMMAND} --target clean\" />\n")
		string(APPEND CBP "\t\t\t\t\t<SilentBuild command=\"${BUILD_COMMAND}\" />\n")
		string(APPEND CBP "\t\t\t\t</MakeCommands>\n")
		string(APPEND CBP "\t\t\t</Target>\n")

	endforeach()

	string(APPEND CBP "\t\t</Build>\n")

	# Add all src files to Code::Blocks tree
	foreach(SOURCE_FILE IN LISTS PROJECT_FILES)
		string(APPEND CBP
			"\t\t<Unit filename=\"${SOURCE_FILE}\">\n"
			"\t\t\t<Option compile=\"0\" link=\"0\" />\n"
			"\t\t</Unit>\n"
		)
	endforeach()

	string(APPEND CBP "\t\t<Unit filename=\"CMakeLists.txt\" />\n")
	string(APPEND CBP "\t\t<Extensions />\n")
	string(APPEND CBP "\t</Project>\n")
	string(APPEND CBP "</CodeBlocks_project_file>\n")

	file(WRITE "${PROJECT_SOURCE_DIR}/Terraria.cbp" "${CBP}")

	message(STATUS "Generated Terraria.cbp with Debug and Release targets")

endif()
