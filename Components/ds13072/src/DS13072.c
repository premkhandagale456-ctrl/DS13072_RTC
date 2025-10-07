/* Includes ---------------------------------------------------------------------*/
#include <string.h>
#include "DS13072.h"


/* Private Constants ------------------------------------------------------------*/
/**
 * @brief  The DS13072 Address on I2C BUS
 */ 
#define DS13072_ADDRESS  0x68

/**
 * @brief  Internal Registers Address
 */ 
#define DS13072_SECOND   0x00  // the address of SECOND register in DS13072
#define DS13072_MINUTE   0x01  // the address of MINUTE register in DS13072
#define DS13072_HOUR     0x02  // the address of HOUR register in DS13072
#define DS13072_DAY      0x03  // the address of DAY register in DS13072
#define DS13072_DATE     0x04  // the address of DATE register in DS13072
#define DS13072_MONTH    0x05  // the address of MONTH register in DS13072
#define DS13072_YEAR     0x06  // the address of YEAR register in DS13072
#define DS13072_CONTROL  0x07  // the address of CONTROL register in DS13072


/**
 * @brief  Non-volatile RAM Address
 */ 
#define DS13072_RAM      0x08  // the address of first byte of Non-volatile RAM
#define DS13072_RAM_SIZE 56    // size of Non-volatile

/**
 * @brief  CONTROL register bits
 */ 
#define DS13072_OUT      7
#define DS13072_SQWE     4
#define DS13072_RS0      0
#define DS13072_RS1      1


/* Private Macro ----------------------------------------------------------------*/
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif


/**
 ==================================================================================
                           ##### Private Functions #####                           
 ==================================================================================
 */

static uint8_t
DS13072_DECtoBCD(uint8_t DEC)
{
  uint8_t Buff2 = DEC % 10;
  uint8_t Buff1 = (DEC / 10) % 10;
  uint8_t BCD   = (Buff1 << 4) | Buff2;

  return BCD;
}

static uint8_t
DS13072_BCDtoDEC(uint8_t BCD)
{
  uint8_t Buff1 = BCD >> 4;
  uint8_t Buff2 = BCD & 0x0f;
  uint8_t DEC   = (Buff1 * 10) + Buff2;

  return DEC;
}

static int8_t
DS13072_WriteRegs(DS13072_Handler_t *Handler,
                 uint8_t StartReg, uint8_t *Data, uint8_t BytesCount)
{
  uint8_t Buffer[DS13072_SEND_BUFFER_SIZE];
  uint8_t Len = 0;

  Buffer[0] = StartReg; // send register address to set RTC pointer
  while (BytesCount)
  {
    Len = MIN(BytesCount, sizeof(Buffer)-1);
    memcpy((void*)(Buffer+1), (const void*)Data, Len);

    if (Handler->PlatformSend(DS13072_ADDRESS, Buffer, Len+1) < 0)
      return -1;

    Data += Len;
    Buffer[0] += Len;
    BytesCount -= Len;
  }

  return 0;
}

static int8_t
DS13072_ReadRegs(DS13072_Handler_t *Handler,
                uint8_t StartReg, uint8_t *Data, uint8_t BytesCount)
{
  if (Handler->PlatformSend(DS13072_ADDRESS, &StartReg, 1) < 0)
    return -1;

  if (Handler->PlatformReceive(DS13072_ADDRESS, Data, BytesCount) < 0)
    return -1;

  return 0;
}



/**
 ==================================================================================
                       ##### Public Common Functions #####                         
 ==================================================================================
 */

/**
 * @brief  Initialize DS13072
 * @param  Handler: Pointer to handler
 * @retval DS13072_Result_t
 *         - DS13072_OK: Operation was successful.
 *         - DS13072_FAIL: Failed to send or receive data.
 *         - DS13072_INVALID_PARAM: One of parameters is invalid.
 */
DS13072_Result_t
DS13072_Init(DS13072_Handler_t *Handler)
{
  if (!Handler->PlatformSend ||
      !Handler->PlatformReceive)
    return DS13072_INVALID_PARAM;

  if (Handler->PlatformInit)
    if (Handler->PlatformInit() < 0)
      return DS13072_FAIL;

  return DS13072_OK;
}

