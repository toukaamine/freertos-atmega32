/*
 * main.c
 *
 *  Created on: 2010-04-08
 *      Author: mk219533
 */

#include <stdlib.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"

#define pollqSTACK_SIZE		( ( unsigned short ) configMINIMAL_STACK_SIZE )
#define mainQUEUE_POLL_PRIORITY			( tskIDLE_PRIORITY + 2 )

#define partstALL_BITS_OUTPUT			( ( unsigned char ) 0xff )
#define partstALL_OUTPUTS_OFF			( ( unsigned char ) 0xff )
#define partstMAX_OUTPUT_LED			( ( unsigned char ) 7 )

#define CONSUMER1_ERROR_LED			( ( unsigned char ) 1 )
#define CONSUMER1_OK_LED			( ( unsigned char ) 0 )
#define CONSUMER2_ERROR_LED			( ( unsigned char ) 3 )
#define CONSUMER2_OK_LED			( ( unsigned char ) 2 )
#define PRODUCER1_ERROR_LED			( ( unsigned char ) 5 )
#define PRODUCER1_OK_LED			( ( unsigned char ) 4 )
#define PRODUCER2_ERROR_LED			( ( unsigned char ) 7 )
#define PRODUCER2_OK_LED			( ( unsigned char ) 6 )

#define LED_DELAY 1000 / portTICK_RATE_MS

static volatile unsigned char ucCurrentOutputValue = partstALL_OUTPUTS_OFF; /*lint !e956 File scope parameters okay here. */

/*-----------------------------------------------------------*/

void vParTestInitialise(void)
{
    ucCurrentOutputValue = partstALL_OUTPUTS_OFF;

    /* Set port B direction to outputs.  Start with all output off. */
    DDRB = partstALL_BITS_OUTPUT;
    PORTB = ucCurrentOutputValue;
}
/*-----------------------------------------------------------*/

void vParTestSetLED(unsigned portBASE_TYPE uxLED, signed portBASE_TYPE xValue)
{
    unsigned char ucBit = (unsigned char) 1;

    if (uxLED <= partstMAX_OUTPUT_LED) {
        ucBit <<= uxLED;

        vTaskSuspendAll();
        {
            if (xValue == pdTRUE) {
                ucBit ^= (unsigned char) 0xff;
                ucCurrentOutputValue &= ucBit;
            } else {
                ucCurrentOutputValue |= ucBit;
            }

            PORTB = ucCurrentOutputValue;
        }
        xTaskResumeAll();
    }
}
/*-----------------------------------------------------------*/

void vParTestToggleLED(unsigned portBASE_TYPE uxLED)
{
    unsigned char ucBit;
    if (uxLED <= partstMAX_OUTPUT_LED) {
        ucBit = ((unsigned char) 1) << uxLED;

        vTaskSuspendAll();
        {
            if (ucCurrentOutputValue & ucBit) {
                ucCurrentOutputValue &= ~ucBit;
            } else {
                ucCurrentOutputValue |= ucBit;
            }

            PORTB = ucCurrentOutputValue;
        }
        xTaskResumeAll();
    }
}

void vLED(unsigned portBASE_TYPE uxLED, unsigned portBASE_TYPE count)
{
    portBASE_TYPE i;
    for (i = 0; i < count; ++i) {
        vParTestSetLED(uxLED, pdTRUE);
        vTaskDelay((portTickType) LED_DELAY / 4);
        vParTestSetLED(uxLED, pdFALSE);
        vTaskDelay((portTickType) LED_DELAY / 4);
    }
    vTaskDelay((portTickType) LED_DELAY);
}

static void vPolledQueueProducer1(void *pvParameters)
{
    unsigned short usValue = 1;
    xQueueHandle *pxQueue;
    //const portTickType xDelay = (portTickType) 100 / portTICK_RATE_MS;
    /* The queue being used is passed in as the parameter. */
    pxQueue = (xQueueHandle *) pvParameters;

    for (;;) {
        /* Send an incrementing number on the queue without blocking. */
        if (xQueueSendToBack( *pxQueue, ( void * ) &usValue, ( portTickType ) 0 ) != pdPASS) {
            vLED(PRODUCER1_ERROR_LED, 1);
        } else {
            /* Update the value we are going to post next time around. */
            vLED(PRODUCER1_OK_LED, 1);
        }
        //vTaskDelay(xDelay);
    }
}

static void vPolledQueueProducer2(void *pvParameters)
{
    unsigned short usValue = 2;
    xQueueHandle *pxQueue;
    //const portTickType xDelay = (portTickType) 300 / portTICK_RATE_MS;
    /* The queue being used is passed in as the parameter. */
    pxQueue = (xQueueHandle *) pvParameters;

    for (;;) {
        /* Send an incrementing number on the queue without blocking. */
        if (xQueueSendToBack( *pxQueue, ( void * ) &usValue, ( portTickType ) 0 ) != pdPASS) {
            vLED(PRODUCER2_ERROR_LED, 1);
        } else {
            /* Update the value we are going to post next time around. */
            vLED(PRODUCER2_OK_LED, 2);
        }
        //vTaskDelay(xDelay);
    }
}
/*-----------------------------------------------------------*/

