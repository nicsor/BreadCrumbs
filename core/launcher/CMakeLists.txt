# Component name
get_filename_component(COMPONENT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" COMPONENT_NAME ${COMPONENT_NAME})

# Project information
project(${COMPONENT_NAME} LANGUAGES C CXX)

# Component source files
file(GLOB_RECURSE COMPONENT_SRCS "src/*.cpp")

# Generate binary
add_library(${COMPONENT_NAME} ${COMPONENT_SRCS})

# Target dependencies
#target_link_libraries(${COMPONENT_NAME} -Wl,--whole-archive DetachedLogger -Wl,--no-whole-archive logger ${Boost_LIBRARIES} component)
target_link_libraries(${COMPONENT_NAME} logger ${Boost_LIBRARIES} component)
target_include_directories(${COMPONENT_NAME} PRIVATE src)
