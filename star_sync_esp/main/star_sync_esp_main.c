// /* UART Echo Example

//    This example code is in the Public Domain (or CC0 licensed, at your option.)

//    Unless required by applicable law or agreed to in writing, this
//    software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
//    CONDITIONS OF ANY KIND, either express or implied.
// */
// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/uart.h"
// #include "driver/gpio.h"
// #include "sdkconfig.h"
// #include "esp_log.h"

// /**
//  * This is an example which echos any data it receives on configured UART back to the sender,
//  * with hardware flow control turned off. It does not use UART driver event queue.
//  *
//  * - Port: configured UART
//  * - Receive (Rx) buffer: on
//  * - Transmit (Tx) buffer: off
//  * - Flow control: off
//  * - Event queue: off
//  * - Pin assignment: see defines below (See Kconfig)
//  */

// #define ECHO_TEST_TXD (CONFIG_EXAMPLE_UART_TXD)
// #define ECHO_TEST_RXD (CONFIG_EXAMPLE_UART_RXD)
// #define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
// #define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

// #define ECHO_UART_PORT_NUM      (CONFIG_EXAMPLE_UART_PORT_NUM)
// #define ECHO_UART_BAUD_RATE     (CONFIG_EXAMPLE_UART_BAUD_RATE)
// #define ECHO_TASK_STACK_SIZE    (CONFIG_EXAMPLE_TASK_STACK_SIZE)

// static const char *TAG = "UART TEST";

// #define BUF_SIZE (1024)

// static void echo_task(void *arg)
// {
//     /* Configure parameters of an UART driver,
//      * communication pins and install the driver */
//     uart_config_t uart_config = {
//         .baud_rate = ECHO_UART_BAUD_RATE,
//         .data_bits = UART_DATA_8_BITS,
//         .parity    = UART_PARITY_DISABLE,
//         .stop_bits = UART_STOP_BITS_1,
//         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
//         .source_clk = UART_SCLK_APB,
//     };
//     int intr_alloc_flags = 0;

// #if CONFIG_UART_ISR_IN_IRAM
//     intr_alloc_flags = ESP_INTR_FLAG_IRAM;
// #endif

//     ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
//     ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
//     ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));

//     // Configure a temporary buffer for the incoming data
//     uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

//     while (1) {
//         // Read data from the UART
//         int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, (BUF_SIZE - 1), 20 / portTICK_RATE_MS);
//         // Write data back to the UART
//         uart_write_bytes(ECHO_UART_PORT_NUM, (const char *) data, len);
//         if (len) {
//             data[len] = '\0';
//             ESP_LOGI(TAG, "Recv str: %s", (char *) data);
//         }
//     }
// }

// void app_main(void)
// {
//     xTaskCreate(echo_task, "uart_echo_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
// }

/**
 * ************************************************************
 * @file   : star_sync_esp_main.c
 * @author : suffiyan,
 * @brief  : Driving EC200U through AT Commands
 * @date   : Nov, 2024
 * ************************************************************
 *
 *
 * ************************************************************
 *
 */
#include "star_sync_esp_main.h"
/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static const char *MAIN_TAG = "MAIN_TASK";

device_details_t device_info;
device_system_stats_t system_info;
/* Private function prototype -----------------------------------------------*/

/* main application code -----------------------------------------------*/

void app_main(void)
{

    device_init();
}

/* Private function implementation -----------------------------------------------*/

void device_init()
{

    ESP_LOGI(MAIN_TAG, "APPLICATION_CODE_RUNNING");

    snprintf(device_info.application_version, sizeof(device_info.application_version), "%d.%d.%d", APP_MAJOR_VER, APP_MINOR_VER, APP_PATCH_VER);
    ESP_LOGI(MAIN_TAG, "APP_CODE Ver:%s", device_info.application_version);

    // get device mac id
    uint8_t Mac[6];

    esp_err_t ret = esp_efuse_mac_get_default(Mac);

    if (ret == ESP_OK)
    {
        snprintf(device_info.device_mac_id, sizeof(device_info.device_mac_id), "%02X:%02X:%02X:%02X:%02X:%02X", Mac[0], Mac[1], Mac[2], Mac[3], Mac[4], Mac[5]);
        ESP_LOGI(MAIN_TAG, "Device Mac-ID : %s\n", device_info.device_mac_id);
    }

    ESP_LOGI(MAIN_TAG, "Intializing Peripherals..");

    gpio_set_direction(EC200U_POWERKEY_GSM, GPIO_MODE_OUTPUT);
    gpio_set_direction(EC200U_BUCK_EN, GPIO_MODE_OUTPUT);
    gpio_set_level(EC200U_BUCK_EN, 0);
    vTaskDelay(pdMS_TO_TICKS(500)); 

    gpio_set_level(EC200U_BUCK_EN, 1); // Turn ON Buck converter
    gpio_set_level(EC200U_POWERKEY_GSM, 1);

    ec200_uart_init();
    power_up_ec200u();
    check_at_connection();
    reset_ec200u();
}

void ec200_uart_init()
{

    //-----Init GSM UART---------//
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    uart_param_config(EC200U_UART, &uart_config);
    uart_set_pin(EC200U_UART, EC200U_TX_PIN, EC200U_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    system_info.ec200_uart_state = uart_driver_install(EC200U_UART, EC200_RX_BUF_SIZE, 0, 0, NULL, 0);
    ESP_LOGI(MAIN_TAG, "EC200 UART STAT: %s", esp_err_to_name(system_info.ec200_uart_state));
}

void power_up_ec200u()
{

    gpio_set_level(EC200U_POWERKEY_GSM, 0); // 3.3v
    vTaskDelay(pdMS_TO_TICKS(2000));        // 2s pulse
    gpio_set_level(EC200U_POWERKEY_GSM, 1); // 0v
    vTaskDelay(pdMS_TO_TICKS(2000));
    gpio_set_level(EC200U_POWERKEY_GSM, 0); // 3.3v

    ESP_LOGI(MAIN_TAG, "Powring up EC200U...");
    vTaskDelay(pdMS_TO_TICKS(10000)); 
}
