#ifndef __PLATFORM_CONFIG_H_
#define __PLATFORM_CONFIG_H_

#define STDOUT_IS_UARTLITE
#ifdef __PPC__
#define CACHEABLE_REGION_MASK 0xffffffff80000800
#endif


#endif