file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.c")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/GAVEN)

add_library(gaven STATIC)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

target_sources(gaven PRIVATE ${SOURCES})

target_include_directories(gaven
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(gaven PUBLIC cjson)
target_include_directories(gaven PUBLIC
    ${cjson_SOURCE_DIR}
)

option(GAVEN_LOG_TO_CMD "Log to Cmd" ON)
if(GAVEN_LOG_TO_CMD)
    target_compile_definitions(gaven PUBLIC GAVEN_LOG_TO_CMD=1)
else()
    target_compile_definitions(gaven PUBLIC GAVEN_LOG_TO_CMD=0)
endif()
if(WIN32)
    target_link_libraries(gaven PRIVATE ws2_32)
endif()