file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.c")

if(NOT ANDROID)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/GAVEN)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/GAVEN)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/GAVEN)
endif()
add_library(gaven SHARED)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

target_sources(gaven PRIVATE ${SOURCES})

target_include_directories(gaven PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(ANDROID)
    find_library(ANDROID_LOG_LIB log)
    target_link_libraries(gaven PRIVATE ${ANDROID_LOG_LIB})
endif()
include(FetchContent)
FetchContent_GetProperties(cjson)
target_link_libraries(gaven PUBLIC cjson)
target_include_directories(gaven PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${cjson_SOURCE_DIR}
)
target_compile_definitions(gaven PRIVATE GAVEN_BUILD_DLL)
option(GAVEN_LOG_TO_CMD "Log to Cmd" ON)
if(GAVEN_LOG_TO_CMD)
    target_compile_definitions(gaven PUBLIC GAVEN_LOG_TO_CMD=1)
else()
    target_compile_definitions(gaven PUBLIC GAVEN_LOG_TO_CMD=0)
endif()
if(WIN32)
    target_link_libraries(gaven PRIVATE ws2_32)
endif()