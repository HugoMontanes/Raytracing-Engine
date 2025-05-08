
cmake_minimum_required ( VERSION 3.10.0 )

project ( App )

set ( CODE_PATH       "${CMAKE_CURRENT_LIST_DIR}/code" )
set ( ENGINE_PATH     "${CMAKE_CURRENT_LIST_DIR}/../engine" )
set ( RAY_TRACER_PATH "${CMAKE_CURRENT_LIST_DIR}/../ray tracer" )

set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/binary )
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2" )
set ( CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Limited configurations" FORCE )

file (
    GLOB_RECURSE
    SOURCES
    ${CODE_PATH}/*.cpp
)

file (
    GLOB_RECURSE
    HEADERS
    ${CODE_PATH}/*.hpp
)

add_executable (
    app
    ${SOURCES}
    ${HEADERS}
)

target_include_directories (
    app
    PRIVATE
    ${ENGINE_PATH}/code/headers
)

target_include_directories (
    app
    PRIVATE
    ${RAY_TRACER_PATH}/code/headers
)

target_link_libraries (
    app
    PRIVATE
    "engine"
    "ray-tracer"
)

set_property ( TARGET app PROPERTY CXX_STANDARD 20 )
set_property ( TARGET app PROPERTY CXX_STANDARD_REQUIRED ON )
set_property ( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT app )
