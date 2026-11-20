if(NOT DEFINED FALCON_DEPS_STAMP_FILE OR NOT DEFINED FALCON_DEPS_STAMP_VALUE)
    message(FATAL_ERROR "Dependency stamp file and value are required")
endif()

get_filename_component(stamp_directory "${FALCON_DEPS_STAMP_FILE}" DIRECTORY)
file(MAKE_DIRECTORY "${stamp_directory}")
file(WRITE "${FALCON_DEPS_STAMP_FILE}" "${FALCON_DEPS_STAMP_VALUE}\n")