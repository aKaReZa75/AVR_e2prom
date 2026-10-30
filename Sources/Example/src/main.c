/**
 ******************************************************************************
 * @file     main.c
 * @brief    Internal EEPROM library usage example for AVR
 * 
 * @author   Hossein Bagheri
 * @github   https://github.com/aKaReZa75
 * 
 * @note     This example demonstrates:
 *           - Writing and reading fixed-size types (uint8/16/32, int8/16/32, float)
 *           - Writing and reading blocks (byte array and string)
 *           - Formatting (erasing) the EEPROM
 *           - Printing the read-back values over USART
 * 
 * @note     EEPROM memory map used in this example:
 *           - 0x0000 : uint8_t   (80)
 *           - 0x0010 : uint16_t  (20000)
 *           - 0x0020 : uint32_t  (400000)
 *           - 0x0030 : float     (3.1415)
 *           - 0x0040 : uint8_t[5] block (0x00 - 0x04)
 *           - 0x0050 : string    ("aKaReZa")
 *           - 0x0060 : int8_t    (-50)
 *           - 0x0070 : int16_t   (-20000)
 *           - 0x0080 : int32_t   (-400000)
 * 
 * @note     Expected USART output:
 *           uint8_t: 80
 *           uint16_t: 20000
 *           uint32_t: 400000
 *           float: 3.1415
 *           _Array: 0x00, 0x01, 0x02, 0x03, 0x04
 *           _String: aKaReZa
 *           int8_t: -50
 *           int16_t: -20000
 *           int32_t: -400000
 * 
 * @note     Printing float with sprintf on AVR requires the printf float
 *           support to be enabled in the linker options:
 *           -Wl,-u,vfprintf -lprintf_flt -lm
 * 
 * @note     For detailed documentation with examples, visit:
 *           https://github.com/aKaReZa75/AVR_E2PROM
 ******************************************************************************
 */

#include "aKaReZa.h"


/* ============================================================================
 *                         GLOBAL VARIABLES
 * ============================================================================ */
char debugBuffer[40];                    /**< Text buffer used to format messages before sending over USART */


/* ============================================================================
 *                         MAIN FUNCTION
 * ============================================================================ */
int main(void)
{
  usart_Init(Initialize);                /**< Initialize USART for debug output */
  usart_Putsln("aKaReZa");  

  /* ===== WRITE FIXED-SIZE TYPES ===== */
  uint8_t _uint8 = 80;
  e2prom_uint8_Write(0x0000, _uint8);

  uint16_t _uint16 = 20000;
  e2prom_uint16_Write(0x0010, _uint16);

  uint32_t _uint32 = 400000;
  e2prom_uint32_Write(0x0020, _uint32);

  float _float = 3.1415;
  e2prom_float_Write(0x0030, _float);

  /* ===== WRITE BLOCKS ===== */
  uint8_t _Array[] = {0x00, 0x01, 0x02, 0x03, 0x04};
  e2prom_block_Update(0x0040, _Array, sizeof(_Array));          /**< Update variant: write a byte array */

  char _String[] = "aKaReZa";
  e2prom_block_Write(0x0050, (uint8_t*)_String, sizeof(_String));  /**< Write a string including the terminating '\0' */

  /* ===== WRITE SIGNED TYPES ===== */
  int8_t _int8 = -50;
  e2prom_int8_Write(0x0060, _int8);

  int16_t _int16 = -20000;
  e2prom_int16_Write(0x0070, _int16);  

  int32_t _int32 = -400000;
  e2prom_int32_Write(0x0080, _int32);    


  /* ===== READ BACK FIXED-SIZE TYPES ===== */
  _uint8 = 0;                                                   /**< Clear the variable to prove the value comes from EEPROM */
  e2prom_uint8_Read(0x0000, &_uint8);
  sprintf(debugBuffer, "uint8_t: %d", _uint8);
  usart_Putsln(debugBuffer);

  _uint16 = 0;
  e2prom_uint16_Read(0x0010, &_uint16);
  sprintf(debugBuffer, "uint16_t: %u", _uint16);
  usart_Putsln(debugBuffer);

  _uint32 = 0;
  e2prom_uint32_Read(0x0020, &_uint32);
  sprintf(debugBuffer, "uint32_t: %lu", _uint32);
  usart_Putsln(debugBuffer);
  
  _float = 0;
  e2prom_float_Read(0x0030, &_float);
  sprintf(debugBuffer, "float: %1.4f", _float);
  usart_Putsln(debugBuffer);  

  /* ===== READ BACK BLOCKS ===== */
  memset(_Array, 0xFF, sizeof(_Array));                         /**< Fill with 0xFF to prove the values come from EEPROM */
  e2prom_block_Read(0x0040, _Array, sizeof(_Array));
  sprintf(debugBuffer, "_Array: 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X", _Array[0], _Array[1], _Array[2], _Array[3], _Array[4]);
  usart_Putsln(debugBuffer);

  memset(_String, '\0', sizeof(_String));
  e2prom_block_Read(0x0050, (uint8_t*)_String, sizeof(_String));
  sprintf(debugBuffer, "_String: %s", _String);
  usart_Putsln(debugBuffer);

  /* ===== READ BACK SIGNED TYPES ===== */
  _int8 = 0x00;
  e2prom_int8_Read(0x0060, &_int8);
  sprintf(debugBuffer, "int8_t: %d", _int8);
  usart_Putsln(debugBuffer);  

  _int16 = 0x00;
  e2prom_int16_Read(0x0070, &_int16);
  sprintf(debugBuffer, "int16_t: %d", _int16);
  usart_Putsln(debugBuffer);  

  _int32 = 0x00;
  e2prom_int32_Read(0x0080, &_int32);
  sprintf(debugBuffer, "int32_t: %ld", _int32);
  usart_Putsln(debugBuffer);  

  /* ===== FORMAT EEPROM ===== */
  e2prom_Format();                                              /**< Erase the whole EEPROM */

  while(1)
  {
    
  };
};