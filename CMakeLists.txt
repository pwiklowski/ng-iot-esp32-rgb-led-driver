# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(PROJECT_VER "1.0.0")

set(EXTRA_COMPONENT_DIRS ../esp-idf-iot-library)

if(NOT DEFINED ENV{VARIANT})
    
#	set(ENV{VARIANT} "VARIANT_3CH")
#	set(ENV{VARIANT} "VARIANT_3CH_RGB")
#	set(ENV{VARIANT} "VARIANT_6CH")
	set(ENV{VARIANT} "VARIANT_6CH_RGB")
endif()

if(NOT DEFINED ENV{VARIANT})
	message(FATAL_ERROR "Please select variant of the build")
endif()

add_compile_definitions("$ENV{VARIANT}")

if("$ENV{VARIANT}" STREQUAL "VARIANT_3CH")
	project(iot-esp32-led-driver-3ch)
endif()

if("$ENV{VARIANT}" STREQUAL "VARIANT_3CH_RGB")
	project(iot-esp32-led-driver-3ch-rgb)
endif()

if("$ENV{VARIANT}" STREQUAL "VARIANT_6CH")
	project(iot-esp32-led-driver-6ch)
endif()

if("$ENV{VARIANT}" STREQUAL "VARIANT_6CH_RGB")
	project(iot-esp32-led-driver-6ch-rgb)
endif()

message(STATUS "Building variant " $ENV{VARIANT})