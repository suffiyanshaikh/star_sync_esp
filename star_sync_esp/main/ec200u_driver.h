/**
 * ************************************************************
 * @file   : ec200u_driver.h
 * @author : suffiyan,
 * @brief  : ec200u communication code
 * @date   : nov, 2024
 * ************************************************************
 *
 *
 * ************************************************************
 *
 */

#ifndef EC200U_DRIVER_H
#define EC200U_DRIVER_H

/* Private includes ----------------------------------------------------------*/
#include "star_sync_esp_main.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define EC200U_RX_BUF_SIZE 1024

/* Private variables ---------------------------------------------------------*/
// Define structure for AT command and timeout
typedef struct
{
    const char *command; // The AT command string
    uint32_t timeout_ms; // Timeout in milliseconds for the command
    const char *response;
} at_command_t;

/* Private function prototype -----------------------------------------------*/

/**
 *  @brief check at connection
 */
void check_at_connection();

/**
 *  @brief reset ec200u
 */
void reset_ec200u();

/**
 *  @brief configure ec200u error reporting
 */
void enable_ec200u_error_output();

/**
 *  @brief set ec200u to bydefault settings
 */
void config_ec200u();

/**
 *  @brief activate ec200u network
 */
void activate_ec200u_network();

/**
 *  @brief turn on gnss engine in ec200u
 */
void turn_on_gnss();

/**
 *  @brief read gps data
 */
void read_gps_cordinates();

/**
 *
 * @brief Function to send an AT command and wait for response
 */
esp_err_t send_at_command(const char *command, const char *expected_response,int at_resp_timeout);

#endif