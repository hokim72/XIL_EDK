#BEGIN OS
# PARAMETER OS_NAME = freertos
# PARAMETER OS_VER = 2.00.a
# PARAMETER PROC_INSTANCE = microblaze_0
# PARAMETER STDIN = usb_uart
# PARAMETER STDOUT = usb_uart
# PARAMETER SYSTMR_INTERVAL=1000
## PARAMETER USE_IDLE_HOOK = true
## PARAMETER USE_TICK_HOOK = true
#END

#BEGIN OS
# PARAMETER OS_NAME = xilkernel
# PARAMETER OS_VER = 5.01.a
# PARAMETER PROC_INSTANCE = microblaze_0
# PARAMETER STDIN = usb_uart
# PARAMETER STDOUT = usb_uart
# PARAMETER SYSTMR_SPEC = true
# PARAMETER SYSTMR_DEV = axi_timer_0
# PARAMETER SYSINTC_SPEC = microblaze_0_intc
# PARAMETER MAX_PTHREADS = 20
# PARAMETER PTHREAD_STACK_SIZE = 32768
# PARAMETER CONFIG_PTHREAD_MUTEX = true
# PARAMETER MAX_PTHREAD_MUTEX = 50
# PARAMETER MAX_PTHREAD_MUTEX_WAITQ = 20
# PARAMETER CONFIG_TIME = true
# PARAMETER MAX_TMRS = 50
# PARAMETER CONFIG_SEMA = true
# PARAMETER MAX_SEM = 50
# PARAMETER MAX_SEM_WAITQ = 20
# PARAMETER STATIC_PTHREAD_TABLE = ((main_thread,1))
#END
