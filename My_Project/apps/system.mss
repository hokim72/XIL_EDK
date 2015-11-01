#BEGIN OS
# PARAMETER OS_NAME = freertos
# PARAMETER OS_VER = 2.00.a
# PARAMETER PROC_INSTANCE = microblaze_0
# PARAMETER STDIN = usb_uart
# PARAMETER STDOUT = usb_uart
# PARAMETER USE_IDLE_HOOK = true
## PARAMETER USE_TICK_HOOK = true
#END


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
