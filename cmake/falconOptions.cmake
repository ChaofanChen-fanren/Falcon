add_library(falcon_options INTERFACE)

target_compile_options(falcon_options INTERFACE
    # Common options for GCC and Clang
    "$<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-fno-strict-aliasing>"

    # Position-independent code on Linux
    "$<$<AND:$<PLATFORM_ID:Linux>,$<CXX_COMPILER_ID:GNU,Clang>>:-fPIC>"

    # Debug options
    "$<$<CONFIG:Debug>:-O0;-g3;-ggdb>"

    # Options shared by Release and RelWithDebInfo
    "$<$<CONFIG:Release,RelWithDebInfo>:-O2;-fno-omit-frame-pointer>"

    # Debug information for optimized builds
    "$<$<CONFIG:RelWithDebInfo>:-g>"
)

target_compile_definitions(falcon_options INTERFACE
    "$<$<CONFIG:Debug>:FALCON_DEBUG>"
    "$<$<CONFIG:Release,RelWithDebInfo>:NDEBUG>"
)

target_link_options(falcon_options INTERFACE
    # Export symbols to help resolve names in runtime backtraces
    "$<$<AND:$<PLATFORM_ID:Linux>,$<CXX_COMPILER_ID:GNU,Clang>>:-rdynamic>"
)

# Optional code coverage instrumentation for Debug builds
option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)

if(ENABLE_COVERAGE)
    target_compile_options(falcon_options INTERFACE
        "$<$<CONFIG:Debug>:--coverage>"
    )

    target_link_options(falcon_options INTERFACE
        "$<$<CONFIG:Debug>:--coverage>"
    )
endif()