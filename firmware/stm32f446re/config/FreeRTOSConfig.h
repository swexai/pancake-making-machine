/**
 * @file FreeRTOSConfig.h
 * @brief FreeRTOS configuration for STM32F446RE
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "stm32f4xx.h"

/* ==================== Basic Configuration ============================== */

#define configUSE_PREEMPTION                       1
#define configUSE_IDLE_HOOK                        0
#define configUSE_TICK_HOOK                        0
#define configCPU_CLOCK_HZ                         180000000
#define configTICK_RATE_HZ                         1000
#define configMAX_PRIORITIES                       6
#define configMINIMAL_STACK_SIZE                   128
#define configTOTAL_HEAP_SIZE                      20480 /* 20 KB */
#define configMAX_TASK_NAME_LEN                    16
#define configUSE_TRACE_FACILITY                   0
#define configUSE_16_BIT_TICKS                     0
#define configIDLE_SHOULD_YIELD                    1
#define configUSE_MUTEXES                          1
#define configQUEUE_REGISTRY_SIZE                  8
#define configCHECK_FOR_STACK_OVERFLOW             2
#define configUSE_RECURSIVE_MUTEXES                1
#define configUSE_COUNTING_SEMAPHORES              1

/* ==================== Memory Management =============================== */

#define configAPPLICATION_ALLOCATED_HEAP           0
#define configSUPPORT_STATIC_ALLOCATION            0
#define configSUPPORT_DYNAMIC_ALLOCATION           1

/* ==================== FreeRTOS+ Features ============================== */

#define configUSE_TASK_FPU_ENTRY_MACRO             1

/* ==================== Hook Functions ================================== */

#define configUSE_MALLOC_FAILED_HOOK               1

/* Handle stack overflow */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* ==================== CMSIS RTOS v2 API =============================== */

#define configUSE_TASK_NOTIFICATIONS               1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES      1
#define configUSE_NEWLIB_REENTRANT                 1

/* ==================== Timer Service Daemon Task ======================= */

#define configUSE_TIMERS                           1
#define configTIMER_TASK_PRIORITY                  4
#define configTIMER_QUEUE_LENGTH                   10
#define configTIMER_TASK_STACK_DEPTH               512

/* ==================== Event Groups ================================== */

#define configUSE_EVENT_GROUPS                     1

/* ==================== Co-Routines ================================== */

#define configUSE_CO_ROUTINES                      0
#define configMAX_CO_ROUTINE_PRIORITIES            2

/* ==================== Interrupt Handling ============================== */

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY             15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY        5
#define configKERNEL_INTERRUPT_PRIORITY                     (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - __NVIC_PRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY                (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - __NVIC_PRIO_BITS))

/* ==================== Optional FreeRTOS Features ===================== */

#define configUSE_QUEUE_SETS                       0
#define configUSE_TIME_SLICING                     0

/* ==================== Run Time Stats ================================= */

#define configGENERATE_RUN_TIME_STATS              0
#define configUSE_PORT_OPTIMISED_TASK_SELECTION   1

/* ==================== Task Snapshot (Debug) ========================== */

#define configINCLUDE_FREERTOS_TASK_H_ERROR_CHECKS 1

/* ==================== Watchdog Configuration ========================= */

#define configWATCHDOG_ENABLED                     1

/* ==================== Assert Macro ================================== */

#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }

#endif /* FREERTOS_CONFIG_H */
