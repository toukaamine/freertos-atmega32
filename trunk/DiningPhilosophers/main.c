/*
 * main.c
 *
 *  Created on: 2010-04-10
 *      Author: Marcel
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
#include "semphr.h"

#include "led.h"
#include "uart.h"

#define PHILOSOPHER_PRIORITY                 ( tskIDLE_PRIORITY + 1 )
#define PHILOSOPHER_COUNT                 5

volatile xSemaphoreHandle pxWaiter;
volatile xSemaphoreHandle pxFork[5];

struct task_parameters
{
  unsigned short usNumber;
  portTickType xEatDelay;
  portTickType xThinkDelay;
};
/*-----------------------------------------------------------*/
static void vPhilosopherTask(void *pvParameters)
{
  struct task_parameters *params = (struct task_parameters*) pvParameters;
  unsigned short uxLeftFork = params->usNumber;
  unsigned short uxRightFork = params->usNumber + 1;

  if (uxRightFork >= PHILOSOPHER_COUNT)
    uxRightFork = 0;

  while (1) {
    uart_putchar('A' + params->usNumber, &uart_str);
    // waiter allows only PHILOSOPHER_COUNT - 1 philosophers to begin eating
    xSemaphoreTake(pxWaiter, portMAX_DELAY);

    uart_putchar('a' + params->usNumber, &uart_str);
    // philosophers take forks in same order: left one first
    xSemaphoreTake(pxFork[uxLeftFork], portMAX_DELAY);

    uart_putchar('k' + params->usNumber, &uart_str);
    xSemaphoreTake(pxFork[uxRightFork], portMAX_DELAY);

    // while eating we turn on the LED for params->xEatDelay ms
    vSetLED(params->usNumber, pdTRUE);

    uart_putchar('0' + params->usNumber, &uart_str);

    vTaskDelay(params->xEatDelay / portTICK_RATE_MS);

    xSemaphoreGive(pxFork[uxRightFork]);
    xSemaphoreGive(pxFork[uxLeftFork]);
    xSemaphoreGive(pxWaiter);

    // philosopher is thinking, we turn off the LED
    vSetLED(params->usNumber, pdFALSE);

    //taskYIELD();
    //vTaskDelay(params->xThinkDelay / portTICK_RATE_MS);
  }
}

/*-----------------------------------------------------------*/
static signed char * taskNames[5] = {
    (signed char *) "Philos0",
    (signed char *) "Philos1",
    (signed char *) "Philos2",
    (signed char *) "Philos3",
    (signed char *) "Philos4" };
/*-----------------------------------------------------------*/
struct task_parameters params[5] = {
    { 0, 2000, 0 },
    { 1, 2000, 0 },
    { 2, 2000, 0 },
    { 3, 2000, 0 },
    { 4, 2000, 0 } };
/*-----------------------------------------------------------*/
int main(void)
{
  unsigned short i;
  vLEDInitialise();

  uart_init();

  printf("start\n");

  /* Create the waiter (semaphore) and forks (mutexes) used by philosophers. */
  pxWaiter = xSemaphoreCreateCounting(PHILOSOPHER_COUNT - 1, PHILOSOPHER_COUNT - 1);
  for (i = 0; i < PHILOSOPHER_COUNT; ++i)
    pxFork[i] = xSemaphoreCreateMutex();

  /* Spawn philosophers. */
  for (i = 0; i < PHILOSOPHER_COUNT; ++i)
    xTaskCreate( vPhilosopherTask, taskNames[i], configMINIMAL_STACK_SIZE, ( void * ) &params[i], PHILOSOPHER_PRIORITY, NULL );

  vTaskStartScheduler();

  return 0;
}


