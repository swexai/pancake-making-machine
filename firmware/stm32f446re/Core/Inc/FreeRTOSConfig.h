/**
 * @file FreeRTOSConfig.h
 * @brief FreeRTOS configuration for STM32F446RE
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdint.h>

/* ============================================================================
 * FREERTOS KERNEL CONFIGURATION
 * ========================================================================= */

/* Minimal stack size for idle and other kernel tasks */
#define configMINIMAL_STACK_SIZE            128U

/* Total heap size available for FreeRTOS (STM32F446 has 128 kB RAM) */
#define configTOTAL_HEAP_SIZE              (32 * 1024)  /* 32 kB for FreeRTOS tasks */

/* Maximum number of priorities (0-4, where 4 is highest) */
#define configMAX_PRIORITIES                6

/* Tick rate in Hz */
#define configTICK_RATE_HZ                  1000

/* System clock frequency - STM32F446 at 180 MHz */
#define configCPU_CLOCK_HZ                  ((unsigned long) 180000000UL)

/* ============================================================================
 * FREERTOS FEATURE CONFIGURATION
 * ========================================================================= */

/* Enable task creation at runtime */
#define configSUPPORT_DYNAMIC_ALLOCATION    1

/* Time slicing (round-robin) enabled */
#define configUSE_PREEMPTION                1

/* Use ticklish idle hook */
#define configUSE_IDLE_HOOK                 0

/* Use tick hook */
#define configUSE_TICK_HOOK                 0

/* Use event groups */
#define configUSE_EVENT_GROUPS              0

/* Use counting semaphores */
#define configUSE_COUNTING_SEMAPHORES       1

/* Use binary semaphores */
#define configUSE_BINARY_SEMAPHORES         1

/* Use mutexes */
#define configUSE_MUTEXES                   1

/* Use recursive mutexes */
#define configUSE_RECURSIVE_MUTEXES         0

/* Use queues */
#define configUSE_QUEUE_SETS                0

/* Use task notifications */
#define configUSE_TASK_NOTIFICATIONS        1

/* Use timers */
#define configUSE_TIMERS                    0

/* Task name length */
#define configMAX_TASK_NAME_LEN             16

/* Trace/debug macros */
#define configUSE_TRACE_FACILITY            0
#define configUSE_STATS_FORMATTING_FUNCTIONS  0

/* ============================================================================
 * ASSERT AND DEBUG CONFIGURATION
 * ========================================================================= */

#ifdef DEBUG
  #define configASSERT(x) if((x)==0) {taskDISABLE_INTERRUPTS(); for(;;);}
#else
  #define configASSERT(x)
#endif

/* ============================================================================
 * MEMORY ALLOCATION SCHEME
 * ========================================================================= */

/* Use heap_4.c (best for mixed allocation sizes) */
#define configFREERTOS_MEMORY_SCHEME        4

/* ============================================================================
 * PORT AND COMPILER SPECIFIC
 * ========================================================================= */

/* Port selection (ARM Cortex-M4 for STM32F446) */
#define configKERNEL_INTERRUPT_PRIORITY     191

#define configMAX_SYSCALL_INTERRUPT_PRIORITY  32

/* SysTick handler priority (must be lowest) */
#define xPortPendSVHandler SVC_Handler
#define xPortSysTickHandler SysTick_Handler

/* ============================================================================
 * COMPILER SPECIFIC
 * ========================================================================= */

#define configUSE_PORT_OPTIMISED_TASK_SELECTION  0

/* ============================================================================
 * RUN-TIME CONFIGURATION OPTIONS
 * ========================================================================= */

/* Generate run-time stats (optional, for performance monitoring) */
#define configGENERATE_RUN_TIME_STATS       0

/* Thread-local storage pointer count */
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS  0

/* ============================================================================
 * FREERTOS HOOKS FOR SYSTEM MONITORING
 * ========================================================================= */

/* Override malloc failed hook */
extern void vApplicationMallocFailedHook(void);
#define configUSE_MALLOC_FAILED_HOOK        1

/* Stack overflow hook */
extern void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
#define configCHECK_FOR_STACK_OVERFLOW      2

#endif /* FREERTOS_CONFIG_H */
