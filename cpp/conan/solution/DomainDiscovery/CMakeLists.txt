cmake_minimum_required (VERSION 3.27)
project(DomainDiscovery CXX)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ddsbus)

# Path to the XML file
set(QOS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../config/EIVAQos.xml)

# Copy Qos.xml next to the executable after build
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${QOS_FILE}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/EIVAQos.xml
)
