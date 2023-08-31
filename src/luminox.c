/* ****************************************************************************/
/** SST Sensing LuminOx O2 Sensor Function Library

  @File Name
    luminox.c

  @Summary
    Driver Library for SST Sensing LuminOx O2 Sensor

  @Description
    Implements functions that allow the user to interact with LuminOx
******************************************************************************/


#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "luminox.h"

extern volatile bool luminox_complete_uart_rx;

/*
    @brief Function for setting the output mode of the luminox sensor

    @note Response will be “M xx\r\n” Where xx equals the Argument of the command

    @param[in] mode Desired mode for the sensor (streaming, polling, off)

    @param[in] luminox_handler Pointer of library handler

    @return luminox_retcode_t Either success or one of the error codes
*/
luminox_retcode_t luminox_set_ouput_mode(luminox_mode_t mode, luminox_handler_t * luminox_handler) {
    unsigned char mode_msg[] = "M x\r\n";
    // set command to given mode
    switch(mode) {
	case LUMINOX_MODE_STREAMING:
	    mode_msg[2] = '0';
	    break;
	case LUMINOX_MODE_POLLING:
	    mode_msg[2] = '1';
	    break;
	case LUMINOX_MODE_OFF:
	    mode_msg[2] = '2';
	    break;
	default:
#ifdef DEBUG_OUTPUT
	    NRF_LOG_INFO("Invalid Mode");
#endif
	    return LUMINOX_ERR_INVALID_MODE;
    }
    

    luminox_handler->luminox_tx(mode_msg, 5); // transmit the message

    //luminox_wait_for_response(luminox_handler);
    luminox_process_response(luminox_handler);

    return luminox_handler->err_code;
}

/*
    @brief Function for retrieving the current output mode of the luminox sensor

    @param[in] luminox_handler Pointer of library handler

    @return luminox_mode_t returns the current_mode (streaming, polling, or off)
*/
luminox_mode_t luminox_get_output_mode(luminox_handler_t * luminox_handler) {
    return luminox_handler->current_mode;
}

/*
    @brief Function for requesting the current ppO2 value from the sensor

    @note Response will be “O xxxx.x\r\n” Where xxxx.x equals the ppO2 in mbar

    @param[in] luminox_handler Pointer of library handler

    @return luminox_retcode_t Either success or one of the error codes
*/
luminox_retcode_t luminox_request_ppO2(luminox_handler_t * luminox_handler) {
    unsigned char req[] = "O\r\n";
    luminox_handler->luminox_tx(req, 3); // transmit the request

     //luminox_wait_for_response(luminox_handler);
     luminox_process_response(luminox_handler);

    return luminox_handler->err_code;
}

/*
    @brief Function for getting the current ppO2 value

    @note This function returns whatever is stored in the current_ppO2 variable, which may be out of date. Call luminox_request_ppO2() first.

    @param[in] luminox_handler Pointer of library handler

    @return Floating point integer of the ppO2 value
*/
float luminox_get_ppO2(luminox_handler_t * luminox_handler) {
    return luminox_handler->current_ppO2;
}

/*
    @brief Function for requesting the current O2 value from the sensor

    @note Response will be “% xxx.xx\r\n" Where xxx.xx equals the O2 in percent %

    @param[in] luminox_handler Pointer of library handler

    @return luminox_retcode_t Either success or one of the error codes
*/
luminox_retcode_t luminox_request_O2(luminox_handler_t * luminox_handler) {
    unsigned char req[] = "%\r\n";
    luminox_handler->luminox_tx(req, 3); // transmit the request

     //luminox_wait_for_response(luminox_handler);
     luminox_process_response(luminox_handler);

    return luminox_handler->err_code;
}

/*
    @brief Function for getting the current O2 value

    @note This function returns whatever is stored in the current_O2 variable, which may be out of date. Call luminox_request_O2() first.

    @param[in] luminox_handler Pointer of library handler

    @return Floating point integer of the O2 value
*/
float luminox_get_O2(luminox_handler_t * luminox_handler) {
    return luminox_handler->current_O2;
}

/*
    @brief Function for requesting the current Temperature inside the sensor

    @note Response will be “T yxx.x\r\n” Where y equals the sign “-” or “+” and xx.x equals the temperature in °C

    @param[in] luminox_handler Pointer of library handler

    @return luminox_retcode_t Either success or one of the error codes
*/
luminox_retcode_t luminox_request_temp(luminox_handler_t * luminox_handler) {
    unsigned char req[] = "T\r\n";
    luminox_handler->luminox_tx(req, 3); // transmit the request

     //luminox_wait_for_response(luminox_handler);
     luminox_process_response(luminox_handler);

    return luminox_handler->err_code;
}

