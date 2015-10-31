
 PARAMETER VERSION = 2.2.0


#BEGIN OS
# PARAMETER OS_NAME = standalone
# PARAMETER OS_VER = 3.07.a
# PARAMETER PROC_INSTANCE = microblaze_0
# PARAMETER STDIN = usb_uart
# PARAMETER STDOUT = usb_uart
#END


BEGIN PROCESSOR
 PARAMETER DRIVER_NAME = cpu
 PARAMETER DRIVER_VER = 1.14.a
 PARAMETER HW_INSTANCE = microblaze_0
END


BEGIN DRIVER
 PARAMETER DRIVER_NAME = tmrctr
 PARAMETER DRIVER_VER = 2.04.a
 PARAMETER HW_INSTANCE = axi_timer_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = uartlite
 PARAMETER DRIVER_VER = 2.00.a
 PARAMETER HW_INSTANCE = debug_module
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = gpio
 PARAMETER DRIVER_VER = 3.00.a
 PARAMETER HW_INSTANCE = dip_switches_4bits
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = emaclite
 PARAMETER DRIVER_VER = 3.03.a
 PARAMETER HW_INSTANCE = ethernet_mac
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = gpio
 PARAMETER DRIVER_VER = 3.00.a
 PARAMETER HW_INSTANCE = leds_4bits
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = s6_ddrx
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = mcb3_lpddr
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = bram
 PARAMETER DRIVER_VER = 3.01.a
 PARAMETER HW_INSTANCE = microblaze_0_d_bram_ctrl
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = bram
 PARAMETER DRIVER_VER = 3.01.a
 PARAMETER HW_INSTANCE = microblaze_0_i_bram_ctrl
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = intc
 PARAMETER DRIVER_VER = 2.05.a
 PARAMETER HW_INSTANCE = microblaze_0_intc
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = spi
 PARAMETER DRIVER_VER = 3.04.a
 PARAMETER HW_INSTANCE = spi_flash
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = uartlite
 PARAMETER DRIVER_VER = 2.00.a
 PARAMETER HW_INSTANCE = usb_uart
END


BEGIN OS
 PARAMETER OS_NAME = freertos
 PARAMETER OS_VER = 2.00.a
 PARAMETER PROC_INSTANCE = microblaze_0
 PARAMETER STDIN = usb_uart
 PARAMETER STDOUT = usb_uart
 PARAMETER USE_IDLE_HOOK = true
END


# lwIP TCP/IP Stack library: lwIP v1.4.0, Xilinx adapter v1.03.a
BEGIN LIBRARY
 PARAMETER LIBRARY_NAME = lwip141
 #PARAMETER LIBRARY_NAME = lwip140
 #PARAMETER LIBRARY_VER = 1.03.a
 PARAMETER PROC_INSTANCE = microblaze_0
 PARAMETER N_TX_DESCRIPTORS = 256
 PARAMETER N_RX_DESCRIPTORS = 256
 PARAMETER PHY_LINK_SPEED = CONFIG_LINKSPEED100
 PARAMETER MEMP_N_PBUF = 1024
 PARAMETER MEMP_N_TCP_SEG = 1024
 PARAMETER PBUF_POOL_SIZE = 1024
 PARAMETER TCP_WND = 4096
 PARAMETER TCP_MSS = 1450
 PARAMETER LWIP_DHCP = true
END

# Provides read/write routines to access files stored on a FAT16/32 file system.
# Requires SystemACE lower level drivers.
#BEGIN LIBRARY
# PARAMETER LIBRARY_NAME = xilfatfs
# PARAMETER LIBRARY_VER = 1.00.a
# PARAMETER PROC_INSTANCE = microblaze_0
#END

# Xilinx Flash library for Intel/AMD CFI compliant parallel flash
BEGIN LIBRARY
 PARAMETER LIBRARY_NAME = xilflash
 PARAMETER LIBRARY_VER = 3.02.a
 PARAMETER PROC_INSTANCE = microblaze_0
END

# Xilinx In-system and Serial Flash Library
BEGIN LIBRARY
 PARAMETER LIBRARY_NAME = xilisf
 PARAMETER LIBRARY_VER = 3.00.a
 PARAMETER PROC_INSTANCE = microblaze_0
END

# Xilinx Memory File System
BEGIN LIBRARY
 PARAMETER LIBRARY_NAME = xilmfs
 PARAMETER LIBRARY_VER = 1.00.a
 PARAMETER PROC_INSTANCE = microblaze_0
 PARAMETER NUMBYTES = 2048000
 PARAMETER BASE_ADDRESS = 0xa5000000
 PARAMETER INIT_TYPE = MFSINIT_IMAGE
 PARAMETER NEED_UTILS = true
END
