# Project information
cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0074 NEW)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-fpermissive")
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID "X86")
set(CMAKE_BUILD_TYPE RelWithDebInfo)

# External libraries
#---------------------------------------
set(THIRD_PARTY_LIBS_DIR "cmake")
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/${THIRD_PARTY_LIBS_DIR})
include(externalLibraries)

# Internal components
#---------------------------------------
set(BUILD_PATHS "core" "components" "networking" "apps" "samples")
foreach(path ${BUILD_PATHS})
    file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/${path}/*)

    foreach(child ${children})
        if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${child})
            add_subdirectory(${child})
        endif()
    endforeach()
endforeach()
