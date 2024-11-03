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
at_command_t basic_at_commands[] = {
    {"AT", 2000, "OK"},       // check AT connection, 1sec timeout
    {"AT+CFUN=", 2000, "OK"}, // set functionality, 1sec timeout
    {"AT+CMEE=", 2000, "OK"}, // Enable verbose error reporting,1sec timeout
    // Add more commands as needed
};

at_command_t network_at_commands[] = {
    {"AT+QSIMDET=1,0\r\n", 2000, "OK"},           // Enable sim detection
    {"AT+QSIMSTAT=1\r\n", 2000, "OK"},            // Enable sim status report
    {"AT+QSIMSTAT?\r\n", 2000, "+QSIMSTAT: 1,1"}, // check sim is inserted or not
    {"AT+CPIN?\r\n", 2000, "+CPIN: READY"},       // check sim is inserted or not
    {"AT+QIDEACT=1\r\n", 2000, "OK"},             // deactivate pdp context
    {"AT+QICSGP=1,1,", 2000, "OK"},               // set apn
    {"AT+QIACT=1\r\n,", 2000, "OK"},              // activate PDP
    {"AT+CEER\r\n", 2000, "OK"},                  // check PDP context
    {"AT+QIACT?\r\n", 2000, "OK"},
    {"AT+CREG?\r\n", 2000, NULL},
    // Add more commands as needed
};

#define NW_AT_COMMAND_COUNT (sizeof(network_at_commands) / sizeof(network_at_commands[0]))

at_command_t gnss_at_commands[] = {
    {"AT+QGPS=", 2000,"OK"},    // turn on GNSS,1sec timeout
    {"AT+QGPSLOC=", 2000,"OK"}, // set functionality, 1sec timeout
    {"AT+QGPSEND", 1000,"OK"},  // turn of GNSS,1sec timeout
    // Add more commands as needed
};

const char *ec200_driver = "ec200_driver";
char apn[20] = "airteliot.com";


/* Private function prototype -----------------------------------------------*/

/* Private function implementation -----------------------------------------------*/
void check_at_connection()
{

    char AT_CMD[100];

    snprintf(AT_CMD, sizeof(AT_CMD), "%s\r\n", basic_at_commands[0].command);
    system_info.ec200_uart_state = send_at_command(AT_CMD, "OK", basic_at_commands[0].timeout_ms);

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

    char AT_CMD[50];

    snprintf(AT_CMD, sizeof(AT_CMD), "%s1,1\r\n", basic_at_commands[1].command);
    system_info.ec200_uart_state = send_at_command(AT_CMD, "OK", basic_at_commands[1].timeout_ms);

    if (system_info.ec200_uart_state == ESP_OK)
    {
        ESP_LOGI(ec200_driver, "EC200 RESET OK!");
    }
    else
    {
        ESP_LOGE(ec200_driver, "EC200 RESET FAIL!,%s", esp_err_to_name(system_info.ec200_uart_state));
    }
}

void enable_ec200u_error_output()
{

    char AT_CMD[100];

    snprintf(AT_CMD, sizeof(AT_CMD), "%s2\r\n", basic_at_commands[2].command);
    system_info.ec200_uart_state = send_at_command(AT_CMD, "OK", basic_at_commands[1].timeout_ms);

    // if (system_info.ec200_uart_state == ESP_OK)
    // {
    //     ESP_LOGI(ec200_driver, "CMEE OK!");
    // }
    // else
    // {
    //     ESP_LOGE(ec200_driver, "EC200 RESET FAIL!,%s", esp_err_to_name(system_info.ec200_uart_state));
    // }
}

void config_ec200u()
{

    reset_ec200u();
    enable_ec200u_error_output();
}

