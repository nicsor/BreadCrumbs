# Boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# regex system filesystem program_options date_time thread
find_package(Boost REQUIRED COMPONENTS program_options)

if (NOT Boost_FOUND)
  message(FATAL_ERROR "Boost not found")
else()
  message(STATUS "Setting up BOOST")
  message(STATUS " Includes - ${Boost_INCLUDE_DIRS}")
  message(STATUS " Library  - ${Boost_LIBRARY_DIRS}")
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
endif (NOT Boost_FOUND)

#Pkg-config
find_package(PkgConfig REQUIRED)

# GTest
find_package(GTest)
if (GTEST_FOUND)
    enable_testing()
    # Global tests
    add_custom_target(tests ${CMAKE_CTEST_COMMAND} -V)
else()
    message("GTest not available: Tests were not enabled!")
endif()

# Valgrind
find_program(VALGRIND "valgrind")
if(VALGRIND)
#    add_custom_target(valgrind
#        COMMAND "${VALGRIND}" --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes $<TARGET_FILE:bftest>)
endif()
