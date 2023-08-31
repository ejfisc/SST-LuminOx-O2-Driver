/* ****************************************************************************/
/** SST Sensing LuminOx O2 Sensor Function Library

  @File Name
    luminox.h

  @Summary
    Driver Library for SST Sensing LuminOx O2 Sensor

  @Description
    Defines functions that allow the user to interact with LuminOx
******************************************************************************/

#ifndef LUMINOX_H
#define LUMINOX_H

#include <stdint.h>

//#define DEBUG_OUTPUT // comment this line out to turn off debug output
#ifdef DEBUG_OUTPUT
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#endif

/* 
    Size of the uart receive and transmit buffer in bytes
    Use these to configure your UART protocol
*/
#define UART_TX_BUF_SIZE 128
#define UART_RX_BUF_SIZE 128

/*
    Set to the largest value that a 32 bit integer can represent, 
    UART is slow but this was a sufficient max wait time in testing.
    If the micro is waiting this long for a response from the sensor it
    is reasonable to assume that something is wrong. 
*/
#define RESPONSE_TIMEOUT 0x989680

// ascii codes for commands/keywords
#define MODE_OUTPUT 0x4D // M
#define PPO2 0x4F // O
#define O2 0x25 // %
#define TEMPERATURE 0x54 // T
#define BAROMETRIC_PRESSURE 0x50 // P
#define ALL 0x41 // A
#define SENSOR_STATUS 0x65 // e
#define SENSOR_INFORMATION 0x23 // #
#define SEPARATOR 0x20 // " "
#define TERMINATOR 0x0A // "\n"
#define ERROR_RESPONSE 0x45 // E

// @brief luminox output modes
typedef enum {
    LUMINOX_MODE_STREAMING = 0, // streaming mode
    LUMINOX_MODE_POLLING, // polling mode
    LUMINOX_MODE_OFF, // sensor off
    LUMINOX_MODE_DEFAULT = LUMINOX_MODE_OFF // default mode is off
} luminox_mode_t;

// @brief luminox sensor information
typedef enum {
    LUMINOX_INFO_DATE_OF_MFG = 0, // date of manufacturing
    LUMINOX_INFO_SERIAL_NUM, // sensor serial number
    LUMINOX_INFO_SW_VER // software revision
} luminox_sensor_info_t;

// @brief luminox return codes
typedef enum {
    /*
	Error: USART Receiver Overflow
	Possible Cause: No <Terminator> received before overflow
	Action: - Check USART Setup
		- Confirm correct termination
    */
    LUMINOX_ERR_RX_OVERFLOW = 0,
    /*
	Error: Invalid Command
	Possible Cause: Unrecognised <Command> received
	Action: - Check command is valid
		- Check command is uppercase ('M' instead of 'm')
    */
    LUMINOX_ERR_INVALID_CMD,
    /*
	Error: Invalid Frame
	Possible Cause: Incorrect character in frame <Separator>
	Action: - Check correct separator is used
    */
    LUMINOX_ERR_INVALID_FRAME,
    /*
	Error: Invalid Argument
	Possible Cause: <Argument> not allowed in limits
	Action: - Check argument is no longer than 6 characters
		- Check argument is within limits
		- Check argument is available for command
    */
    LUMINOX_ERR_INVALID_ARG,
    /*
	Error: Invalid Mode Type
	Cause: luminox_set_output_mode() was given an invalid luminox_mode_t
	Action:	- Check luminox_set_ouput_mode() is given a valid luminox_mode_t
    */
    LUMINOX_ERR_INVALID_MODE, 
    /*
	Error: Invalid Info Type
	Cause: luminox_request_sensor_info() was given an invalid luminox_sensor_info_t
	Action:	- Check luminox_request_sensor_info() is given a valid luminox_sensor_info_t
    */
    LUMINOX_ERR_INVALID_INFO,
    /*
	Error: UART Response Timeout
	Cause: luminox library function waited too long for response from sensor
	Action: - Check that the sensor is in not in off mode
		- Check that the micro is sending and receiving data from the sensor
    */
    LUMINOX_ERR_TIMEOUT,
    LUMINOX_ERROR, // generic error code
    LUMINOX_SUCCESS // message sent or response received successfully
} luminox_retcode_t;

// luminox driver handler struct
typedef struct {
    luminox_mode_t current_mode;
    float current_ppO2;
    float current_O2;
    float current_temp;
    float current_barometric_pressure;
    uint8_t luminox_data[UART_RX_BUF_SIZE];
    luminox_retcode_t err_code;
    void (*luminox_tx)(unsigned char *request, uint8_t size); // must be initialized
} luminox_handler_t;

/*******************************[ High-Level Sensor Functions For General Use ]****************************************/

/*
    @brief Function for setting the output mode of the luminox sensor

    @note Response will be “M xx\r\n” Where xx equals the Argument of the command

    @param[in] mode Desired mode for the sensor (streaming, polling, off)

    @return luminox_retcode_t Either success or one of the error codes
*/
luminox_retcode_t luminox_set_ouput_mode(luminox_mode_t mode, luminox_handler_t * luminox_handler);

/*
    @brief Function for retrieving the current output mode of the luminox sensor

    @return luminox_mode_t returns the current_mode (streaming, polling, or off)
*/
luminox_mode_t luminox_get_output_mode(luminox_handler_t * luminox_handler);

