/**
* ************************************************************
* @file   : star_sync_esp_main.h
* @author : suffiyan, 
* @brief  : 
* @date   : Nov, 2024
* ************************************************************
* 
*
* ************************************************************
* 
*/

#ifndef STAR_SYNC_ESP_MAIN_H
#define STAR_SYNC_ESP_MAIN_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "string.h"

/* Private includes ----------------------------------------------------------*/
#include "version.h"
#include "ec200u_driver.h"

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/

//-------------EC200U---------------//

#define EC200U_UART UART_NUM_1
#define EC200U_RX_PIN GPIO_NUM_35   // main uart reciver pin
#define EC200U_TX_PIN GPIO_NUM_25   // main uart transmitt pin
#define EC200U_BUCK_EN GPIO_NUM_4       // buck enable pin for gsm
#define EC200U_POWERKEY_GSM GPIO_NUM_26 // GSM powerkey pin

#define EC200_RX_BUF_SIZE (1024)


/* Private variables ---------------------------------------------------------*/

typedef struct
{

char application_version[10];
char device_mac_id[30];


}device_details_t;

typedef struct 
{
    
   esp_err_t ec200_uart_state;

}device_system_stats_t;

extern device_details_t device_info;
extern device_system_stats_t system_info;


/* Private function prototype -----------------------------------------------*/


/**
* @brief init all the peripherals and basic configuration
*
*/
void device_init();

/**
* @brief initalize esp32 uart connected with ec200
*
*/
void ec200_uart_init();

/**
* @brief power up ec200u
*
*/
void power_up_ec200u();

#endif