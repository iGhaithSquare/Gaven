include(FetchContent)
set(ENABLE_CJSON_TEST OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    cjson
    GIT_REPOSITORY https://github.com/davegamble/cjson.git
    GIT_TAG v1.7.19
)
FetchContent_MakeAvailable(cjson)
target_compile_options(cjson PRIVATE
    -w
)
