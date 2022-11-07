/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"
#include "semphr.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )

/* Read the Buttons States */
#define BUTTON_1_STATE		GPIO_read(PORT_0, PIN0)
#define BUTTON_2_STATE		GPIO_read(PORT_0, PIN1)


/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

TaskHandle_t xButton_1Handle = NULL;
TaskHandle_t xButton_2Handle = NULL;
TaskHandle_t xPeriodic_TransmitterHandle = NULL;
TaskHandle_t xUart_ReceiverTaskHandle = NULL;
TaskHandle_t xLoad_1_SimulationHandle = NULL;
TaskHandle_t xLoad_2_SimulationHandle = NULL;

QueueHandle_t xUARTQueue = NULL;

const TickType_t xButton_1Period = 50;
const TickType_t xButton_2Period = 50;
const TickType_t xPeriodic_TransmitterPeriod = 100;
const TickType_t xUart_ReceiverTaskPeriod = 20;
const TickType_t xLoad_1_SimulationPeriod = 10;
const TickType_t xLoad_2_SimulationPeriod = 100;

uint32_t xButton_1TimeIn;
uint32_t xButton_1TimeOut;
uint32_t xButton_1TotalTime;

uint32_t xButton_2TimeIn;
uint32_t xButton_2TimeOut;
uint32_t xButton_2TotalTime;

uint32_t xPeriodic_TransTimeIn;
uint32_t xPeriodic_TransTimeOut;
uint32_t xPeriodic_TransTotalTime;

uint32_t xUart_ReceiverTimeIn;
uint32_t xUart_ReceiverTimeOut;
uint32_t xUart_ReceiverTotalTime;

uint32_t xLoad_1_SimTimeIn;
uint32_t xLoad_1_SimTimeOut;
uint32_t xLoad_1_SimTotalTime;

uint32_t xLoad_2_SimTimeIn;
uint32_t xLoad_2_SimTimeOut;
uint32_t xLoad_2_SimTotalTime;

uint32_t SystemTime;
uint32_t CPULoad;
SemaphoreHandle_t userinput;

/* Strings to transmit */
char Button_1_RIS[]= "BUTTON_1_RISING";
char Button_1_FAl[]= "BUTTON_1_FALLING";

char Button_2_RIS[]= "BUTTON_2_RISING";
char Button_2_FAl[]= "BUTTON_2_FALLING";

char Periodic_STR[]= "PERIODIC_TASK";

char UARTBuffer[20];
uint8_t TransmitterBufferLength;

char RunTimeStatsBuffer[200];
/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */



void Button_1_MonitorTask(void * ptr);

void Button_2_MonitorTask(void * ptr);

void Periodic_TransmitterTask(void * ptr);

void Uart_ReceiverTask(void * ptr);

void Load_1_SimulationTask(void * ptr);

void Load_2_SimulationTask(void * ptr);

void vApplicationTickHook(void){
	
	GPIO_write(PORT_0, PIN8, PIN_IS_HIGH);
	GPIO_write(PORT_0, PIN8, PIN_IS_LOW);
}

void vApplicationIdleHook(void){
	GPIO_write(PORT_0, PIN9, PIN_IS_HIGH);
}

int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
	
	
    /* Create the tasks */
	
    xTaskPeriodicCreate(
                    Button_1_MonitorTask,       /* Function that implements the task. */
                    "Button_1",          /* Text name for the task. */
                    120,      /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    1,						/* Priority at which the task is created. */
                    &xButton_1Handle,	/* Used to pass out the created task's handle. */
										xButton_1Period);  /* Periodicity of task */    
									
	xTaskPeriodicCreate(
                    Button_2_MonitorTask,       /* Function that implements the task. */
                    "Button_2",          /* Text name for the task. */
                    120,      /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    2,						/* Priority at which the task is created. */
                    &xButton_2Handle,	/* Used to pass out the created task's handle. */
										xButton_2Period);  /* Periodicity of task */ 
										
	xTaskPeriodicCreate(
                    Periodic_TransmitterTask,       /* Function that implements the task. */
                    "Periodic_Transmitter",          /* Text name for the task. */
                    200,      /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    3,						/* Priority at which the task is created. */
                    &xPeriodic_TransmitterHandle,	/* Used to pass out the created task's handle. */
										xPeriodic_TransmitterPeriod);  /* Periodicity of task */ 
										
	xTaskPeriodicCreate(
                    Uart_ReceiverTask,       /* Function that implements the task. */
                    "Uart_Receiver",          /* Text name for the task. */
                    200,      /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    4,						/* Priority at which the task is created. */
                    &xUart_ReceiverTaskHandle,	/* Used to pass out the created task's handle. */
										xUart_ReceiverTaskPeriod);  /* Periodicity of task */ 
										
	xTaskPeriodicCreate(
                    Load_1_SimulationTask,       /* Function that implements the task. */
                    "Load_1_Simulation",          /* Text name for the task. */
                    200,      /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    5,						/* Priority at which the task is created. */
                    &xLoad_1_SimulationHandle,	/* Used to pass out the created task's handle. */
										xLoad_1_SimulationPeriod);  /* Periodicity of task */ 
										
	xTaskPeriodicCreate(
                    Load_2_SimulationTask,       /* Function that implements the task. */
                    "Load_2_Simulation",          /* Text name for the task. */
                    200,      /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    6,						/* Priority at which the task is created. */
                    &xLoad_2_SimulationHandle,	/* Used to pass out the created task's handle. */
										xLoad_2_SimulationPeriod);  /* Periodicity of task */ 


	/* Create Queue to receives messages for UART */
	 xUARTQueue = xQueueCreate(
                         /* The number of items the queue can hold. */
                         3,
                         /* Size of each item is big enough to hold the
                         whole structure. */
                         sizeof(UARTBuffer) );
										
										
	/* Now all the tasks have been started - start the scheduler */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/

