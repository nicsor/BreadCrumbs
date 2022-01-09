# Component name
get_filename_component(COMPONENT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" COMPONENT_NAME ${COMPONENT_NAME})

# Project information
project(${COMPONENT_NAME} LANGUAGES C CXX)

# Component source files
file(GLOB_RECURSE COMPONENT_SRCS "src/*.cpp")

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(glfw3 REQUIRED)


include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

# Generate binary
add_executable(${COMPONENT_NAME} ${COMPONENT_SRCS})

# Target dependencies
# As the message consumer is not connected directly to the app, we must force the linkage.
target_link_libraries(${COMPONENT_NAME} PRIVATE -Wl,--whole-archive server client patterns -Wl,--no-whole-archive launcher)
target_link_libraries(${COMPONENT_NAME} LINK_PUBLIC ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} glfw ${GLFW_LIBRARIES})
target_include_directories(${COMPONENT_NAME} PRIVATE src)
target_include_directories(${COMPONENT_NAME} PUBLIC  include)