/*
    @brief Function for getting the current temp value

    @note This function returns whatever is stored in the current_temp variable, which may be out of date. Call luminox_request_temp() first.

    @param[in] luminox_handler Pointer of library handler

    @return Floating point integer of the temp value
*/
float luminox_get_temp(luminox_handler_t * luminox_handler) {
    return luminox_handler->current_temp;
}

/*
    @brief Function for requesting the current barometric pressure

    @note Response will be “P xxxx\r\n” Where xxxx equals the pressure in mbar

    @note Only valid for sensors fitted with barometric pressure sensor. Otherwise returns "------"

    @param[in] luminox_handler Pointer of library handler

    @return luminox_retcode_t Either success or one of the error codes
*/
luminox_retcode_t luminox_request_barometric_pressure(luminox_handler_t * luminox_handler) {
    unsigned char req[] = "P\r\n";
    luminox_handler->luminox_tx(req, 3); // transmit the request

     //luminox_wait_for_response(luminox_handler);
     luminox_process_response(luminox_handler);

    return luminox_handler->err_code;
}

/*
    @brief Function for getting the current barometric pressure value

    @note This function returns whatever is stored in the current_barometric_pressure variable, which may be out of date. Call luminox_request_barometric_pressure() first.

    @param[in] luminox_handler Pointer of library handler

    @return Floating point integer of the barometric pressure value
*/
float luminox_get_barometric_pressure(luminox_handler_t * luminox_handler) {
    return luminox_handler->current_barometric_pressure;
}

/*
    @brief Function for requesting the current sensor status

    @note Response will be “e 0000\r\n” = Sensor Status Good 
			   “e xxxx\r\n” = Any other response, contact SST Sensing for advice

    @param[in] luminox_handler Pointer of library handler

    @return luminox_retcode_t Either success or one of the error codes
*/
luminox_retcode_t luminox_request_sensor_status(luminox_handler_t * luminox_handler) {
    unsigned char req[] = "e\r\n";
    luminox_handler->luminox_tx(req, 3); // transmit the request

     //luminox_wait_for_response(luminox_handler);
     luminox_process_response(luminox_handler);

    return luminox_handler->err_code;
}


/*
    @brief Function for requesting ppO2, O2, temperature, barometric pressure, and sensor status

    @note Response will be “O xxxx.x T yxx.x P xxxx % xxx.xx e xxxx\r\n”

    @param[in] luminox_handler Pointer of library handler

    @return luminox_retcode_t Either success or one of the error codes
*/
luminox_retcode_t luminox_request_all(luminox_handler_t * luminox_handler) {
    unsigned char req[] = "A\r\n";
    luminox_handler->luminox_tx(req, 3); // transmit the request

     //luminox_wait_for_response(luminox_handler);
     luminox_process_response(luminox_handler);


    return luminox_handler->err_code;
}

/*
    @brief Function for requesting the given sensor information

    @note Response will be “# YYYYY DDDDD\r\n” - Date of manufacture
			   “# xxxxx xxxxx\r\n” - Serial number
			   “# xxxxx\r\n”       - Software revision

    @param[in] info The desired information to be requested (date of mfg, serial #, sw version)

    @param[in] luminox_handler Pointer of library handler
    
    @return luminox_retcode_t Either success or one of the error codes
*/
luminox_retcode_t luminox_request_sensor_info(luminox_sensor_info_t info, luminox_handler_t * luminox_handler) {
    unsigned char info_req[] = "# x\r\n";
    // set command to given info
    switch(info) {
	case LUMINOX_INFO_DATE_OF_MFG:
#ifdef DEBUG_OUTPUT
	    NRF_LOG_INFO("Date of Manufacturing: ");
	    NRF_LOG_FLUSH();
#endif
	    info_req[2] = '0';
	    break;
	case LUMINOX_INFO_SERIAL_NUM:
#ifdef DEBUG_OUTPUT
	    NRF_LOG_INFO("Serial Number: ");
	    NRF_LOG_FLUSH();
#endif
	    info_req[2] = '1';
	    break;
	case LUMINOX_INFO_SW_VER:
#ifdef DEBUG_OUTPUT
	    NRF_LOG_INFO("Software Revision: ");
	    NRF_LOG_FLUSH();
#endif
	    info_req[2] = '2';
	    break;
	default:
#ifdef DEBUG_OUTPUT
	    NRF_LOG_INFO("Invalid Info");
	    NRF_LOG_FLUSH();
#endif
	    return LUMINOX_ERR_INVALID_INFO;
    }

    luminox_handler->luminox_tx(info_req, 5); // transmit the request

     //luminox_wait_for_response(luminox_handler);
     luminox_process_response(luminox_handler);

    return luminox_handler->err_code;
}

