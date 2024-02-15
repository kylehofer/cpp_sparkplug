cmake_minimum_required(VERSION 3.17)

include(FetchContent)

SET(BUILD_TARGET LINUX CACHE BOOL "")
SET(FETCH_REMOTE ON CACHE BOOL "")

IF(DEFINED ENV{FETCH_REMOTE})
    SET(FETCH_REMOTE $ENV{FETCH_REMOTE})
ENDIF()

IF(${BUILD_TARGET} STREQUAL "LINUX")
    option(CPP_SPARKPLUG_PAHO "" ON)
    option(CPP_SPARKPLUG_TESTS "" ON)
ENDIF()

project(cpp_sparkplug C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

set(cmake_package_name CPPSparkplug CACHE INTERNAL "")

option(CPP_SPARKPLUG_STATIC "" OFF)
option(CPP_SPARKPLUG_SHARED "" ON)
option(CPP_SPARKPLUG_TESTS "" ON)

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

IF(NOT CPP_SPARKPLUG_PAHO)
    list(FILTER SOURCES EXCLUDE REGEX "Paho")
ENDIF()

list(FILTER SOURCES EXCLUDE REGEX "PahoSync")

# Main library compiling
add_library(cpp_sparkplug STATIC ${SOURCES})

target_include_directories(cpp_sparkplug PUBLIC "src/")

IF(FETCH_REMOTE)
    FetchContent_Declare(
        pico_tahu
        GIT_REPOSITORY https://github.com/kylehofer/pico_tahu.git
        GIT_TAG main
    )
    FetchContent_MakeAvailable(pico_tahu)
ELSE()
    add_subdirectory(./external/pico_tahu)
ENDIF()

IF(FETCH_REMOTE)
    FetchContent_Declare(
        cpp_mqtt_client
        GIT_REPOSITORY https://github.com/kylehofer/cpp_mqtt_client.git
        GIT_TAG main
    )
    FetchContent_MakeAvailable(cpp_mqtt_client)
ELSE()
    add_subdirectory(./external/cpp_mqtt_client)
ENDIF()

IF(CPP_SPARKPLUG_PAHO)
    set(PAHO_BUILD_STATIC TRUE)
    set(PAHO_BUILD_SHARED TRUE)
    set(PAHO_WITH_SSL FALSE)
    set(PAHO_HIGH_PERFORMANCE TRUE)
    set(PAHO_BUILD_SAMPLES FALSE)
    set(PAHO_ENABLE_TESTING FALSE)
    set(PAHO_BUILD_DOCUMENTATION FALSE)

    # Grabbing paho MQTT
    FetchContent_Declare(
        paho_mqtt
        GIT_REPOSITORY https://github.com/eclipse/paho.mqtt.c.git
        GIT_TAG e0ce44679aaeada4c39769f9d0c9bd954ad4b156 # release-1.4
    )

    FetchContent_MakeAvailable(paho_mqtt)
    target_include_directories(cpp_sparkplug PUBLIC "${paho_mqtt_SOURCE_DIR}/src")

    target_link_libraries(
        cpp_sparkplug
        paho-mqtt3a-static

        # paho-mqtt3c-static
    )
ENDIF()

target_link_libraries(cpp_sparkplug
    pico_tahu
    cpp_mqtt_client
)

IF(${BUILD_TARGET} STREQUAL "LINUX")
    add_subdirectory(./examples)
ENDIF()

IF(CPP_SPARKPLUG_TESTS AND ${BUILD_TARGET} STREQUAL "LINUX")
    # enable_testing()
    # add_subdirectory(./tests)
ENDIF()