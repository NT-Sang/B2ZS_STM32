#ifndef __MCP4725_H
#define __MCP4725_H

#include "stm32f1xx_hal.h"

#define MCP4725_MAXVALUE 4095
// ERRORS
#define MCP4725_OK 0
#define MCP4725_VALUE_ERROR -999
#define MCP4725 REG_ERROR -998
#define MCP4725_NOT_CONNECTED -997

#define MCP4725_PDMODE_NORMAL 0x00
#define MCP4725_PDMODE_1K 0x01
#define MCP4725_PDMODE_100K 0x02
#define MCP4725_PDMODE_5O0K 0x03
#define MCP4725_DAC 0x40
#define MCP4725_DACEEPROM 0x60

#define MCP4725_MIDPOINT 2048

//fastmode
int setValue (const uint16_t value);
int writeFastMode (const uint16_t value);
uint8_t readCommand (uint8_t * data);
//nozmal & eeprom
int writeRegistexMode (const uint16_t value, uint8_t reg);
int writeDAC (const uint16_t value, uint8_t EEPROM) ;
uint16_t readEEPROM(uint16_t *data0) ;
uint16_t readDAC(uint16_t *data) ;

#endif
