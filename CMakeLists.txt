cmake_minimum_required(VERSION 3.17)

include(FetchContent)
include(cmake/CPM.cmake)

SET(BUILD_TARGET LINUX CACHE BOOL "")
SET(FETCH_REMOTE ON CACHE BOOL "")
SET(CPP_SPARKPLUG_STATIC OFF CACHE BOOL "")
SET(CPP_SPARKPLUG_SHARED ON CACHE BOOL "")
SET(CPP_SPARKPLUG_TESTS ON CACHE BOOL "")
SET(CPP_SPARKPLUG_PAHO_ASYNC ON CACHE BOOL "")
SET(CPP_SPARKPLUG_PAHO_SYNC ON CACHE BOOL "")
SET(CPP_SPARKPLUG_EXAMPLES ON CACHE BOOL "")
SET(CPP_SPARKPLUG_MQTT OFF CACHE BOOL "")

IF(DEFINED ENV{FETCH_REMOTE})
    SET(FETCH_REMOTE $ENV{FETCH_REMOTE})
ENDIF()

project(cpp_sparkplug C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(cmake_package_name CPPSparkplug CACHE INTERNAL "")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

IF(NOT ${BUILD_TARGET} STREQUAL "PICO")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ENDIF()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")
file(GLOB_RECURSE HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.h*")

IF(NOT CPP_SPARKPLUG_PAHO_SYNC)
    list(FILTER SOURCES EXCLUDE REGEX "PahoSync")
ENDIF()

IF(NOT CPP_SPARKPLUG_PAHO_ASYNC)
    list(FILTER SOURCES EXCLUDE REGEX "PahoAsync")
ENDIF()

IF(NOT CPP_SPARKPLUG_MQTT)
    list(FILTER SOURCES EXCLUDE REGEX "CppMqttClient")
ENDIF()

# Main library compiling
IF(${CPP_SPARKPLUG_STATIC})
    add_library(cpp_sparkplug STATIC ${SOURCES})
ELSE()
    add_library(cpp_sparkplug SHARED ${SOURCES})
ENDIF()

target_include_directories(cpp_sparkplug PUBLIC "src/")

IF(FETCH_REMOTE)
    CPMAddPackage(
        pico_tahu
        GIT_REPOSITORY https://github.com/kylehofer/pico_tahu.git
        GIT_TAG main
        OPTIONS
        "BUILD_TARGET ${BUILD_TARGET}"
        "PICO_TAHU_STATIC ${CPP_SPARKPLUG_STATIC}"
        "PICO_TAHU_SHARED ${CPP_SPARKPLUG_SHARED}"
        "CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}"
        "CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}"
    )

ELSE()
    SET(PICO_TAHU_STATIC CPP_SPARKPLUG_STATIC CACHE BOOL "")
    SET(PICO_TAHU_SHARED CPP_SPARKPLUG_SHARED CACHE BOOL "")
    add_subdirectory(./external/pico_tahu)
ENDIF()

target_link_libraries(cpp_sparkplug
    pico_tahu
)

IF(CPP_SPARKPLUG_MQTT)
    IF(FETCH_REMOTE)
        CPMAddPackage(
            cpp_mqtt_client
            GIT_REPOSITORY https://github.com/kylehofer/cpp_mqtt_client.git
            GIT_TAG main
            OPTIONS
            "BUILD_TARGET ${BUILD_TARGET}"
            "MQTT_ENABLE_TESTING ${FALSE}"
            "CPP_MQTT_STATIC ${CPP_SPARKPLUG_STATIC}"
            "CPP_MQTT_SHARED ${CPP_SPARKPLUG_SHARED}"
            "CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}"
            "CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}"
        )

    ELSE()
        SET(CPP_MQTT_STATIC CPP_SPARKPLUG_STATIC CACHE BOOL "")
        SET(CPP_MQTT_SHARED CPP_SPARKPLUG_SHARED CACHE BOOL "")
        add_subdirectory(./external/cpp_mqtt_client)
    ENDIF()

    target_link_libraries(cpp_sparkplug
        cpp_mqtt_client
    )
ENDIF()

IF(CPP_SPARKPLUG_PAHO_SYNC OR CPP_SPARKPLUG_PAHO_ASYNC)
    # Grabbing paho MQTT
    CPMAddPackage(
        paho_mqtt
        GIT_REPOSITORY https://github.com/eclipse/paho.mqtt.c.git
        GIT_TAG e0ce44679aaeada4c39769f9d0c9bd954ad4b156
        OPTIONS
        "PAHO_BUILD_STATIC ${CPP_SPARKPLUG_STATIC}"
        "PAHO_BUILD_SHARED ${CPP_SPARKPLUG_SHARED}"
        "PAHO_WITH_SSL FALSE"
        "PAHO_HIGH_PERFORMANCE TRUE"
        "PAHO_BUILD_SAMPLES FALSE"
        "PAHO_ENABLE_TESTING FALSE"
        "PAHO_BUILD_DOCUMENTATION FALSE"
        "CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}"
        "CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}"
    )

    IF(CPP_SPARKPLUG_PAHO_SYNC)
        IF(CPP_SPARKPLUG_STATIC)
            target_link_libraries(
                cpp_sparkplug
                paho-mqtt3c-static
            )
        ELSE()
            target_link_libraries(
                cpp_sparkplug
                paho-mqtt3c
            )
        ENDIF()
    ENDIF()

    IF(CPP_SPARKPLUG_PAHO_ASYNC)
        IF(CPP_SPARKPLUG_STATIC)
            target_link_libraries(
                cpp_sparkplug
                paho-mqtt3a-static
            )
        ELSE()
            target_link_libraries(
                cpp_sparkplug
                paho-mqtt3a
            )
        ENDIF()
    ENDIF()

    IF(CPP_SPARKPLUG_EXAMPLES)
        add_subdirectory(./examples)
    ENDIF()

    IF(CPP_SPARKPLUG_TESTS)
        enable_testing()
        add_subdirectory(./tests)
    ENDIF()
ELSE()
ENDIF()

INSTALL(
    DIRECTORY ${CMAKE_SOURCE_DIR}/src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h*")

INSTALL(TARGETS cpp_sparkplug
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})