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

target_link_libraries(gaven PRIVATE cjson)
target_include_directories(gaven PRIVATE
    ${cjson_SOURCE_DIR}
)
if(WIN32)
    target_link_libraries(gaven PRIVATE ws2_32)
endif()