cmake_minimum_required (VERSION 3.27)
project(demo_idl CXX)

find_program(FASTDDSGEN fastddsgen)
message(STATUS "Configuring Fast DDS IDL code generation")

if (NOT FASTDDSGEN)
	message(FATAL_ERROR "fastddsgen not found. Please install Fast DDS and add it to your PATH.")
endif()

set(GENERATED_TYPE_SUPPORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../Solution/IDL/idls/)

set (IDL_FILES
    Awesome.idl
)

set(ALL_GENERATED_FILES
    ${GENERATED_TYPE_SUPPORT_DIR}/AwesomePubSubTypes.cxx
	${GENERATED_TYPE_SUPPORT_DIR}/AwesomeTypeObjectSupport.cxx
)

add_custom_command(
        OUTPUT ${ALL_GENERATED_FILES}
        COMMAND ${FASTDDSGEN} -replace -d "${GENERATED_TYPE_SUPPORT_DIR}" ${IDL_FILES}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${IDL_FILES}
        COMMENT "Generating ePromisa FastDDS C++ sources ..."
        VERBATIM
    )

add_library(${PROJECT_NAME} ${ALL_GENERATED_FILES})

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${GENERATED_TYPE_SUPPORT_DIR}/>
)

target_link_libraries(${PROJECT_NAME} PUBLIC fastdds::fastdds)
