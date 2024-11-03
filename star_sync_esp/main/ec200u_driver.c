/**
 * ************************************************************
 * @file   : ec200u_driver.c
 * @author : suffiyan,
 * @brief  :
 * @date   : nov, 2024
 * ************************************************************
 *
 *
 * ************************************************************
 *
 */

/* Private includes ----------------------------------------------------------*/
#include "ec200u_driver.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
// Define a table of commands with timeouts
at_command_t at_commands[] = {
    {"AT", 2000},                // Basic AT command (1 second timeout)
    {"AT+CFUN=", 2000},          // Set full functionality (2 seconds timeout)
    {"AT+QGPS=1\r\n", 3000},    // Enable GNSS (3 seconds timeout)
    {"AT+QGPSLOC=2\r\n", 5000}, // Get GPS location (5 seconds timeout)
    {"AT+QIACT=1\r\n", 10000},  // Activate PDP context (10 seconds timeout)
    {"AT+CMEE=1\r\n", 1000},    // Enable verbose error reporting (1 second timeout)
    // Add more commands as needed
};

const char *ec200_driver = "ec200_driver";

/* Private function prototype -----------------------------------------------*/

/* Private function implementation -----------------------------------------------*/
void check_at_connection()
{

    char AT_CMD[100];

    snprintf(AT_CMD, sizeof(AT_CMD),"%s\r\n", at_commands[0].command);
    system_info.ec200_uart_state = send_at_command(AT_CMD, "OK", at_commands[0].timeout_ms);

    if (system_info.ec200_uart_state == ESP_OK)
    {
        ESP_LOGI(ec200_driver, "AT connection OK");
    }
    else
    {
        ESP_LOGI(ec200_driver, "AT Connection error ,%s", esp_err_to_name(system_info.ec200_uart_state));
    }
}

void reset_ec200u()
{

    char AT_CMD[100];

    snprintf(AT_CMD, sizeof(AT_CMD),"%s1,1\r\n", at_commands[1].command);
    system_info.ec200_uart_state = send_at_command(AT_CMD, "OK", at_commands[1].timeout_ms);

    if (system_info.ec200_uart_state == ESP_OK)
    {
        ESP_LOGI(ec200_driver, "EC200 RESET OK!");
    }
    else
    {
        ESP_LOGE(ec200_driver, "EC200 RESET FAIL!,%s", esp_err_to_name(system_info.ec200_uart_state));
    }

}


// Function to send an AT command and wait for response
esp_err_t send_at_command(const char *command, const char *expected_response, int at_resp_timeout)
{

    char response[EC200U_RX_BUF_SIZE];
    int len;
    uint8_t retry = 0;
    // int64_t start_time = esp_timer_get_time() / 1000;

    uart_flush(EC200U_UART);

    while(retry < 3)
    {
        retry++;
    // Send AT command
    uart_write_bytes(EC200U_UART, command, strlen(command));
    ESP_LOGI(ec200_driver, "Sent command: %s", command);
    vTaskDelay(pdMS_TO_TICKS(at_resp_timeout));

    // Wait for response
    // while ((esp_timer_get_time() / 1 - start_time) < (at_resp_timeout * 1000))
    { // Convert timeout to microseconds
        len = uart_read_bytes(EC200U_UART, response, EC200U_RX_BUF_SIZE - 1, pdMS_TO_TICKS(20));
        if (len > 0)
        {
            response[len] = '\0'; // Null-terminate the response
            ESP_LOGI(ec200_driver, "Received response: %s", response);

            // Check if expected response is found
            if (strstr(response, expected_response) != NULL)
            {
                return ESP_OK; // Expected response found
            }
        }
    }

    }

    // If timeout occurs
    ESP_LOGE(ec200_driver, "waiting for response");

    return ESP_ERR_TIMEOUT;
}