cmake_minimum_required(VERSION 3.15)

project(Orbifold-visualization LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all sources automatically from src/
file(GLOB SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

file(GLOB HEADERS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    SHADER_DIR="${CMAKE_SOURCE_DIR}/src/shaders"
)


# --- Use vcpkg to find libraries ---
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
    GLEW::GLEW
    GLUT::GLUT
)
