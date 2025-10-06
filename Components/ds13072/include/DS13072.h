/* Define to prevent recursive inclusion ----------------------------------------*/
#ifndef _DS13072_H_
#define _DS13072_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ---------------------------------------------------------------------*/
#include <stdint.h>


/* Exported Data Types ----------------------------------------------------------*/

/**
 * @brief  Library functions result data type
 */
typedef enum DS1307_Result_e
{
  DS1307_OK             = 0,
  DS1307_FAIL           = 1,
  DS1307_INVALID_PARAM  = 2,
} DS1307_Result_t;

/**
 * @brief  Function type for Initialize/Deinitialize the platform dependent layer.
 * @retval 
 *         -  0: The operation was successful.
 *         - -1: The operation failed. 
 */
typedef int8_t (*DS1307_PlatformInitDeinit_t)(void);

/**
 * @brief  Function type for Send/Receive data to/from the slave.
 * @param  Address: Address of slave (0 <= Address <= 127)
 * @param  Data: Pointer to data
 * @param  Len: data len in Bytes
 * @retval 
 *         -  0: The operation was successful.
 *         - -1: Failed to send/receive.
 *         - -2: Bus is busy.
 *         - -3: Slave doesn't ACK the transfer.
 */
typedef int8_t (*DS1307_PlatformSendReceive_t)(uint8_t Address,
                                                uint8_t *Data, uint8_t Len);

/**
 * @brief  Handler
 * @note   This handler must be initialize before using library functions
 */
typedef struct DS1307_Handler_s
{
  // Initializes platform dependent layer
  DS1307_PlatformInitDeinit_t PlatformInit;
  // De-initializes platform dependent layer
  DS1307_PlatformInitDeinit_t PlatformDeInit;
  // Send Data to the DS1307
  DS1307_PlatformSendReceive_t PlatformSend;
  // Receive Data from the DS1307
  DS1307_PlatformSendReceive_t PlatformReceive;
} DS1307_Handler_t;

/**
 * @brief  Date and time data type
 */
typedef struct DS1307_DateTime_s
{
  uint8_t   Second;
  uint8_t   Minute;
  uint8_t   Hour;
  uint8_t   WeekDay;
  uint8_t   Day;
  uint8_t   Month;
  uint8_t   Year;
} DS1307_DateTime_t;

/**
 * @brief  squarewave output signal options
 */
typedef enum DS1307_OutWave_e
{
  DS1307_OutWave_Low    = 0,  // Logic level 0 on the SQW/OUT pin
  DS1307_OutWave_High   = 1,  // Logic level 1 on the SQW/OUT pin
  DS1307_OutWave_1Hz    = 2,  // Output wave frequency = 1Hz
  DS1307_OutWave_4KHz   = 3,  // Output wave frequency = 4.096KHz
  DS1307_OutWave_8KHz   = 4,  // Output wave frequency = 8.192KHz
  DS1307_OutWave_32KHz  = 5   // Output wave frequency = 32.768KHz
} DS1307_OutWave_t;


/* Functionality Options --------------------------------------------------------*/
/**
 * @brief  Specify Send buffer size.
 * @note   larger buffer size => better performance
 * @note   The DS1307_SEND_BUFFER_SIZE must be set larger than 1 (9 or more is
 *         suggested)
 */   
#define DS1307_SEND_BUFFER_SIZE   9



/**
 ==================================================================================
                           ##### Common Functions #####                           
 ==================================================================================
 */

/**
 * @brief  Initialize DS1307 
 * @param  Handler: Pointer to handler
 * @retval DS1307_Result_t
 *         - DS1307_OK: Operation was successful.
 *         - DS1307_FAIL: Failed to send or receive data.
 *         - DS1307_INVALID_PARAM: One of parameters is invalid.
 */
DS1307_Result_t
DS1307_Init(DS1307_Handler_t *Handler);


/**
 * @brief  Uninitialize DS1307 
 * @param  Handler: Pointer to handler
 * @retval DS1307_Result_t
 *         - DS1307_OK: Operation was successful.
 *         - DS1307_FAIL: Failed to send or receive data.
 */
