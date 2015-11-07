cmake_minimum_required(VERSION 3.1)

# Use folder name for project name
project(My_Project_IIC_TEMP NONE)

message(STATUS "Generating targets for demo ${PROJECT_NAME}!")

set(SYSTEM_NAME mb_system)

add_custom_target(
	copy-files
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${SYSTEM_NAME}.mhs ${CMAKE_BINARY_DIR}/${SYSTEM_NAME}.mhs
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/${SYSTEM_NAME}.xmp ${CMAKE_BINARY_DIR}/${SYSTEM_NAME}.xmp
	#OUTPUT ${CMAKE_BINARY_DIR}/${SYSTEM_NAME}.xmp
)

add_custom_command(
	COMMAND ${XPS} -nw -scr $ENV{XIL_EDK}/xps-genmakefile.tcl mb_system.xmp
	COMMAND ${CMAKE_MAKE_PROGRAM} -f ${SYSTEM_NAME}.make bits
	COMMAND ${CMAKE_MAKE_PROGRAM} -f ${SYSTEM_NAME}.make exporttosdk
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	#DEPENDS ${CMAKE_BINARY_DIR}/${SYSTEM_NAME}.xmp
	DEPENDS copy-files
	OUTPUT ${CMAKE_BINARY_DIR}/SDK/SDK_Export
)

add_custom_target(
	exporttosdk
	ALL
	DEPENDS ${CMAKE_BINARY_DIR}/SDK/SDK_Export 
)

install(DIRECTORY ${CMAKE_BINARY_DIR}/SDK/SDK_Export 
		DESTINATION ${CMAKE_SOURCE_DIR}
)

set(ADD_CLEAN_FILES 
		${CMAKE_BINARY_DIR}/clock_generator_0.log
		${CMAKE_BINARY_DIR}/hdl
		${CMAKE_BINARY_DIR}/implementation
		${CMAKE_BINARY_DIR}/${SYSTEM_NAME}_incl.make
		${CMAKE_BINARY_DIR}/${SYSTEM_NAME}.log
		${CMAKE_BINARY_DIR}/${SYSTEM_NAME}.make
		${CMAKE_BINARY_DIR}/platgen.log
		${CMAKE_BINARY_DIR}/platgen.opt
		${CMAKE_BINARY_DIR}/psf2Edward.log
		${CMAKE_BINARY_DIR}/SDK
		${CMAKE_BINARY_DIR}/synthesis
		${CMAKE_BINARY_DIR}/xdsgen.log
		${CMAKE_BINARY_DIR}/__xps
)

# Add clean files
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${ADD_CLEAN_FILES}")

