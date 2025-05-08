
cmake_minimum_required ( VERSION 3.10.0 )

project ( RayTracer )

set ( CODE_PATH ${CMAKE_CURRENT_LIST_DIR}/code )
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2" )
set ( CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Limited configurations" FORCE )

file (
    GLOB_RECURSE
    SOURCES
    ${CODE_PATH}/sources/*.cpp
)

file (
    GLOB_RECURSE
    HEADERS
    ${CODE_PATH}/headers/*.hpp
)

list ( FILTER HEADERS EXCLUDE REGEX ${CODE_PATH}/headers/raytracer/dependencies/.* )

add_library (
    ray-tracer
    STATIC
    ${SOURCES}
    ${HEADERS}
)

target_include_directories (
    ray-tracer
    PUBLIC
    ${CODE_PATH}/headers
)

set_property ( TARGET ray-tracer PROPERTY CXX_STANDARD 20 )
set_property ( TARGET ray-tracer PROPERTY CXX_STANDARD_REQUIRED ON )
set_property ( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ray-tracer )
