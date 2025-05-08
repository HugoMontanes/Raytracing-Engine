
cmake_minimum_required ( VERSION 3.10.0 )

project ( Engine )

set ( CODE_PATH        ${CMAKE_CURRENT_LIST_DIR}/code )
set ( SDL3_PATH        ${CMAKE_CURRENT_LIST_DIR}/libraries/sdl3 )
set ( RAY_TRACER_PATH "${CMAKE_CURRENT_LIST_DIR}/../ray tracer" )

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

list ( FILTER HEADERS EXCLUDE REGEX ${CODE_PATH}/headers/engine/dependencies/.* )

add_library (
    sdl3
    STATIC
    IMPORTED
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set_target_properties (
        sdl3
        PROPERTIES
        IMPORTED_LOCATION_DEBUG   ${SDL3_PATH}/lib/visual-studio-2022/x64/sdl3-static-d.lib
        IMPORTED_LOCATION_RELEASE ${SDL3_PATH}/lib/visual-studio-2022/x64/sdl3-static.lib
    )
elseif (CMAKE_GENERATOR MATCHES "Xcode")
    set_target_properties (
        sdl3
        PROPERTIES
        IMPORTED_LOCATION_DEBUG   ${SDL3_PATH}/lib/xcode-15/arm64/sdl3-static-d.a
        IMPORTED_LOCATION_RELEASE ${SDL3_PATH}/lib/xcode-15/arm64/sdl3-static.a
    )
endif()

add_library (
    engine
    STATIC
    ${SOURCES}
    ${HEADERS}
)

target_include_directories ( engine PUBLIC  ${CODE_PATH}/headers )
target_include_directories ( engine PRIVATE ${RAY_TRACER_PATH}/code/headers )
target_include_directories ( engine PRIVATE ${SDL3_PATH}/include )

target_link_libraries (
    engine
    PUBLIC
    sdl3
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")

    target_link_libraries (
        engine
        PUBLIC
        version.lib
        setupapi.lib
        winmm.lib
        imm32.lib
    )

elseif (CMAKE_GENERATOR MATCHES "Xcode")

    target_link_libraries (
        engine
        PUBLIC
        "-framework CoreMedia"
        "-framework CoreVideo"
        "-framework Cocoa"
        "-framework UniformTypeIdentifiers"
        "-framework IOKit"
        "-framework ForceFeedback"
        "-framework Carbon"
        "-framework CoreAudio"
        "-framework AudioToolbox"
        "-framework AVFoundation"
        "-framework CoreBluetooth"
        "-framework CoreGraphics"
        "-framework CoreMotion"
        "-framework Foundation"
        "-framework GameController"
        "-framework Metal"
        "-framework QuartzCore"
        "-framework CoreHaptics"
    )

    target_link_directories ( engine PUBLIC "${CMAKE_OSX_SYSROOT}/System/Library/Frameworks" )

endif()

set_property ( TARGET engine PROPERTY CXX_STANDARD 20 )
set_property ( TARGET engine PROPERTY CXX_STANDARD_REQUIRED ON )
set_property ( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT engine )
