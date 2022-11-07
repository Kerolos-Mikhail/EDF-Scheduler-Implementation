/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <lpc21xx.h>
#include "GPIO.h"

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/
 
extern unsigned int xButton_1TimeIn;
extern unsigned int xButton_1TimeOut;
extern unsigned int xButton_1TotalTime;

extern unsigned int xButton_2TimeIn;
extern unsigned int xButton_2TimeOut;
extern unsigned int xButton_2TotalTime;

extern unsigned int xPeriodic_TransTimeIn;
extern unsigned int xPeriodic_TransTimeOut;
extern unsigned int xPeriodic_TransTotalTime;

extern unsigned int xUart_ReceiverTimeIn;
extern unsigned int xUart_ReceiverTimeOut;
extern unsigned int xUart_ReceiverTotalTime;

extern unsigned int xLoad_1_SimTimeIn;
extern unsigned int xLoad_1_SimTimeOut;
extern unsigned int xLoad_1_SimTotalTime;

extern unsigned int xLoad_2_SimTimeIn;
extern unsigned int xLoad_2_SimTimeOut;
extern unsigned int xLoad_2_SimTotalTime;

extern unsigned int SystemTime;
extern unsigned int CPULoad;

/*****************************************************************/
#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK			1
#define configUSE_TICK_HOOK			1
#define configCPU_CLOCK_HZ			( ( unsigned long ) 60000000 )	/* =12.0MHz xtal multiplied by 5 using the PLL. */
#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES		( 10 )
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 100 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) 13 * 1024 )
#define configMAX_TASK_NAME_LEN		( 20 )
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		1
#define configUSE_TIME_SLICING    0												/* Configure Round Robin Scheduler */

#define configGENERATE_RUN_TIME_STATS	 1 									/* Configure run time stats */
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()	
#define portGET_RUN_TIME_COUNTER_VALUE()					(T1TC)
#define configUSE_STATS_FORMATTING_FUNCTIONS		1
#define configSUPPORT_DYNAMIC_ALLOCATION				1
#define configUSE_TRACE_FACILITY                1


#define configQUEUE_REGISTRY_SIZE 	0


/* Configuration for EDF Scheduler */
#define configUSE_EDF_SCHEDULER				1  						/* Enable EDF Scheduler */


/* Trace Hooks */
#define traceTASK_SWITCHED_OUT()			do{	\
																				if((int)pxCurrentTCB->pxTaskTag == 1){	\
																					 GPIO_write(PORT_0, PIN2, PIN_IS_LOW);	\
																					 xButton_1TimeOut = T1TC;\
																					 xButton_1TotalTime += (xButton_1TimeOut - xButton_1TimeIn);\
																				}	\
																				else if((int)pxCurrentTCB->pxTaskTag == 2){ \
																					GPIO_write(PORT_0, PIN3, PIN_IS_LOW);\
																					xButton_2TimeOut = T1TC;\
																					xButton_2TotalTime += (xButton_2TimeOut - xButton_2TimeIn);\
																				}	\
																				else if((int)pxCurrentTCB->pxTaskTag == 3){ \
																					GPIO_write(PORT_0, PIN4, PIN_IS_LOW);\
																					xPeriodic_TransTimeOut = T1TC;\
																					xPeriodic_TransTotalTime += (xPeriodic_TransTimeOut - xPeriodic_TransTimeIn);\
																				}	\
																				else if((int)pxCurrentTCB->pxTaskTag == 4){ \
																					GPIO_write(PORT_0, PIN5, PIN_IS_LOW);\
																					xUart_ReceiverTimeOut = T1TC;\
																					xUart_ReceiverTotalTime += (xUart_ReceiverTimeOut - xUart_ReceiverTimeIn);\
																				}	\
																				else if((int)pxCurrentTCB->pxTaskTag == 5){ \
																					GPIO_write(PORT_0, PIN6, PIN_IS_LOW);\
																					xLoad_1_SimTimeOut = T1TC;\
																					xLoad_1_SimTotalTime += (xLoad_1_SimTimeOut - xLoad_1_SimTimeIn);\
																				}	\
																				else if((int)pxCurrentTCB->pxTaskTag == 6){ \
																					GPIO_write(PORT_0, PIN7, PIN_IS_LOW);\
																					xLoad_2_SimTimeOut = T1TC;\
																					xLoad_2_SimTotalTime += (xLoad_2_SimTimeOut - xLoad_2_SimTimeIn);\
																					SystemTime = T1TC;\
																				CPULoad = (((xButton_1TotalTime + xButton_2TotalTime + xPeriodic_TransTotalTime \
																				+ xUart_ReceiverTotalTime + xLoad_1_SimTotalTime + xLoad_2_SimTotalTime) / (float)SystemTime) * 100);\
																				}	\
																			}while(0)

#define traceTASK_SWITCHED_IN()				do{\
																				if((int)pxCurrentTCB->pxTaskTag == 1){	\
																					GPIO_write(PORT_0, PIN9, PIN_IS_LOW);\
																					GPIO_write(PORT_0, PIN2, PIN_IS_HIGH); \
																					xButton_1TimeIn = T1TC;\
																				}																				\
																				else if((int)pxCurrentTCB->pxTaskTag == 2){ \
																					GPIO_write(PORT_0, PIN9, PIN_IS_LOW);\
																					GPIO_write(PORT_0, PIN3, PIN_IS_HIGH);\
																					xButton_2TimeIn = T1TC;\
																				}	\
																				else if((int)pxCurrentTCB->pxTaskTag == 3){ \
																					GPIO_write(PORT_0, PIN9, PIN_IS_LOW);\
																					GPIO_write(PORT_0, PIN4, PIN_IS_HIGH);\
																					xPeriodic_TransTimeIn = T1TC;\
																				}	\
																				else if((int)pxCurrentTCB->pxTaskTag == 4){ \
																					GPIO_write(PORT_0, PIN9, PIN_IS_LOW);\
																					GPIO_write(PORT_0, PIN5, PIN_IS_HIGH);\
																					xUart_ReceiverTimeIn = T1TC;\
																				}	\
																				else if((int)pxCurrentTCB->pxTaskTag == 5){ \
																					GPIO_write(PORT_0, PIN9, PIN_IS_LOW);\
																					GPIO_write(PORT_0, PIN6, PIN_IS_HIGH);\
																					xLoad_1_SimTimeIn = T1TC;\
																				}	\
																				else if((int)pxCurrentTCB->pxTaskTag == 6){ \
																					GPIO_write(PORT_0, PIN9, PIN_IS_LOW);\
																					GPIO_write(PORT_0, PIN7, PIN_IS_HIGH);\
																					xLoad_2_SimTimeIn = T1TC;\
																				}	\
																			}while(0)

/* Task Application Task Tag */
#define configUSE_APPLICATION_TASK_TAG		1


/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1

#define vPortSVCHandler     SVC_Handler
#define xPortPendSVHandler  PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#endif /* FREERTOS_CONFIG_H */
