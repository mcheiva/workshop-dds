cmake_minimum_required(VERSION 3.27)
project(DDSIDLTypes  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(fastdds CONFIG REQUIRED)
find_program(FASTDDSGEN_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/Fast-DDS-Gen/scripts/fastddsgen)

message(STATUS "Configuring Fast DDS IDL code generation")
message(STATUS "fastddsgen found at: ${FASTDDSGEN_EXECUTABLE}")

set(GENERATED_TYPE_SUPPORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated)

set(WORK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/idl/)
file(GLOB_RECURSE IDL_FILES CONFIGURE_DEPENDS "${WORK_DIR}/*.idl")

# Final check
if (IDL_FILES STREQUAL "")
    message(FATAL_ERROR "No IDL files found in ${WORK_DIR}")
endif()

set(ALL_GENERATED_FILES "")
foreach(IDL_FILE ${IDL_FILES})
	file(RELATIVE_PATH REL_PATH ${WORK_DIR} ${IDL_FILE})
    get_filename_component(IDL_NAME ${IDL_FILE} NAME_WE)
	get_filename_component(REL_DIR ${REL_PATH} DIRECTORY)

	# Compute output subdirectory path
    set(OUTPUT_SUBDIR "${GENERATED_TYPE_SUPPORT_DIR}/${REL_DIR}")
	
    set(GENERATED_PUBTYPE_FILE "${OUTPUT_SUBDIR}/${IDL_NAME}PubSubTypes.cxx")
    set(GENERATED_TYPEOBJ_FILE "${OUTPUT_SUBDIR}/${IDL_NAME}TypeObjectSupport.cxx")

    list(APPEND ALL_GENERATED_FILES ${GENERATED_PUBTYPE_FILE} ${GENERATED_TYPEOBJ_FILE})
endforeach()

add_custom_command(
        OUTPUT ${ALL_GENERATED_FILES}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${GENERATED_TYPE_SUPPORT_DIR}"
        COMMAND ${FASTDDSGEN_EXECUTABLE} -replace -d "${GENERATED_TYPE_SUPPORT_DIR}" -csharp ${IDL_FILES}
		WORKING_DIRECTORY ${WORK_DIR}
        DEPENDS ${IDL_FILES}
        COMMENT "Generating ePromisa FastDDS C++ sources ..."
        VERBATIM
    )

set_source_files_properties(${ALL_GENERATED_FILES} PROPERTIES GENERATED TRUE)

add_custom_target(generate_dds_types ALL
    DEPENDS ${ALL_GENERATED_FILES}
)