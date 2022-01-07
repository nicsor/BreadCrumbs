include_directories(${GTEST_INCLUDE_DIRS})

# Generate test binary
add_executable(utils_test src/main.cpp src/EnumCastTest.cpp)
target_link_libraries(utils_test utils gtest gtest_main pthread)

# Setup tests
add_test(utils_test utils_test)

# Enable global testing of component
add_dependencies(tests utils_test)