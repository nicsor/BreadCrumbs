# Component name
get_filename_component(COMPONENT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" COMPONENT_NAME ${COMPONENT_NAME})

# Project information
project(${COMPONENT_NAME} LANGUAGES C CXX)

# Component source files
file(GLOB_RECURSE COMPONENT_SRCS "src/*.cpp")

# Generate binary
add_executable(${COMPONENT_NAME} ${COMPONENT_SRCS})

# Target dependencies
# As the message consumer is not connected directly to the app, we must force the linkage.
target_link_libraries(${COMPONENT_NAME} -Wl,--whole-archive MessageConsumer -Wl,--no-whole-archive logger utils component ${Boost_LIBRARIES})
target_include_directories(${COMPONENT_NAME} PRIVATE src)
