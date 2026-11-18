# Each source is a standalone test.
file(GLOB FALCON_TEST_SOURCES
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
)

if(NOT FALCON_TEST_SOURCES)
    return()
endif()

# Sort the test sources to ensure consistent build order across different platforms and filesystems.
list(SORT FALCON_TEST_SOURCES)

# Redefine the __FILE__ macro for each test source file to ensure that the correct relative path is used in the test output.
redefine_file_macro("${FALCON_TEST_SOURCES}")

find_package(PkgConfig REQUIRED)
pkg_check_modules(URCU REQUIRED IMPORTED_TARGET liburcu)
find_package(Threads REQUIRED)

foreach(TEST_SOURCE IN LISTS FALCON_TEST_SOURCES)
    get_filename_component(TEST_NAME "${TEST_SOURCE}" NAME_WE)

    if(TARGET "${TEST_NAME}")
        message(FATAL_ERROR "Duplicate target: ${TEST_NAME}")
    endif()

    add_executable("${TEST_NAME}" "${TEST_SOURCE}")

    target_link_libraries("${TEST_NAME}"
        PRIVATE falcon Threads::Threads PkgConfig::URCU
    )

    # Keep build configurations separate.
    set_target_properties("${TEST_NAME}" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/tests/$<CONFIG>"
    )

    add_test(NAME "${TEST_NAME}" COMMAND "${TEST_NAME}")
endforeach()

add_subdirectory(inner_tests)