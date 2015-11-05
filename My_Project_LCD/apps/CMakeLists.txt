cmake_minimum_required(VERSION 3.1)

set(SYSTEM_NAME mb_system)
set(PROCESSOR microblaze_0)
set(APPLICATION_NAME empty_application)
# Application Name : dhrystone, empty_application, hello_world, lwip_echo_server(lwip140), memory_tests, peripheral_tests, srec_bootloader, xilkernel_posix_threads_demo, zynq_fsbl
set(BSP standalone_bsp_0)

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/${SYSTEM_NAME}_hw_platform)
execute_process(
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMAND ${CMAKE_COMMAND} -E make_directory ${SYSTEM_NAME}_hw_platform
	COMMAND ${CMAKE_COMMAND} -E copy ../hardware/SDK_Export/hw/${SYSTEM_NAME}.xml  ${SYSTEM_NAME}_hw_platform/system.xml
	COMMAND ${CMAKE_COMMAND} -E copy ../hardware/SDK_Export/hw/${SYSTEM_NAME}.bit ${SYSTEM_NAME}_hw_platform/system.bit
	COMMAND ${CMAKE_COMMAND} -E copy ../hardware/SDK_Export/hw/${SYSTEM_NAME}_bd.bmm  ${SYSTEM_NAME}_hw_platform/system_bd.bmm
)
execute_process(
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/${SYSTEM_NAME}_hw_platform
	COMMAND ${ELFCHECK} -hw system.xml -mode bootload -mem BRAM -pe ${PROCESSOR} $ENV{XIL_ISE_ROOT}/EDK/sw/lib/microblaze/mb_bootloop_le.elf
	COMMAND ${DATA2MEM} -bm system_bd.bmm -bt system.bit -bd $ENV{XIL_ISE_ROOT}/EDK/sw/lib/microblaze/mb_bootloop_le.elf tag ${PROCESSOR} -o b download.bit
)
endif()

execute_process(
	COMMAND ${APPGURU} -hw ${CMAKE_SOURCE_DIR}/${SYSTEM_NAME}_hw_platform/system.xml -lp $ENV{XIL_EDK}/lib -app ${APPLICATION_NAME}
)
execute_process(
	COMMAND ${CMAKE_COMMAND} -E rename Makefile Makefile.app
)
execute_process(
	COMMAND ${CMAKE_COMMAND} -E rename system.mss system.mss.orig
)

file(WRITE ${CMAKE_BINARY_DIR}/concat.cmake "
FILE(READ \${SRC1} S1)
FILE(READ \${SRC2} S2)
FILE(WRITE \${DST} \${S1}\${S2})
")

add_custom_command(
	COMMAND ${CMAKE_COMMAND} -D SRC1=${CMAKE_BINARY_DIR}/system.mss.orig
							 -D SRC2=${CMAKE_SOURCE_DIR}/system.mss
							 -D DST=${CMAKE_BINARY_DIR}/system.mss
							 -P ${CMAKE_BINARY_DIR}/concat.cmake
	OUTPUT ${CMAKE_BINARY_DIR}/system.mss
)

add_custom_target(libxil ALL
	COMMAND ${CMAKE_MAKE_PROGRAM} -f Makefile.app ${PROCESSOR}/lib/libxil.a
	DEPENDS ${CMAKE_BINARY_DIR}/system.mss
)

install(FILES ${CMAKE_BINARY_DIR}/system.mss ${CMAKE_BINARY_DIR}/lscript.ld DESTINATION ${CMAKE_SOURCE_DIR}/${BSP})
install(DIRECTORY ${CMAKE_BINARY_DIR}/${PROCESSOR} DESTINATION ${CMAKE_SOURCE_DIR}/${BSP})

set(ADD_CLEAN_FILES
	#${CMAKE_SOURCE_DIR}/${SYSTEM_NAME}_hw_platform
	#${CMAKE_SOURCE_DIR}/${BSP}
)

# Add clean files
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${ADD_CLEAN_FILES}")

