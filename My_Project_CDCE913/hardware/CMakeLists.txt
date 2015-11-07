cmake_minimum_required(VERSION 3.1)

# Use folder name for project name
project(My_Project_CDCE913 NONE)

message(STATUS "Generating targets for demo ${PROJECT_NAME}!")

set(SYSTEM_NAME mb_system)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/${SYSTEM_NAME} ${CMAKE_BINARY_DIR}/${SYSTEM_NAME}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${SYSTEM_NAME}.ucf ${CMAKE_BINARY_DIR}/${SYSTEM_NAME}.ucf
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${SYSTEM_NAME}_top.vhd ${CMAKE_BINARY_DIR}/${SYSTEM_NAME}_top.vhd
)

add_custom_command( 
	COMMAND  ${XTCLSH} ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.tcl rebuild_project
	OUTPUT ${SYSTEM_NAME}_top.bit
)

add_custom_target(build ALL 
	DEPENDS ${SYSTEM_NAME}_top.bit
)

install(DIRECTORY ${CMAKE_BINARY_DIR}/${SYSTEM_NAME}/SDK/SDK_Export
		DESTINATION ${CMAKE_SOURCE_DIR}
)
