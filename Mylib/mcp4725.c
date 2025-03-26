#include "mcp4725.h"
 I2C_HandleTypeDef hi2cl; // change your handler here accordingly
#define ADDRESS_DEVICE 0xC0
// registerMode

// page 22
#define MCP4725_GC_RESET 0x06
#define MCP4725_GC_WAKEUP 0x09

// PAGE 18 DATASHEET
int writeFastMode (uint16_t value)
		{
		uint8_t data_t[2];
		data_t[1] = value & 0xFF;
		//data_t[0] = ((value / 256) & 0x0F); // (&0x0F là set CO = Cl = 0, no PDmode) 8 BIT
		data_t[0] = value >>8 & 0x0F; //co the thay the o tren

		//GUI CAO TRUOC VÀ BIT THAP SAU TUONG UNG DATAO ROI DEN DATA1
		return HAL_I2C_Master_Transmit (&hi2cl, ADDRESS_DEVICE, (uint8_t*) data_t, 2, 100);
		}
int setValue (const uint16_t value)
		{
		if (value > MCP4725_MAXVALUE) return MCP4725_VALUE_ERROR;
		int rv = writeFastMode (value) ;
			return rv;
		}
int writeRegisterMode (const uint16_t value, uint8_t reg)
		{
		uint8_t data_t2[3];
		data_t2[0]= reg;
		data_t2[1] = value >>4;
		data_t2[2] = (value << 4) &0xF0;
		//GUI CAO TRUOC VÀ BIT THAP SAU TUONG UNG DATAI ROI DEN DATA2
		return HAL_I2C_Master_Transmit (&hi2cl, 0x60<<1, (uint8_t*) data_t2, 3, 100);
		}
 int writeDAC (const uint16_t value, uint8_t EEPROM)
		 {
			 if (value > MCP4725_MAXVALUE) return MCP4725_VALUE_ERROR;
			 int rv = writeRegisterMode(value, EEPROM);
			 return rv;
		 }
uint8_t readCommand(uint8_t * data)
			{
			return HAL_I2C_Master_Receive (&hi2cl, ADDRESS_DEVICE&0xFl, (uint8_t *) data, 5, 100) ;
			// vi CO LA GHI, C1 LA RECEIVE NEN ADDR= ADDR &1 =CO &1 =C1
			}
//uint16_t readEEPROM(uint16_t *data)

//uint8_t data_n[5];
//HAL_I2C_Master_Receive(&hi2cl, ADDRESS_DEVICE&0xFl, (uint8_t *)data_n, 5, 100);
//data= (uint16_t) data_n[3] << 8;
//data= data | (uint16_t) (data_n[4]) ;
//return *data;

uint16_t readEEPROM(uint16_t *data0)
			{
			uint8_t data_n[5];
			HAL_I2C_Master_Receive(&hi2cl, ADDRESS_DEVICE, data_n, 5, 100);
			*data0 = (uint16_t) data_n[3] << 8 | data_n[4];
			return *data0;
			}

uint16_t readDAC(uint16_t *data)
			{
				uint8_t data_n[5];
				HAL_I2C_Master_Receive(&hi2cl, ADDRESS_DEVICE & 0xF1, (uint8_t *)data_n,5,100);
				*data = (uint16_t )data_n[1]<<4 | (uint16_t)(data_n[2]>>4);
				return *data;
}
