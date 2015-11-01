
/*******************************************************************
*
* CAUTION: This file is automatically generated by libgen.
* Version: Xilinx EDK 14.3 EDK_P.40xd
* DO NOT EDIT.
*
* Copyright (c) 1995-2012 Xilinx, Inc.  All rights reserved.

* 
* Description: FreeRTOS Configuration parameters
*
*******************************************************************/

#ifndef _FREERTOSCONFIG_H
#define _FREERTOSCONFIG_H


#include "xparameters.h" 

#define configUSE_PREEMPTION 1

#define configUSE_MUTEXES 1

#define configUSE_RECURSIVE_MUTEXES 1

#define configUSE_COUNTING_SEMAPHORES 1

#define configUSE_TIMERS 1

#define configUSE_IDLE_HOOK 0

#define configUSE_TICK_HOOK 1

#define configUSE_MALLOC_FAILED_HOOK 1

#define configUSE_TRACE_FACILITY 1

#define configUSE_16_BIT_TICKS 0

#define configUSE_APPLICATION_TASK_TAG 0

#define configUSE_CO_ROUTINES 0

#define configTICK_RATE_HZ 10

#define configMAX_PRIORITIES 4

#define configMAX_CO_ROUTINE_PRIORITIES 2

#define configMINIMAL_STACK_SIZE 120

#define configTOTAL_HEAP_SIZE 65536

#define configMAX_TASK_NAME_LEN 8

#define configIDLE_SHOULD_YIELD 1

#define configCHECK_FOR_STACK_OVERFLOW 2

#define configQUEUE_REGISTRY_SIZE 10

#define configGENERATE_RUN_TIME_STATS 0

#define configTIMER_TASK_PRIORITY 10

#define configTIMER_QUEUE_LENGTH 10

#define configTIMER_TASK_STACK_DEPTH 120

#define configINSTALL_EXCEPTION_HANDLERS 1

#define configINTERRUPT_CONTROLLER_TO_USE XPAR_INTC_SINGLE_DEVICE_ID

#define INCLUDE_vTaskCleanUpResources 0

#define INCLUDE_vTaskDelay 1

#define INCLUDE_vTaskDelayUntil 1

#define INCLUDE_vTaskDelete 1

#define INCLUDE_xTaskGetCurrentTaskHandle 1

#define INCLUDE_xTaskGetIdleTaskHandle 1

#define INCLUDE_xTaskGetSchedulerState 1

#define INCLUDE_xTimerGetTimerDaemonTaskHandle 1

#define INCLUDE_uxTaskGetStackHighWaterMark 1

#define INCLUDE_uxTaskPriorityGet 1

#define INCLUDE_vTaskPrioritySet 1

#define INCLUDE_xTaskResumeFromISR 1

#define INCLUDE_vTaskSuspend 1

#define INCLUDE_pcTaskGetTaskName 1

#define INCLUDE_xTaskGetIdleTaskHandle 1

#define INCLUDE_xTimerGetTimerDaemonTaskHandle 1

#endif
