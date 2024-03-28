file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

IF(CPP_SPARKPLUG_PAHO_ASYNC)
    add_executable(cpu_example_async ${SOURCES})
    target_compile_definitions(cpu_example_async PRIVATE
        EXAMPLE_ASYNC=1
    )
    target_include_directories(cpu_example_async PUBLIC "./")
    target_link_libraries(cpu_example_async cpp_sparkplug)
ENDIF()

IF(CPP_SPARKPLUG_PAHO_SYNC)
    add_executable(cpu_example_sync ${SOURCES})
    target_include_directories(cpu_example_sync PUBLIC "./")
    target_link_libraries(cpu_example_sync cpp_sparkplug)
ENDIF()
