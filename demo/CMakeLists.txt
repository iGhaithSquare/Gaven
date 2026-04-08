set(SOURCES src/main.c)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/DEMO)

add_executable(demo)

target_sources(demo PRIVATE ${SOURCES})

target_link_libraries(demo gaven)

target_include_directories(demo PRIVATE include)