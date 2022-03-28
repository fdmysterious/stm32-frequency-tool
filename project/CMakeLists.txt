cmake_minimum_required(VERSION 3.16)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/stm32-cmake/cmake/stm32_gcc.cmake)

project(stm32-template C ASM)

####################################
# Project config
####################################

set(OUTPUT_PATH "/output")

set(HAL_COMP_LIST RCC GPIO CORTEX STM32F3)
set(CMSIS_COMP_LIST "")

####################################
# Find packages
####################################

find_package(CMSIS COMPONENTS "${CMSIS_COMP_LIST}" REQUIRED)
find_package(HAL   COMPONENTS "${HAL_COMP_LIST}"   REQUIRED)

####################################
# Generate executable
####################################

set(PROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/io/clock.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/io/gpio.c

	${CMAKE_CURRENT_SOURCE_DIR}/src/func/error.c

	${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/stm32f3xx_hal_conf.h)
add_custom_command(
	OUTPUT   ${PROJECT_NAME}.bin
	DEPENDS  ${PROJECT_NAME}.elf
	COMMAND  "${CMAKE_OBJCOPY}" -O binary "${PROJECT_NAME}.elf" "${PROJECT_NAME}.bin"
)

add_custom_target(bin ALL
	DEPENDS ${PROJECT_NAME}.bin
	COMMAND  "${CMAKE_OBJCOPY}" -O binary "${PROJECT_NAME}.elf" "${PROJECT_NAME}.bin"
)

target_link_libraries( stm32-template
	HAL::STM32::F3::RCC
	HAL::STM32::F3::RCCEx
	HAL::STM32::F3::GPIO
	HAL::STM32::F3::CORTEX
	CMSIS::STM32::F302xC
	STM32::NoSys
)

stm32_print_size_of_target(stm32-template)

####################################
# Install rules
####################################

install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin
	DESTINATION
		${OUTPUT_PATH}
)
