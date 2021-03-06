cmake_minimum_required(VERSION 3.1)

set(SYSTEM_NAME mb_system)
set(PROCESSOR microblaze_0)
set(BSP freertos_bsp_0)

set(MCU v8.40.b)

set(TARGET freertos_hello_world)

set(CPP_FLAGS)

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
	-T${CMAKE_SOURCE_DIR}/../${BSP}/lscript.ld
)
string(REPLACE ";" " " LD_FLAGS "${LD_FLAGS}")

set(SRCS ${TARGET}.c)
set_source_files_properties(${SRCS} PROPERTIES COMPILE_FLAGS "${CPP_FLAGS} ${C_FLAGS}")

add_executable(${TARGET}.elf ${SRCS})
set(CMAKE_EXE_LINKER_FLAGS "${LD_FLAGS}")
target_link_libraries(${TARGET}.elf xil freertos)

add_custom_target(${TARGET} ALL
	COMMAND ${SIZE} ${TARGET}.elf | tee ${TARGET}.elf.size
	COMMAND ${CMAKE_COMMAND} -E copy ${TARGET}.elf ${CMAKE_SOURCE_DIR}/${TARGET}.elf
	#COMMAND ${CMAKE_COMMAND} -E copy ${TARGET}.elf.size ${CMAKE_SOURCE_DIR}/${TARGET}.elf.size
	DEPENDS ${TARGET}.elf
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

file(WRITE ${CMAKE_BINARY_DIR}/generate-prom.cmake "
configure_file(\$ENV{XIL_EDK}/generate-prom.cmd \${CMAKE_BINARY_DIR}/generate-prom.cmd)
")
add_custom_command(
    COMMAND ${DATA2MEM} -bm ${CMAKE_SOURCE_DIR}/../${SYSTEM_NAME}_hw_platform/system_bd.bmm -bt ${CMAKE_SOURCE_DIR}/../${SYSTEM_NAME}_hw_platform/system.bit -bd ${CMAKE_SOURCE_DIR}/../bootloader/bootloader.elf tag ${PROCESSOR} -o b download.bit
    COMMAND ${CMAKE_COMMAND} -D SIZE=16384
                             -P generate-prom.cmake
    COMMAND ${IMPACT} -batch generate-prom.cmd
	OUTPUT bitstream.mcs
)

add_custom_command(
    COMMAND ${OBJCOPY} -O  binary  -R .vectors.reset -R .vectors.sw_exception -R .vectors.interrupt -R .vectors.debug_sw_break -R .vectors.hw_exception ${TARGET}.elf app.b
    COMMAND ${OBJCOPY} -O  binary  -j .vectors.reset -j .vectors.sw_exception -j .vectors.interrupt -j .vectors.debug_sw_break -j .vectors.hw_exception ${TARGET}.elf vectors.b
    DEPENDS ${TARGET}
    COMMAND ${PROMGEN} -w -p mcs -c FF -o data -s 16384 -data_file up 100000 app.b -data_file up 400000 vectors.b -spi
    OUTPUT data.mcs
)

file(WRITE ${CMAKE_BINARY_DIR}/combine.cmake "
FILE(READ \${SRC1} S1)
STRING(REGEX REPLACE \"\\n:00000001FF\" \"\" S1 \${S1})
FILE(READ \${SRC2} S2)
FILE(WRITE \${DST} \${S1}\${S2})
")

add_custom_target(
    generate-mcs
	COMMAND ${CMAKE_COMMAND} -D SRC1=bitstream.mcs
							 -D SRC2=data.mcs
							 -D DST=${TARGET}.mcs
							 -P combine.cmake
    COMMAND ${CMAKE_COMMAND} -E copy ${TARGET}.mcs ${CMAKE_SOURCE_DIR}/${TARGET}.mcs
    DEPENDS bitstream.mcs data.mcs
)

file(WRITE ${CMAKE_BINARY_DIR}/program-prom.cmake "
configure_file(\$ENV{XIL_EDK}/program-prom.cmd \${CMAKE_BINARY_DIR}/program-prom.cmd)
")

add_custom_command(
    COMMAND ${CMAKE_COMMAND} -D FLASH_DEVICE="N25Q128" 
                             -D MCS_FILE=${TARGET}.mcs
                             -P program-prom.cmake
    OUTPUT program-prom.cmd
)

add_custom_target(program-mcs
    COMMAND ${IMPACT} -batch ${CMAKE_BINARY_DIR}/program-prom.cmd
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS generate-mcs program-prom.cmd
)

set(ADD_CLEAN_FILES ${CMAKE_SOURCE_DIR}/${TARGET}.elf 
                    ${CMAKE_SOURCE_DIR}/${TARGET}.mcs
                    ${TARGET}.mcs
                    ${TARGET}.elf.size
                    vectors.b
                    app.b
                    _impactbatch.log
                    bitstream.cfi
                    bitstream.prm
                    data.cfi
                    data.prm
                    download.bit
                    download_cclktemp.bit
)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${ADD_CLEAN_FILES}")
