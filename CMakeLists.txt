cmake_minimum_required(VERSION 3.10)

# Set the project name
project(cricket VERSION 1.0)

# Specify the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Set the build directory
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Ensure the build directory exists
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR})

# Find all source files recursively
file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "src/*.h")

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Find and link seccomp
find_library(SECCOMP_LIBRARY seccomp REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${SECCOMP_LIBRARY})

# Find and link libdw (part of elfutils)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBDW REQUIRED libdw)
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBDW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBDW_LIBRARIES})

# Add compiler warnings
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -g)

# Install the executable
install(TARGETS ${PROJECT_NAME} DESTINATION bin)