/* Button_1_Monitor Code */ 
void Button_1_MonitorTask(void * ptr){

	const TickType_t xDelay = xButton_1Period;
	TickType_t xLastWakeTime = xTaskGetTickCount();	
	vTaskSetApplicationTaskTag( NULL, ( void * ) 1 );
	for(;;){
				if((BUTTON_1_STATE) == PIN_IS_HIGH)
				{
					TransmitterBufferLength = strlen(Button_1_RIS);
					xQueueSend(xUARTQueue,( void * ) &Button_1_RIS,( TickType_t ) 0 );
				}
				else if((BUTTON_1_STATE) == PIN_IS_LOW)
				{
					TransmitterBufferLength = strlen(Button_1_FAl);
					xQueueSend(xUARTQueue,( void * ) &Button_1_FAl,( TickType_t ) 0 );
				}
			vTaskDelayUntil( &xLastWakeTime,xDelay);
	}
}

/* Button_2_Monitor Code */ 
void Button_2_MonitorTask(void * ptr){

	const TickType_t xDelay = xButton_2Period;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	vTaskSetApplicationTaskTag( NULL, ( void * ) 2 );	
	for(;;){
					if((BUTTON_2_STATE) == PIN_IS_HIGH)
					{
						TransmitterBufferLength = strlen(Button_2_RIS);
						xQueueSend(xUARTQueue,( void * ) &Button_2_RIS,( TickType_t ) 0 );
					}
					else if((BUTTON_2_STATE) == PIN_IS_LOW)
					{
						TransmitterBufferLength = strlen(Button_2_FAl);
						xQueueSend(xUARTQueue,( void * ) &Button_2_FAl,( TickType_t ) 0 );
					}
					
			vTaskDelayUntil( &xLastWakeTime,xDelay);
	}
}

/* Send preiodic string every 100ms to the consumer task */ 
void Periodic_TransmitterTask(void * ptr){

	const TickType_t xDelay = xPeriodic_TransmitterPeriod;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	vTaskSetApplicationTaskTag( NULL, ( void * ) 3 );
	for(;;){

				TransmitterBufferLength = strlen(Periodic_STR);
				xQueueSend(xUARTQueue,( void * ) &Periodic_STR,( TickType_t ) 0 );

				vTaskDelayUntil( &xLastWakeTime,xDelay);
		}
}

/* write on UART any received string from other tasks */
void Uart_ReceiverTask(void * ptr){

	const TickType_t xDelay = xUart_ReceiverTaskPeriod;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	vTaskSetApplicationTaskTag( NULL, ( void * ) 4 );
	for(;;){
				xQueueReceive(xUARTQueue, ( void * )UARTBuffer, (TickType_t) 0);
				vSerialPutString((const signed char * const)UARTBuffer, (uint16_t)TransmitterBufferLength);
				xSerialPutChar('\n');
				vTaskDelayUntil( &xLastWakeTime,xDelay);
		}
}


void Load_1_SimulationTask(void * ptr){
	
	const TickType_t xDelay = xLoad_1_SimulationPeriod;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	vTaskSetApplicationTaskTag( NULL, ( void * ) 5 );
	for(;;){
				uint32_t Counts;
				for(Counts=0; Counts < 37200; Counts++)
				{
				}	
				vTaskDelayUntil( &xLastWakeTime,xDelay);
		}
}

void Load_2_SimulationTask(void * ptr){

	const TickType_t xDelay = xLoad_2_SimulationPeriod;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	vTaskSetApplicationTaskTag( NULL, ( void * ) 6 );
	
	for(;;){
				uint32_t Counts;
				for(Counts=0; Counts < 88500; Counts++)
				{
				}

				vTaskDelayUntil( &xLastWakeTime,xDelay);
		}
}
