cmake_minimum_required(VERSION 3.16)

set(VCPKG_ROOT "${CMAKE_SOURCE_DIR}/vcpkg")

if(WIN32)
	set(VCPKG_EXECUTABLE "${VCPKG_ROOT}/vcpkg.exe")
	set(BOOTSTRAP_SCRIPT "${VCPKG_ROOT}/bootstrap-vcpkg.bat")
else()
	set(VCPKG_EXECUTABLE "${VCPKG_ROOT}/vcpkg")
	set(BOOTSTRAP_SCRIPT "${VCPKG_ROOT}/bootstrap-vcpkg.sh")
endif()

if(NOT EXISTS "${VCPKG_EXECUTABLE}")
	message(STATUS "Bootstrapping vcpkg...")
	execute_process(
		COMMAND ${BOOTSTRAP_SCRIPT} -disableMetrics
		WORKING_DIRECTORY ${VCPKG_ROOT}
		RESULT_VARIABLE BOOTSTRAP_RESULT
	)
	if(NOT BOOTSTRAP_RESULT EQUAL 0)
		message(FATAL_ERROR "vcpkg bootstrap failed!")
	endif()
endif()

set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

project(Hel LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(GENERATED_COMPONENT_CPP "${CMAKE_SOURCE_DIR}/src/ecs/ComponentList.cpp")
add_custom_command(
	OUTPUT "${GENERATED_COMPONENT_CPP}"
	COMMAND python3 "${CMAKE_SOURCE_DIR}/autoAddComponent.py"
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	COMMENT "Scanning for components and updating ComponentList.cpp..."
	VERBATIM
)
add_custom_target(run_python_script ALL
	DEPENDS "${GENERATED_COMPONENT_CPP}"
)

file(GLOB_RECURSE HEL_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE EXTERNAL_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_SOURCE_DIR}/external/*.cpp"
)
list(APPEND HEL_SOURCES ${EXTERNAL_SOURCES} "${GENERATED_COMPONENT_CPP}")
list(REMOVE_DUPLICATES HEL_SOURCES)

add_executable(hel ${HEL_SOURCES})
add_dependencies(hel run_python_script)

target_include_directories(hel PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(hel SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/external)

target_compile_definitions(hel PRIVATE
	IMGUI_DEFINE_MATH_OPERATORS
	$<$<CONFIG:Debug>:VALIDATION_LAYERS>
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|AppleClang")
	target_compile_options(hel PRIVATE
		$<$<CONFIG:Debug>:-Wall -Wextra -g -O0>
		$<$<CONFIG:Release>:-Wall -Wextra -O3>
	)
elseif(MSVC)
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

find_program(GLSLC_EXECUTABLE glslangValidator REQUIRED)
if(NOT GLSLC_EXECUTABLE)
	message(FATAL_ERROR "glslangValidator not found. Run: sudo apt install glslang-tools")
else()
	message(STATUS "glslangValidator found: ${GLSLC_EXECUTABLE}")
endif()

file(GLOB_RECURSE SHADERS CONFIGURE_DEPENDS
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
		COMMAND ${GLSLC_EXECUTABLE} -V ${shader} -o ${SPV_FILE}
		DEPENDS ${shader}
		COMMENT "Compiling shader: ${REL_PATH}"
	)
	list(APPEND SPV_FILES ${SPV_FILE})
endforeach()
add_custom_target(compile_shaders ALL DEPENDS ${SPV_FILES})
add_dependencies(hel compile_shaders)

find_package(Git REQUIRED)
set(SPONZA_EXTRACT_DIR "${CMAKE_SOURCE_DIR}/assets/models/sponza")
if(NOT EXISTS "${SPONZA_EXTRACT_DIR}/sponza.obj")
	message(STATUS "Sponza not found. Cloning from GitHub...")
	execute_process(
		COMMAND ${GIT_EXECUTABLE} clone --depth 1
			https://github.com/jimmiebergmann/Sponza.git
			"${SPONZA_EXTRACT_DIR}"
		RESULT_VARIABLE GIT_RESULT
	)
	if(NOT GIT_RESULT EQUAL 0)
		message(FATAL_ERROR "Failed to clone Sponza repo (exit code: ${GIT_RESULT})")
	endif()
	file(REMOVE_RECURSE "${SPONZA_EXTRACT_DIR}/.git")
	message(STATUS "Sponza assets ready at: ${SPONZA_EXTRACT_DIR}")
else()
	message(STATUS "Sponza assets already present, skipping clone.")
endif()

set(MODELS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets/models")
set(MODELS_DEST_DIR "${CMAKE_BINARY_DIR}/assets/models")
add_custom_target(copy_models ALL
	COMMAND ${CMAKE_COMMAND} -E make_directory ${MODELS_DEST_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${MODELS_SOURCE_DIR} ${MODELS_DEST_DIR}
	COMMENT "Copying models to assets/models"
)
add_dependencies(hel copy_models)

set(MODELS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets/images")
set(MODELS_DEST_DIR "${CMAKE_BINARY_DIR}/assets/images")
add_custom_target(copy_images ALL
	COMMAND ${CMAKE_COMMAND} -E make_directory ${MODELS_DEST_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${MODELS_SOURCE_DIR} ${MODELS_DEST_DIR}
	COMMENT "Copying images to assets/images"
)
add_dependencies(hel copy_images)

install(TARGETS hel RUNTIME DESTINATION .)
install(DIRECTORY ${SPV_OUTPUT_DIR}/
	DESTINATION assets/shaders
	FILES_MATCHING PATTERN "*.spv"
)
install(DIRECTORY ${MODELS_DEST_DIR}/
	DESTINATION assets/models
)