void activate_ec200u_network()
{
    esp_err_t result;

    ESP_LOGI(ec200_driver, "Registering EC200U network!");

    for (int i = 0; i < 5; i++)
    {

        result = send_at_command(network_at_commands[i].command, network_at_commands[i].response, network_at_commands[i].timeout_ms);

        if (result == ESP_OK)
        {
            ESP_LOGI(ec200_driver, "Command %s succeeded", network_at_commands[i].command);
        }
        else
        {
            ESP_LOGE(ec200_driver, "Command %s failed or timed out", network_at_commands[i].command);
        }
    }

    char AT_CMD[30];

    snprintf(AT_CMD, sizeof(AT_CMD), "%s\"%s\"\r\n", network_at_commands[5].command, apn);
    result = send_at_command(AT_CMD, "OK", network_at_commands[5].timeout_ms);

    for (int i = 6; i < 9; i++)
    {

        result = send_at_command(network_at_commands[i].command, network_at_commands[i].response, network_at_commands[i].timeout_ms);

        if (result == ESP_OK)
        {
            ESP_LOGI(ec200_driver, "Command %s succeeded", network_at_commands[i].command);
        }
        else
        {
            ESP_LOGE(ec200_driver, "Command %s failed or timed out", network_at_commands[i].command);
        }
    }

    result = send_at_command(network_at_commands[9].command, network_at_commands[9].response, network_at_commands[9].timeout_ms);


    if (result == ESP_OK)
    {
        ESP_LOGI(ec200_driver, "EC200U Network Activated");
    }
    else
    {
        ESP_LOGE(ec200_driver, "Failed to Activate Network");
    }
}

void turn_on_gnss()
{

    char AT_CMD[100];

    snprintf(AT_CMD, sizeof(AT_CMD), "%s1\r\n", gnss_at_commands[0].command);
    system_info.ec200_uart_state = send_at_command(AT_CMD, "OK", gnss_at_commands[0].timeout_ms);

    if (system_info.ec200_uart_state == ESP_OK)
    {
        ESP_LOGI(ec200_driver, "GNSS ENGINE ENABLED..");
    }
    else
    {
        ESP_LOGE(ec200_driver, "GNSS ENGINE FAIL!,%s", esp_err_to_name(system_info.ec200_uart_state));
    }
}

void read_gps_cordinates()
{

    char AT_CMD[100];

    snprintf(AT_CMD, sizeof(AT_CMD), "%s1\r\n", gnss_at_commands[1].command);
    system_info.ec200_uart_state = send_at_command(AT_CMD, "OK", gnss_at_commands[1].timeout_ms);

    // if (system_info.ec200_uart_state == ESP_OK)
    // {
    //     ESP_LOGI(ec200_driver, "GNSS ENGINE ENABLED..");
    // }
    // else
    // {
    //     ESP_LOGE(ec200_driver, "GNSS ENGINE FAIL!,%s", esp_err_to_name(system_info.ec200_uart_state));
    // }
}

// Function to send an AT command and wait for response
esp_err_t send_at_command(const char *command, const char *expected_response, int at_resp_timeout)
{

    char response[EC200U_RX_BUF_SIZE];
    int len;
    uint8_t retry = 0;

    uart_flush(EC200U_UART);

    while (retry < 3)
    {
        retry++;
        // Send AT command
        uart_write_bytes(EC200U_UART, command, strlen(command));
        ESP_LOGI(ec200_driver, "Sent command: %s", command);
        vTaskDelay(pdMS_TO_TICKS(at_resp_timeout));

        len = uart_read_bytes(EC200U_UART, response, EC200U_RX_BUF_SIZE - 1, pdMS_TO_TICKS(20));
        if (len > 0)
        {
            response[len] = '\0'; // Null-terminate the response
            ESP_LOGI(ec200_driver, "Received response: %s", response);

            if (expected_response != NULL)
            {
                // Check if expected response is found
                if (strstr(response, expected_response) != NULL)
                {
                    return ESP_OK; // Expected response found
                }
            }
            else if (expected_response == NULL)
            {

                if ((strstr(response, "+CREG: 0,1") != NULL )| (strstr(response, "+CREG: 0,5") != NULL))
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