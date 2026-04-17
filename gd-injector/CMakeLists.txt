cmake_minimum_required(VERSION 3.15)
project(GDInjector)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output to bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Create DLL
add_library(GDInjector SHARED
    main.cpp
)

# Link libraries
target_link_libraries(GDInjector
    PRIVATE
)

# Set output name
set_target_properties(GDInjector PROPERTIES
    OUTPUT_NAME "gd-mcp-injector"
    SUFFIX ".dll"
)