/*
    @brief Function for handling any unsuccessfull requests

    @note Called in luminox_process_response if an error message is received from the sensor, 
	  uses what's in luminox_data[] to process the error

    @param[in] luminox_handler Pointer of library handler

    @return luminox_retcode_t returns one of the defined luminox error codes
*/
luminox_retcode_t luminox_error_handler(luminox_handler_t * luminox_handler) {
    switch(luminox_handler->luminox_data[3]) {
	    case 0x30:
#ifdef DEBUG_OUTPUT
	        NRF_LOG_INFO("Error: USART Receiver Overflow");
		NRF_LOG_FLUSH();
#endif
	        return LUMINOX_ERR_RX_OVERFLOW;
	    case 0x31:
#ifdef DEBUG_OUTPUT
	        NRF_LOG_INFO("Error: Invalid Command");
		NRF_LOG_FLUSH();
#endif
	        return LUMINOX_ERR_INVALID_CMD;
	    case 0x32:
#ifdef DEBUG_OUTPUT
	        NRF_LOG_INFO("Error: Invalid Frame");
		NRF_LOG_FLUSH();
#endif
	        return LUMINOX_ERR_INVALID_FRAME;
	    case 0x33:
#ifdef DEBUG_OUTPUT
	        NRF_LOG_INFO("Error: Invalid Argument");
		NRF_LOG_FLUSH();
#endif
	        return LUMINOX_ERR_INVALID_ARG;
    }
    return LUMINOX_ERROR;
}

/*
    @brief Function for printing the response from the luminox sensor

    @note The LuminOx sensor responds in ASCII encoded messages

    @note Updates the current_x variables and err_code

    @param[in] luminox_handler Pointer of library handler
*/
void luminox_process_response(luminox_handler_t * luminox_handler) {
    uint16_t i = 0;
    while(luminox_handler->luminox_data[i] != '\n') {
        switch(luminox_handler->luminox_data[i]) {
	        case ERROR_RESPONSE:
		        luminox_handler->err_code = luminox_error_handler(luminox_handler);
		        goto EndWhile; // break out of while loop (to stop printing rest of message)
            case MODE_OUTPUT:
                switch(luminox_handler->luminox_data[i+3]) {
                    case 0x30:
                        luminox_handler->current_mode = LUMINOX_MODE_STREAMING; // update current mode
#ifdef DEBUG_OUTPUT
        		        NRF_LOG_INFO("Mode changed to streaming mode.");
				NRF_LOG_FLUSH();
#endif
        		        break;
        	        case 0x31:
			            luminox_handler->current_mode = LUMINOX_MODE_POLLING; // update current mode
#ifdef DEBUG_OUTPUT
        		        NRF_LOG_INFO("Mode changed to polling mode.");
				NRF_LOG_FLUSH();
#endif
        		        break;
        	        case 0x32:
			            luminox_handler->current_mode = LUMINOX_MODE_OFF; // update current mode
#ifdef DEBUG_OUTPUT
        		        NRF_LOG_INFO("Mode changed to off.");
				NRF_LOG_FLUSH();
#endif
        		        break;
        	    }
        	    goto EndWhile; // break out of while loop (to stop printing rest of message)
            case PPO2: {
                uint8_t ppo2_data[6];
                i += 2; // increment by  2 to move the index to the start of the actual ppO2 data
                memcpy(ppo2_data, &luminox_handler->luminox_data[i], sizeof(luminox_handler->luminox_data[i]) * 6); // separate ascii string with data from rest of response
                float ppo2_flt = atof((char*)(ppo2_data)); // turn ascii response into floating point integer that the computer can use
                luminox_handler->current_ppO2 = ppo2_flt; // update value
#ifdef DEBUG_OUTPUT
                NRF_LOG_INFO("ppO2 Value: " NRF_LOG_FLOAT_MARKER " mbar", ppo2_flt);
		NRF_LOG_FLUSH();
#endif
		i += 5; // increment by 5 to move the index to the end of the string or next value in the response
		break;
            }
            case O2: {
		uint8_t o2_data[6];
		i += 2;
		memcpy(o2_data, &luminox_handler->luminox_data[i], sizeof(luminox_handler->luminox_data[i]) * 6);
		float o2_flt = atof((char*)(o2_data));
		luminox_handler->current_O2 = o2_flt;
#ifdef DEBUG_OUTPUT
                NRF_LOG_INFO("o2 Value: " NRF_LOG_FLOAT_MARKER " %", o2_flt);
		NRF_LOG_FLUSH();
#endif
		i += 5;
		break;
            }
            case TEMPERATURE: {
                uint8_t temp_data[5];
                i += 2;
                memcpy(temp_data, &luminox_handler->luminox_data[i], sizeof(luminox_handler->luminox_data[i]) * 5);
                float temp_flt = atof((char*)(temp_data));
                luminox_handler->current_temp = temp_flt;
#ifdef DEBUG_OUTPUT
                NRF_LOG_INFO("Temperature: " NRF_LOG_FLOAT_MARKER " C", temp_flt);
		NRF_LOG_FLUSH();
#endif  
		i += 4;
		break;
            }
            case BAROMETRIC_PRESSURE: {
                uint8_t barometric_data[4];
                i += 2;
                memcpy(barometric_data, &luminox_handler->luminox_data[i], sizeof(luminox_handler->luminox_data[i]) * 4);
                float barometric_flt = atof((char*)(barometric_data));
                luminox_handler->current_barometric_pressure = barometric_flt;
#ifdef DEBUG_OUTPUT
                NRF_LOG_INFO("Barometric Pressure: " NRF_LOG_FLOAT_MARKER " mbar", barometric_flt);
		NRF_LOG_FLUSH();
#endif
                i += 3;
                break;
            }
            case SEPARATOR:
#ifdef DEBUG_OUTPUT
		NRF_LOG_INFO(" ");
		NRF_LOG_FLUSH();
#endif
		break;
            case SENSOR_STATUS:
#ifdef DEBUG_OUTPUT
		NRF_LOG_INFO("Sensor Status:");
		NRF_LOG_FLUSH();
#endif
		break;
	    case SENSOR_INFORMATION:
		break;
            default:
#ifdef DEBUG_OUTPUT
	    NRF_LOG_INFO("%c", luminox_handler->luminox_data[i]);
	    NRF_LOG_FLUSH();
#endif
	    break;
        }

        i++;
    }
    EndWhile: ;
#ifdef DEBUG_OUTPUT
    NRF_LOG_INFO("");
#endif
    luminox_handler->err_code = LUMINOX_SUCCESS;
}

