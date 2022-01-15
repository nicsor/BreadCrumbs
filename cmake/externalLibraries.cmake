# Boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# Setting to version 1.75 in order to be in tune with the version found 
# on hardknott for serialization. Slightly lower version might work, as well.
find_package(Boost 1.75 REQUIRED COMPONENTS program_options serialization)

if (NOT Boost_FOUND)
  message(FATAL_ERROR "Boost not found")
else()
  message(STATUS "Setting up BOOST")
  message(STATUS " Includes - ${Boost_INCLUDE_DIRS}")
  message(STATUS " Library  - ${Boost_LIBRARY_DIRS}")
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
endif (NOT Boost_FOUND)

# Supress boost header warnings
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

#Pkg-config
find_package(PkgConfig REQUIRED)

# GTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

FetchContent_MakeAvailable(googletest)
enable_testing()
add_custom_target(tests ${CMAKE_CTEST_COMMAND} -V)

# Valgrind
find_program(VALGRIND "valgrind")
if(VALGRIND)
#    add_custom_target(valgrind
#        COMMAND "${VALGRIND}" --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes $<TARGET_FILE:bftest>)
endif()