/**
 * @brief  Uninitialize DS13072 
 * @param  Handler: Pointer to handler
 * @retval DS13072_Result_t
 *         - DS13072_OK: Operation was successful.
 *         - DS13072_FAIL: Failed to send or receive data.
 */
DS13072_Result_t
DS13072_DeInit(DS13072_Handler_t *Handler)
{
  if (Handler->PlatformDeInit)
    if (Handler->PlatformDeInit() < 0)
      return DS13072_FAIL;

  return DS13072_OK;
}



/**
 ==================================================================================
                         ##### Public RTC Functions #####                          
 ==================================================================================
 */

/**
 * @brief  Set date and time on DS13072 real time chip
 * @param  Handler: Pointer to handler
 * @param  DateTime: pointer to date and time value structure
 * @retval DS130722_Result_t
 *         - DS130722_OK: Operation was successful.
 *         - DS130722_FAIL: Failed to send or receive data.
 *         - DS130722_INVALID_PARAM: One of parameters is invalid.
 */
DS13072_Result_t
DS13072_SetDateTime(DS13072_Handler_t *Handler, DS13072_DateTime_t *DateTime)
{
  uint8_t Buffer[7] = {0};

  if (DateTime->Second > 59 ||
      DateTime->Minute > 59 ||
      DateTime->Hour > 23 ||
      DateTime->WeekDay > 7 || DateTime->WeekDay == 0 ||
      DateTime->Day > 31 || DateTime->Day == 0 ||
      DateTime->Month > 12 || DateTime->Month == 0 ||
      DateTime->Year > 99)
    return DS13072_INVALID_PARAM;

        // Convert to BCD
  Buffer[0] = DS13072_DECtoBCD(DateTime->Second) & 0x7F;
  Buffer[1] = DS13072_DECtoBCD(DateTime->Minute);
  Buffer[2] = DS13072_DECtoBCD(DateTime->Hour);

  if(DateTime->HourMode == 1) {      // 12-hour mode
        Buffer[2] |= (1 << 7);          // Set 12-hour mode
      if(DateTime->isPM == true){
        Buffer[2] |= (1<<6);  // set PM
      }else{
        Buffer[2] &= (1<<6); // set AM
      }
  }else{
      Buffer[2] &= ~(1 << 7);
  }
  Buffer[3] = DS13072_DECtoBCD(DateTime->WeekDay);
  Buffer[4] = DS13072_DECtoBCD(DateTime->Day);
  Buffer[5] = DS13072_DECtoBCD(DateTime->Month);
  Buffer[6] = DS13072_DECtoBCD(DateTime->Year);

  if (DS13072_WriteRegs(Handler, DS13072_SECOND, Buffer, 7) < 0)
    return DS13072_FAIL;
  
  return DS13072_OK;
}


/**
 * @brief  Get date and time from DS13072 real time chip
 * @param  Handler: Pointer to handler
 * @param  DateTime: pointer to date and time value structure
 * @retval DS13072_Result_t
 *         - DS13072_OK: Operation was successful.
 *         - DS13072_FAIL: Failed to send or receive data.
 */
DS13072_Result_t
DS13072_GetDateTime(DS13072_Handler_t *Handler, DS13072_DateTime_t *DateTime)
{
  uint8_t Buffer[7] = {0};

  if (DS13072_ReadRegs(Handler, DS13072_SECOND, Buffer, 7) < 0)
    return DS13072_FAIL;

  // convert BCD value to decimal
  DateTime->Second  = DS13072_BCDtoDEC(Buffer[0] & 0x7F);
  DateTime->Minute  = DS13072_BCDtoDEC(Buffer[1]);
  DateTime->WeekDay = DS13072_BCDtoDEC(Buffer[3]);
  DateTime->Day     = DS13072_BCDtoDEC(Buffer[4]);
  DateTime->Month   = DS13072_BCDtoDEC(Buffer[5]);
  DateTime->Year    = DS13072_BCDtoDEC(Buffer[6]);
    uint8_t hourReg = Buffer[2];
    // 12-hour mode
    if(hourReg & (1 << 7)) {
        DateTime->isPM = (hourReg & (1<<6)) ? 1: 0;
        DateTime->Hour = DS13072_BCDtoDEC(hourReg & 0x1F);  // bits 0-4
    } else { // 24-hour mode
        DateTime->Hour = DS13072_BCDtoDEC(hourReg & 0x3F);  // bits 0-5
    }
    return DS13072_OK;
}


