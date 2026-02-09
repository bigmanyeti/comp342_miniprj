cmake_minimum_required(VERSION 3.10)
project(GravitySimulator)

set(CMAKE_CXX_STANDARD 17)

# Find Packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Include directories (Adjust if you have local headers)
include_directories(${OPENGL_INCLUDE_DIRS})

# Add your source file here! (This fixes your 'main' error)
add_executable(GravitySimulator main.cpp)

# Link Libraries
if(UNIX)
    target_link_libraries(GravitySimulator glfw GL m)
elseif(WIN32)
    target_link_libraries(GravitySimulator glfw3 opengl32)
endif()
