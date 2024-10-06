cmake_minimum_required(VERSION 3.12)
project(simulation C)

set(CMAKE_C_STANDARD 99)

# Define paths to external libraries
set(CGLM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/cglm")
set(RAYLIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/raylib")
set(RAYGUI_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/raygui")

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CGLM_PATH}/include
    ${RAYLIB_PATH}/include
    ${RAYGUI_PATH}/include
)

# Add compiler definitions
add_definitions(-DCGLM_USE_DOUBLE -DCGLM_USE_ANONYMOUS_STRUCT -DRAYGUI_IMPLEMENTATION)

# Find source files
file(GLOB SOURCES "src/*.c")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${CGLM_PATH}/lib/libcglm.a
    ${RAYLIB_PATH}/lib/libraylib.a
)

# MacOS specific settings
if(APPLE)
    target_link_libraries(${PROJECT_NAME}
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)