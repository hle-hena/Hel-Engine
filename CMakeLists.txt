cmake_minimum_required(VERSION 3.16)
project(Hel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

file(GLOB_RECURSE HEL_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_SOURCE_DIR}/src/*.cpp"
)
add_executable(hel ${HEL_SOURCES})
target_include_directories(hel PRIVATE
	${CMAKE_SOURCE_DIR}/src
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|AppleClang")
	target_compile_options(hel PRIVATE
		$<$<CONFIG:Debug>:-Wall -Wextra -g -O0>
		$<$<CONFIG:Release>:-Wall -Wextra -O3>
	)
elseif (MSVC)
	target_compile_options(hel PRIVATE
		$<$<CONFIG:Debug>:/W4 /Zi /Od>
		$<$<CONFIG:Release>:/W4 /O2>
	)
endif()

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
target_link_libraries(hel PRIVATE
	Vulkan::Vulkan
	glfw
)

find_program(GLSLC_EXECUTABLE glslc REQUIRED)
if (NOT GLSLC_EXECUTABLE)
	message(FATAL_ERROR "glslc not found. Install shaderc or Vulkan SDK.")
else()
	message(STATUS "glslc found: ${GLSLC_EXECUTABLE}")
endif()

file(GLOB_RECURSE SHADERS
	"${CMAKE_SOURCE_DIR}/assets/shaders/*.vert"
	"${CMAKE_SOURCE_DIR}/assets/shaders/*.frag"
)
set(SPV_OUTPUT_DIR "${CMAKE_BINARY_DIR}/assets/shaders")
file(MAKE_DIRECTORY ${SPV_OUTPUT_DIR})
set(SPV_FILES "")

foreach(shader ${SHADERS})
	file(RELATIVE_PATH REL_PATH "${CMAKE_SOURCE_DIR}/assets/shaders" "${shader}")
	set(SPV_FILE "${SPV_OUTPUT_DIR}/${REL_PATH}.spv")
	get_filename_component(SPV_DIR "${SPV_FILE}" DIRECTORY)
	file(MAKE_DIRECTORY ${SPV_DIR})
	add_custom_command(
		OUTPUT ${SPV_FILE}
		COMMAND ${GLSLC_EXECUTABLE} ${shader} -o ${SPV_FILE}
		DEPENDS ${shader}
		COMMENT "Compiling shader: ${REL_PATH}"
	)
	list(APPEND SPV_FILES ${SPV_FILE})
endforeach()
add_custom_target(compile_shaders ALL DEPENDS ${SPV_FILES})
add_dependencies(hel compile_shaders)

set(MODELS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets/models")
set(MODELS_DEST_DIR "${CMAKE_BINARY_DIR}/assets/models")
add_custom_target(copy_models ALL
	COMMAND ${CMAKE_COMMAND} -E make_directory ${MODELS_DEST_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${MODELS_SOURCE_DIR} ${MODELS_DEST_DIR}
	COMMENT "Copying models to assets/models"
)
add_dependencies(hel copy_models)



install(TARGETS hel
	RUNTIME DESTINATION .
)

install(DIRECTORY ${SPV_OUTPUT_DIR}/
	DESTINATION assets/shaders
	FILES_MATCHING PATTERN "*.spv"
)

install(DIRECTORY ${MODELS_DEST_DIR}/
	DESTINATION assets/models
)
