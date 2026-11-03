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

# Treat warnings as errors when building the library
target_compile_options(falcon PRIVATE
    "$<$<CXX_COMPILER_ID:GNU,Clang>:-Werror>"
)

# Install library artifacts
install(TARGETS falcon
    EXPORT falconTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install header files
install(FILES ${FALCON_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/falcon
)

# add_executable(falcon_main main.cpp)
# target_link_libraries(falcon_main PRIVATE falcon_options)