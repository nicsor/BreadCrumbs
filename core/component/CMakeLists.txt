# Component name
get_filename_component(COMPONENT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" COMPONENT_NAME ${COMPONENT_NAME})

# Project information
project(${COMPONENT_NAME} LANGUAGES C CXX)

# Component source files
file(GLOB_RECURSE COMPONENT_SRCS "src/*.cpp")

# Generate library
add_library(${COMPONENT_NAME} ${COMPONENT_SRCS})

# Target dependencies
target_link_libraries(${COMPONENT_NAME}
		logger
		utils)

target_include_directories(${COMPONENT_NAME} PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
		"$<INSTALL_INTERFACE:include>")

target_include_directories(${COMPONENT_NAME} PRIVATE src)
