include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES ${CMAKE_SOURCE_DIR} "*.cpp")

enable_testing()

add_executable(cpp_sparkplug_tests ${TEST_SOURCES})

target_include_directories(cpp_sparkplug_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} "utils/")
target_include_directories(cpp_sparkplug_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} "mocks/")

set(WITH_DOCS OFF CACHE BOOL "" FORCE)
set(WITH_CJSON OFF CACHE BOOL "" FORCE)

# Grabbing mosquitto
ExternalProject_Add(mosquitto_external
    GIT_REPOSITORY https://github.com/eclipse/mosquitto.git
    GIT_TAG b0277869d9806f6fab8e1bc11c4a4987c9a79ded # v2.0.15
    INSTALL_COMMAND ""
    CMAKE_ARGS
    -DDOCUMENTATION=OFF
    -DWITH_CJSON=OFF
)

add_dependencies(cpp_sparkplug mosquitto_external)

target_link_libraries(
    cpp_sparkplug_tests
    GTest::gtest_main
    GTest::gmock_main
    cpp_sparkplug
    mosquitto
)

include(GoogleTest)
gtest_discover_tests(cpp_sparkplug_tests)