/*
    @brief Function for initializing communication with the LuminOx sensor

    @note First sets the output mode to polling to request and print out sensor information, then sets the output mode
	  to default mode, which is set to be off. Then resets all of the static variables keeping track of state and recent sensor readings.

    @param[in] luminox_handler Pointer of library handler
*/
void luminox_init(luminox_handler_t * luminox_handler) {
#ifdef DEBUG_OUTPUT
    NRF_LOG_INFO("LuminOx Sensor Initialization...");
    NRF_LOG_FLUSH();
#endif

    // set output mode to polling
    luminox_handler->err_code = luminox_set_ouput_mode(LUMINOX_MODE_POLLING, luminox_handler);

#ifdef DEBUG_OUTPUT
    NRF_LOG_INFO("LuminOx Sensor Info:");
    NRF_LOG_FLUSH();
#endif

    luminox_request_sensor_info(LUMINOX_INFO_DATE_OF_MFG, luminox_handler);

    luminox_request_sensor_info(LUMINOX_INFO_SERIAL_NUM, luminox_handler);

    luminox_request_sensor_info(LUMINOX_INFO_SW_VER, luminox_handler);

    // set output mode to default
    luminox_set_ouput_mode(LUMINOX_MODE_DEFAULT, luminox_handler);
    luminox_handler->current_mode = LUMINOX_MODE_DEFAULT;
    
    // set measurements to 0
    luminox_handler->current_ppO2 = 0;
    luminox_handler->current_O2 = 0;
    luminox_handler->current_temp = 0;
    luminox_handler->current_barometric_pressure = 0;
    memset(luminox_handler->luminox_data, 0, UART_RX_BUF_SIZE * sizeof(uint8_t));

    // set error code to success
    luminox_handler->err_code = LUMINOX_SUCCESS;
}

/*
    @brief Function for updating the luminox_data array with the most recent response from the LuminOx sensor

    @param[in] p_luminox_response Pointer to the byte string containing the response from the LuminOx sensor

    @param[in] size Size, in bytes, of the response fromthe LuminOx sensor

    @param[in] luminox_handler Pointer of library handler

    @note call this function in your uart_event_handler when a complete response from the sensor has been recognized
*/
void luminox_update_data(uint8_t * p_response, uint8_t size, luminox_handler_t * luminox_handler) {
    memcpy(luminox_handler->luminox_data, p_response, size);
}

/*
    @brief Function for waiting for response from sensor

    @param[in] luminox_handler Pointer of library handler
*/
luminox_retcode_t luminox_wait_for_response(luminox_handler_t * luminox_handler) {
    uint32_t timer = 0;
    while(!luminox_complete_uart_rx) {
         if(timer >= RESPONSE_TIMEOUT) {
 #ifdef DEBUG_OUTPUT
             NRF_LOG_INFO("UART Response Timeout");
 #endif
             luminox_handler->err_code = LUMINOX_ERR_TIMEOUT; // timeout
             return LUMINOX_ERR_TIMEOUT;
         }
         timer++;
    }
    luminox_complete_uart_rx = false; // reset flag
    return LUMINOX_SUCCESS;
}
