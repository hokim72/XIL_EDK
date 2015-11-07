cmake_minimum_required(VERSION 3.1)

set(SYSTEM_NAME mb_system)
set(PROCESSOR microblaze_0)
set(BSP standalone_bsp_0)

set(MCU v8.40.b)

set(TARGET bootloader)

set(CPP_FLAGS -DBOOT_DEBUG)
string(REPLACE ";" " " CPP_FLAGS "${CPP_FLAGS}")

set(C_FLAGS
	-Wall -Os -fmessage-length=0
	-mlittle-endian
	-mno-xl-soft-mul
	-mxl-barrel-shift
	-mxl-pattern-compare
	-mno-xl-soft-div
	-mcpu=${MCU}
	-MMD -MP
	-std=gnu99
)
string(REPLACE ";" " " C_FLAGS "${C_FLAGS}")

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/../${BSP}/${PROCESSOR}/include)

set(LD_FLAGS
	-mlittle-endian
	-mno-xl-soft-mul
	-mxl-barrel-shift
	-mxl-pattern-compare
	-mno-xl-soft-div
	-mcpu=${MCU}
	-L${CMAKE_SOURCE_DIR}/../${BSP}/${PROCESSOR}/lib
	#-T${CMAKE_SOURCE_DIR}/../${BSP}/lscript.ld
	-T${CMAKE_SOURCE_DIR}/lscript.ld
)
string(REPLACE ";" " " LD_FLAGS "${LD_FLAGS}")

set(SRCS ${TARGET}.c SF_commands.c)
#set_source_files_properties(${SRCS} PROPERTIES COMPILE_FLAGS "${CPP_FLAGS} ${C_FLAGS}")

set(CMAKE_EXE_LINKER_FLAGS "${LD_FLAGS}")
add_executable(${TARGET}.elf ${SRCS})
set_target_properties(${TARGET}.elf PROPERTIES COMPILE_FLAGS "${CPP_FLAGS} ${C_FLAGS}")
target_link_libraries(${TARGET}.elf xil)
#set(CPP_FLAGS -DBOOT_DEBUG -DUSE_FS)
add_executable(${TARGET}_fs.elf ${SRCS})
set_target_properties(${TARGET}_fs.elf PROPERTIES COMPILE_FLAGS "${CPP_FLAGS} ${C_FLAGS} -DUSE_FS")
target_link_libraries(${TARGET}_fs.elf xil)

add_custom_target(${TARGET} ALL
	COMMAND ${SIZE} ${TARGET}.elf | tee ${TARGET}.elf.size
	COMMAND ${SIZE} ${TARGET}_fs.elf | tee ${TARGET}_fs.elf.size
	COMMAND ${CMAKE_COMMAND} -E copy ${TARGET}.elf ${CMAKE_SOURCE_DIR}/${TARGET}.elf
	COMMAND ${CMAKE_COMMAND} -E copy ${TARGET}_fs.elf ${CMAKE_SOURCE_DIR}/${TARGET}_fs.elf
	DEPENDS ${TARGET}.elf ${TARGET}_fs.elf
)
		
add_custom_target(download-bit
	COMMAND ${IMPACT} -batch $ENV{XIL_EDK}/download.cmd
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../${SYSTEM_NAME}_hw_platform
	DEPENDS ${CMAKE_SOURCE_DIR}/../${SYSTEM_NAME}_hw_platform/download.bit
)

add_custom_target(download-elf
	COMMAND ${XMD} -hw ${CMAKE_SOURCE_DIR}/../${SYSTEM_NAME}_hw_platform/system.xml -tcl $ENV{XIL_EDK}/xmd-downloadelf.tcl ${TARGET}.elf
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS ${TARGET}
)

add_custom_target(download-fs-elf
	COMMAND ${XMD} -hw ${CMAKE_SOURCE_DIR}/../${SYSTEM_NAME}_hw_platform/system.xml -tcl $ENV{XIL_EDK}/xmd-downloadelf.tcl ${TARGET}_fs.elf
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS ${TARGET}
)

set(ADD_CLEAN_FILES ${CMAKE_SOURCE_DIR}/${TARGET}.elf
					${CMAKE_SOURCE_DIR}/${TARGET}_fs.elf
					${TARGET}.elf.size
					${TARGET}_fs.elf.size
)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${ADD_CLEAN_FILES}")
