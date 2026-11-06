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

find_package(Threads REQUIRED)

foreach(TEST_SOURCE IN LISTS FALCON_TEST_SOURCES)
    get_filename_component(TEST_NAME "${TEST_SOURCE}" NAME_WE)

    if(TARGET "${TEST_NAME}")
        message(FATAL_ERROR "Duplicate target: ${TEST_NAME}")
    endif()

    add_executable("${TEST_NAME}" "${TEST_SOURCE}")

    target_link_libraries("${TEST_NAME}"
        PRIVATE falcon Threads::Threads
    )

    # Keep build configurations separate.
    set_target_properties("${TEST_NAME}" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/tests/$<CONFIG>"
    )

    add_test(NAME "${TEST_NAME}" COMMAND "${TEST_NAME}")
endforeach()