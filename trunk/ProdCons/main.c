/*
 * main.c
 *
 *  Created on: 2010-04-28
 *      Author: Marcel
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "led.h"
#include "uart.h"

#define PRODUCER_PRIORITY       ( tskIDLE_PRIORITY + 1 )
#define CONSUMER_PRIORITY       ( tskIDLE_PRIORITY + 1 )
#define PRODUCER_COUNT          3
#define CONSUMER_COUNT          3
#define CONSUMER_SLEEP_TIME_MS 1000
#define PRODUCER_SLEEP_TIME_MS 600

#define BUFFER_SIZE   10

volatile xQueueHandle pxBuffer;

struct task_parameters
{
  unsigned portBASE_TYPE uxNumber;
  portTickType xDelay;
  signed char sName[8];
};

/*-----------------------------------------------------------*/
struct task_parameters consumerParams[] = {
    { 0, 3000, "Cons0" },
    { 1, 2200, "Cons1" },
    { 2, 1400, "Cons2" }  };

struct task_parameters producerParams[] = {
    { 0, 600, "Prod0" },
    { 1, 600, "Prod1" },
    { 2, 600, "Prod2" } };
/*-----------------------------------------------------------*/
static void vProducerTask(void *pvParameters)
{
  unsigned portBASE_TYPE uxNumber = ((struct task_parameters*)pvParameters)->uxNumber;
  portTickType xDelay = ((struct task_parameters*)pvParameters)->xDelay;

  while (1) {
    if (xQueueSend( pxBuffer, &uxNumber, portMAX_DELAY ) == pdTRUE)
      uart_putchar('A' + uxNumber, &uart_str);
    else
      vSetLED(uxNumber + 4, pdTRUE);

    vTaskDelay(xDelay / portTICK_RATE_MS);
    vSetLED(uxNumber + 4, pdFALSE);
  }
}

/*-----------------------------------------------------------*/
static void vConsumerTask(void *pvParameters)
{
  unsigned portBASE_TYPE uxValue, uxNumber = ((struct task_parameters*)pvParameters)->uxNumber;
  portTickType xDelay = ((struct task_parameters*)pvParameters)->xDelay;

  while (1) {
    if (xQueueReceive( pxBuffer, &uxValue, portMAX_DELAY ) == pdTRUE)
      uart_putchar('0' + 3 * uxNumber + uxValue, &uart_str);
    else
      vSetLED(uxNumber, pdTRUE);

    vTaskDelay(xDelay / portTICK_RATE_MS);
    vSetLED(uxNumber, pdFALSE);
  }}

/*-----------------------------------------------------------*/
int main(void)
{
  unsigned short i;
  vLEDInitialise();

  uart_init();

  printf("start\n");

  /* Create the buffer */
  pxBuffer = xQueueCreate(BUFFER_SIZE, sizeof(unsigned portBASE_TYPE));

  /* Spawn consumers. */
  for (i = 0; i < CONSUMER_COUNT; ++i)
    xTaskCreate( vConsumerTask, consumerParams[i].sName, configMINIMAL_STACK_SIZE, ( void * )( consumerParams + i ), CONSUMER_PRIORITY, NULL );

  /* Spawn producers. */
  for (i = 0; i < PRODUCER_COUNT; ++i)
    xTaskCreate( vProducerTask, producerParams[i].sName, configMINIMAL_STACK_SIZE, ( void * )( producerParams + i ), PRODUCER_PRIORITY, NULL );

  vTaskStartScheduler();

  return 0;
}