DS1307_Result_t
DS1307_DeInit(DS1307_Handler_t *Handler);



/**
 ==================================================================================
                             ##### RTC Functions #####                             
 ==================================================================================
 */

/**
 * @brief  Set date and time on DS1307 real time chip
 * @param  Handler: Pointer to handler
 * @param  DateTime: pointer to date and time value structure
 * @retval DS1307_Result_t
 *         - DS1307_OK: Operation was successful.
 *         - DS1307_FAIL: Failed to send or receive data.
 *         - DS1307_INVALID_PARAM: One of parameters is invalid.
 */
DS1307_Result_t
DS1307_SetDateTime(DS1307_Handler_t *Handler, DS1307_DateTime_t *DateTime);


/**
 * @brief  Get date and time from DS1307 real time chip
 * @param  Handler: Pointer to handler
 * @param  DateTime: pointer to date and time value structure
 * @retval DS1307_Result_t
 *         - DS1307_OK: Operation was successful.
 *         - DS1307_FAIL: Failed to send or receive data.
 */
DS1307_Result_t
DS1307_GetDateTime(DS1307_Handler_t *Handler, DS1307_DateTime_t *DateTime);



/**
 ==================================================================================
                           ##### Memory Functions #####                            
 ==================================================================================
 */

/**
 * @brief  Write data on DS1307 data Non-volatile RAM
 * @param  Handler: Pointer to handler
 * @param  Address: address of block beginning (0 to 55)
 * @param  Data: pointer to data array
 * @param  Size: data size (1 to 56)
 * @retval DS1307_Result_t
 *         - DS1307_OK: Operation was successful.
 *         - DS1307_FAIL: Failed to send or receive data.
 *         - DS1307_INVALID_PARAM: Requested area is out of range.
 */
DS1307_Result_t
DS1307_WriteRAM(DS1307_Handler_t *Handler,
                uint8_t Address, uint8_t *Data, uint8_t Size);


/**
 * @brief  Read data from DS1307 data Non-volatile RAM
 * @param  Handler: Pointer to handler
 * @param  Address: address of block beginning (0 to 55)
 * @param  Data: pointer to data array
 * @param  Size: data size (1 to 56)
 * @retval DS1307_Result_t
 *         - DS1307_OK: Operation was successful.
 *         - DS1307_FAIL: Failed to send or receive data.
 *         - DS1307_INVALID_PARAM: Requested area is out of range.
 */
DS1307_Result_t
DS1307_ReadRAM(DS1307_Handler_t *Handler,
               uint8_t Address, uint8_t *Data, uint8_t Size);



/**
 ==================================================================================
                          ##### Out Wave Functions #####                           
 ==================================================================================
 */

/**
 * @brief  Set output Wave on SQW/Out pin of DS1307
 * @param  Handler: Pointer to handler
 * @param  OutWave: where OutWave Shows different output wave states
 *         - DS1307_OutWave_Low:    Logic level 0 on the SQW/OUT pin
 *         - DS1307_OutWave_High:   Logic level 1 on the SQW/OUT pin
 *         - DS1307_OutWave_1Hz:    Output wave frequency = 1Hz
 *         - DS1307_OutWave_4KHz:   Output wave frequency = 4.096KHz
 *         - DS1307_OutWave_8KHz:   Output wave frequency = 8.192KHz
 *         - DS1307_OutWave_32KHz:  Output wave frequency = 32.768KHz
 * 
 * @retval DS1307_Result_t
 *         - DS1307_OK: Operation was successful.
 *         - DS1307_FAIL: Failed to send or receive data.
 *         - DS1307_INVALID_PARAM: One of parameters is invalid.
 */
DS1307_Result_t
DS1307_SetOutWave(DS1307_Handler_t *Handler, DS1307_OutWave_t OutWave);



#ifdef __cplusplus
}
#endif


#endif //! _DS1307_H_