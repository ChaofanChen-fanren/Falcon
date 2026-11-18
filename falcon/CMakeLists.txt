# Collect source and header files
file(GLOB_RECURSE FALCON_SOURCES CONFIGURE_DEPENDS
    "*.cpp"
    "*.cc"
    "*.c"
)

file(GLOB_RECURSE FALCON_HEADERS CONFIGURE_DEPENDS
    "*.h"
    "*.hpp"
)

# Falcon staic library
add_library(falcon STATIC ${FALCON_SOURCES} ${FALCON_HEADERS})

# Redefine the __FILE__ macro for each source file.
redefine_file_macro("${FALCON_SOURCES}")

# Set C++ standard to C++20
target_compile_features(falcon PUBLIC cxx_std_20)

# Public and internal include directories
target_include_directories(falcon
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Apply common build settings and propagate them to consumers
target_link_libraries(falcon PUBLIC falcon_options)

# Link against the Threads library for multithreading support
target_link_libraries(falcon PUBLIC Threads::Threads)

# Treat warnings as errors when building the library
target_compile_options(falcon PRIVATE
    "$<$<CXX_COMPILER_ID:GNU,Clang>:-Werror>"
)

# Link against platform-specific libraries
if(WIN32)
    # Link against dbghelp on Windows for stack trace support
    target_link_libraries(falcon PRIVATE dbghelp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Use pkg-config to find libdw for stack trace support on Linux
    target_link_libraries(falcon PRIVATE PkgConfig::FALCON_LIBDW)
endif()

# Link against bundled dependencies
target_link_libraries(falcon PUBLIC yaml-cpp::yaml-cpp)
add_dependencies(falcon falcon_third_party_dependencies)

# Install library artifacts
install(TARGETS falcon
    EXPORT falconTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install the falcon_options target for consumers to use
install(TARGETS falcon_options EXPORT falconTargets)

# Install header files
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# add_executable(falcon_main main.cpp)
# target_link_libraries(falcon_main PRIVATE falcon_options)