static void vPolledQueueConsumer1(void *pvParameters)
{
    unsigned short usData;
    xQueueHandle *pxQueue;
    const portTickType xDelay = (portTickType) 3000 / portTICK_RATE_MS;

    /* The queue being used is passed in as the parameter. */
    pxQueue = (xQueueHandle *) pvParameters;

    for (;;) {
        /* Loop until the queue is empty. */
        while (uxQueueMessagesWaiting(*pxQueue)) {
            if (xQueueReceive( *pxQueue, &usData, ( portTickType ) 0 ) == pdPASS) {
                //if (usData != usExpectedValue) {
                //	usExpectedValue = usData;
                //	vLED(CONSUMER1_ERROR_LED);
                //} else {
                //	vLED(CONSUMER1_OK_LED);
                //}
                //++usExpectedValue;
                vLED(CONSUMER1_OK_LED, usData);
            } else {
                vLED(CONSUMER1_ERROR_LED, 1);
            }
            vTaskDelay(xDelay);
        }
        vTaskDelay(xDelay);
    }
}

static void vPolledQueueConsumer2(void *pvParameters)
{
    unsigned short usData;
    xQueueHandle *pxQueue;
    const portTickType xDelay = (portTickType) 6000 / portTICK_RATE_MS;

    /* The queue being used is passed in as the parameter. */
    pxQueue = (xQueueHandle *) pvParameters;

    for (;;) {
        /* Loop until the queue is empty. */
        while (uxQueueMessagesWaiting(*pxQueue)) {
            if (xQueueReceive( *pxQueue, &usData, ( portTickType ) 0 ) == pdPASS) {
                //if (usData != usExpectedValue) {
                //	usExpectedValue = usData;
                //	vLED(CONSUMER_ERROR_LED);
                //} else {
                //	vLED(CONSUMER_OK_LED);
                //}
                //++usExpectedValue;
                vLED(CONSUMER2_OK_LED, usData);
            } else {
                vLED(CONSUMER2_ERROR_LED, 1);
            }
            vTaskDelay(xDelay);
        }
        vTaskDelay(xDelay);
    }
}

void vStartPolledQueueTasks(unsigned portBASE_TYPE uxPriority)
{
    static xQueueHandle xPolledQueue;
    const unsigned portBASE_TYPE uxQueueSize = 10;

    /* Create the queue used by the producer and consumer. */
    xPolledQueue = xQueueCreate(uxQueueSize, (unsigned portBASE_TYPE ) sizeof(unsigned short));

    /* Spawn the producer and consumer. */
    xTaskCreate( vPolledQueueConsumer1, "QConsN1", pollqSTACK_SIZE, ( void * ) &xPolledQueue, uxPriority, NULL );
    xTaskCreate( vPolledQueueProducer1, "QProdN1", pollqSTACK_SIZE, ( void * ) &xPolledQueue, uxPriority, NULL );
    xTaskCreate( vPolledQueueConsumer2, "QConsN2", pollqSTACK_SIZE, ( void * ) &xPolledQueue, uxPriority, NULL );
    xTaskCreate( vPolledQueueProducer2, "QProdN2", pollqSTACK_SIZE, ( void * ) &xPolledQueue, uxPriority, NULL );

}
/*-----------------------------------------------------------*/

short main(void)
{
    //prvIncrementResetCount();

    /* Setup the LED's for output. */
    vParTestInitialise();

    /* Create the standard demo tasks. */
    //vStartIntegerMathTasks( tskIDLE_PRIORITY );
    //vAltStartComTestTasks( mainCOM_TEST_PRIORITY, mainCOM_TEST_BAUD_RATE, mainCOM_TEST_LED );
    vStartPolledQueueTasks(mainQUEUE_POLL_PRIORITY);
    //vStartRegTestTasks();

    /* Create the tasks defined within this file. */
    //xTaskCreate( vErrorChecks, ( signed char * ) "Check", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY, NULL );

    /* Create the co-routines that flash the LED's. */
    //vStartFlashCoRoutines( mainNUM_FLASH_COROUTINES );

    /* In this port, to use preemptive scheduler define configUSE_PREEMPTION
     as 1 in portmacro.h.  To use the cooperative scheduler define
     configUSE_PREEMPTION as 0. */
    vTaskStartScheduler();

    return 0;
}

void vApplicationIdleHook(void)
{
    vCoRoutineSchedule();
}
