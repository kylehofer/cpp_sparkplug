file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

add_executable(cpu_example ${SOURCES})
target_include_directories(cpu_example PUBLIC "./")
target_link_libraries(cpu_example cpp_sparkplug)