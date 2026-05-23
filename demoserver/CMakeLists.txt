set(SOURCES src/main.c)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/DEMO)
add_executable(demoserver)

target_sources(demoserver PRIVATE ${SOURCES})

target_link_libraries(demoserver gaven)

target_include_directories(demoserver PRIVATE include)