/**
 ==================================================================================
                       ##### Public Memory Functions #####                         
 ==================================================================================
 */

/**
 * @brief  Write data on DS13072 data Non-volatile RAM
 * @param  Handler: Pointer to handler
 * @param  Address: address of block beginning (0 to 55)
 * @param  Data: pointer to data array
 * @param  Size: data size (1 to 56)
 * @retval DS13072_Result_t
 *         - DS13072_OK: Operation was successful.
 *         - DS13072_FAIL: Failed to send or receive data.
 *         - DS13072_INVALID_PARAM: Requested area is out of range.
 */
DS13072_Result_t
DS13072_WriteRAM(DS13072_Handler_t *Handler,
                uint8_t Address, uint8_t *Data, uint8_t Size)
{
  Address += 8;

  if ((Address + Size) > 0x3f)
    return DS13072_INVALID_PARAM;

  if (DS13072_WriteRegs(Handler, Address, Data, Size) < 0)
    return DS13072_FAIL;

  return DS13072_OK;
}


/**
 * @brief  Read data from DS13072 data Non-volatile RAM
 * @param  Handler: Pointer to handler
 * @param  Address: address of block beginning (0 to 55)
 * @param  Data: pointer to data array
 * @param  Size: data size (1 to 56)
 * @retval DS13072_Result_t
 *         - DS13072_OK: Operation was successful.
 *         - DS13072_FAIL: Failed to send or receive data.
 *         - DS13072_INVALID_PARAM: Requested area is out of range.
 */
DS13072_Result_t
DS13072_ReadRAM(DS13072_Handler_t *Handler,
               uint8_t Address, uint8_t *Data, uint8_t Size)
{
  Address += 8;

  if ((Address + Size) > 0x3f)
    return DS13072_INVALID_PARAM;

  if (DS13072_ReadRegs(Handler, Address, Data, Size) < 0)
    return DS13072_FAIL;

  return DS13072_OK;
}



/**
 ==================================================================================
                     ##### Public Out Wave Functions #####                         
 ==================================================================================
 */

/**
 * @brief  Set output Wave on SQW/Out pin of DS13072
 * @param  Handler: Pointer to handler
 * @param  OutWave: where OutWave Shows different output wave states
 *         - DS13072_OutWave_Low:    Logic level 0 on the SQW/OUT pin
 *         - DS13072_OutWave_High:   Logic level 1 on the SQW/OUT pin
 *         - DS13072_OutWave_1Hz:    Output wave frequency = 1Hz
 *         - DS13072_OutWave_4KHz:   Output wave frequency = 4.096KHz
 *         - DS13072_OutWave_8KHz:   Output wave frequency = 8.192KHz
 *         - DS13072_OutWave_32KHz:  Output wave frequency = 32.768KHz
 * 
 * @retval DS13072_Result_t
 *         - DS13072_OK: Operation was successful.
 *         - DS13072_FAIL: Failed to send or receive data.
 *         - DS13072_INVALID_PARAM: One of parameters is invalid.
 */
DS13072_Result_t
DS13072_SetOutWave(DS13072_Handler_t *Handler, DS13072_OutWave_t OutWave)
{
  uint8_t ControlReg;

  switch (OutWave)
  {
  case DS13072_OutWave_Low:
    ControlReg = 0;
    break;

  case DS13072_OutWave_High:
    ControlReg = (1 << DS13072_OUT);
    break;

  case DS13072_OutWave_1Hz:
    ControlReg = (1 << DS13072_SQWE);
    break;

  case DS13072_OutWave_4KHz:
    ControlReg = (1 << DS13072_SQWE) | (1 << DS13072_RS0);
    break;

  case DS13072_OutWave_8KHz:
    ControlReg = (1 << DS13072_SQWE) | (1 << DS13072_RS1);
    break;

  case DS13072_OutWave_32KHz:
    ControlReg = (1 << DS13072_SQWE) | (3 << DS13072_RS0);
    break;

  default:
    return DS13072_INVALID_PARAM;
  }

  if (DS13072_WriteRegs(Handler, DS13072_CONTROL, &ControlReg, 1) < 0)
    return DS13072_FAIL;

  return DS13072_OK;
}