/*
    @brief Function for requesting the current ppO2 value from the sensor

    @note Response will be “O xxxx.x\r\n” Where xxxx.x equals the ppO2 in mbar

    @return luminox_retcode_t Either success or one of the error codes
*/
luminox_retcode_t luminox_request_ppO2(luminox_handler_t * luminox_handler);

/*
    @brief Function for getting the current ppO2 value

    @note This function returns whatever is stored in the current_ppO2 variable, which may be out of date. Call luminox_request_ppO2() first.

    @return Floating point integer of the ppO2 value
*/
float luminox_get_ppO2(luminox_handler_t * luminox_handler);

/*
    @brief Function for requesting the current O2 value from the sensor

    @note Response will be “% xxx.xx\r\n" Where xxx.xx equals the O2 in percent %

    @return luminox_retcode_t Either success or one of the error codes
*/
luminox_retcode_t luminox_request_O2(luminox_handler_t * luminox_handler);

/*
    @brief Function for getting the current O2 value

    @note This function returns whatever is stored in the current_O2 variable, which may be out of date. Call luminox_request_O2() first.

    @return Floating point integer of the O2 value
*/
float luminox_get_O2(luminox_handler_t * luminox_handler);

/*
    @brief Function for requesting the current Temperature inside the sensor

    @note Response will be “T yxx.x\r\n” Where y equals the sign “-” or “+” and xx.x equals the temperature in °C

    @return luminox_retcode_t Either success or one of the error codes
*/
luminox_retcode_t luminox_request_temp(luminox_handler_t * luminox_handler);

/*
    @brief Function for getting the current temp value

    @note This function returns whatever is stored in the current_temp variable, which may be out of date. Call luminox_request_temp() first.

    @return Floating point integer of the temp value
*/
float luminox_get_temp(luminox_handler_t * luminox_handler);

/*
    @brief Function for requesting the current barometric pressure

    @note Response will be “P xxxx\r\n” Where xxxx equals the pressure in mbar

    @note Only valid for sensors fitted with barometric pressure sensor. Otherwise returns "------"

    @return luminox_retcode_t Either success or one of the error codes
*/
luminox_retcode_t luminox_request_barometric_pressure(luminox_handler_t * luminox_handler);

/*
    @brief Function for getting the current barometric pressure value

    @note This function returns whatever is stored in the current_barometric_pressure variable, which may be out of date. Call luminox_request_barometric_pressure() first.

    @return Floating point integer of the barometric pressure value
*/
float luminox_get_barometric_pressure(luminox_handler_t * luminox_handler);

/*
    @brief Function for requesting the current sensor status

    @note Response will be “e 0000\r\n” = Sensor Status Good 
			   “e xxxx\r\n” = Any other response, contact SST Sensing for advice
    
    @return luminox_retcode_t Either success or one of the error codes
*/
luminox_retcode_t luminox_request_sensor_status(luminox_handler_t * luminox_handler);

/*
    @brief Function for requesting ppO2, O2, temperature, barometric pressure, and sensor status

    @note Response will be “O xxxx.x T yxx.x P xxxx % xxx.xx e xxxx\r\n”

    @return luminox_retcode_t Either success or one of the error codes
*/
luminox_retcode_t luminox_request_all(luminox_handler_t * luminox_handler);

/*
    @brief Function for requesting the given sensor information

    @note Response will be “# YYYYY DDDDD\r\n” - Date of manufacture
			   “# xxxxx xxxxx\r\n” - Serial number
			   “# xxxxx\r\n”       - Software revision

    @param[in] info The desired sensor information to request (date of mfg, serial #, sw version)

    @return luminox_retcode_t Either success or one of the error codes
*/
luminox_retcode_t luminox_request_sensor_info(luminox_sensor_info_t info, luminox_handler_t * luminox_handler);

/*
    @brief Function for handling any unsuccessfull requests

    @note Called in luminox_process_response if an error message is received from the sensor, 
	  uses what's in luminox_data[] to process the error

    @return luminox_retcode_t returns one of the defined luminox error codes
*/
luminox_retcode_t luminox_error_handler(luminox_handler_t * luminox_handler);

/*
    @brief Function for printing the response from the luminox sensor

    @note The LuminOx sensor responds in ASCII encoded messages

    @note Updates the current_x variables and err_code
*/
void luminox_process_response(luminox_handler_t * luminox_handler);

/*
    @brief Function for initializing communication with the LuminOx sensor

    @note First sets the output mode to polling to request and print out sensor information, then sets the output mode
	  to default mode, which is set to be off. Then resets all of the static variables keeping track of state and recent sensor readings.
*/
void luminox_init(luminox_handler_t * luminox_handler);

/*
    @brief Function for updating the luminox_data array with the most recent response from the LuminOx sensor

    @param[in] p_luminox_response Pointer to the byte string containing the response from the LuminOx sensor

    @param[in] size Size, in bytes, of the response fromthe LuminOx sensor

    @note call this function in your uart_event_handler when a complete response from the sensor has been recognized
*/
void luminox_update_data(uint8_t * p_luminox_response, uint8_t size, luminox_handler_t * luminox_handler);

/*
    @brief Function for waiting for response from sensor

    @param[in] luminox_handler Pointer of library handler
*/
luminox_retcode_t luminox_wait_for_response(luminox_handler_t * luminox_handler);


#endif // LUMINOX_H