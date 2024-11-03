
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
    config_ec200u();
    activate_ec200u_network();
    turn_on_gnss();

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(5000)); 
        read_gps_cordinates();
    }
    
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
