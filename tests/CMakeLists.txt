include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# find_package(Boost REQUIRED COMPONENTS program_options regex)
# include_directories(${Boost_INCLUDE_DIRS})
file(GLOB_RECURSE TEST_SOURCES ${CMAKE_SOURCE_DIR} "*.cpp")

enable_testing()

add_executable(cpp_sparkplug_tests ${TEST_SOURCES})

target_include_directories(cpp_sparkplug_tests
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} "utils/"
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} "mocks/"
)

set(WITH_DOCS OFF CACHE BOOL "" FORCE)
set(WITH_CJSON OFF CACHE BOOL "" FORCE)

target_link_libraries(
    cpp_sparkplug_tests
    GTest::gtest_main
    GTest::gmock_main
    cpp_sparkplug
    mosquitto
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_REGEX_LIBRARY}
)

include(GoogleTest)
gtest_discover_tests(cpp_sparkplug_tests)
