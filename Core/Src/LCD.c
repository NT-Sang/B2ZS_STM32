#include "LCD.h"
#include "string.h"
#include "stdio.h"


void LCD_Write(uint8_t data)
{
	HAL_GPIO_WritePin(LCD_PORT, LCD_D4, ((data >> 0) & 0x01));
	HAL_GPIO_WritePin(LCD_PORT, LCD_D5, ((data >> 1) & 0x01));
	HAL_GPIO_WritePin(LCD_PORT, LCD_D6, ((data >> 2) & 0x01));
	HAL_GPIO_WritePin(LCD_PORT, LCD_D7, ((data >> 3) & 0x01));

// kich hoat chan enable
	HAL_GPIO_WritePin(LCD_PORT,LCD_E, 1);
	HAL_Delay(1);
	HAL_GPIO_WritePin(LCD_PORT,LCD_E, 0);
}
void LCD_Send(uint8_t Reg, uint8_t data)
{
	HAL_GPIO_WritePin(LCD_PORT, LCD_RS, Reg);   // Reg = 0 : thanh ghi data ; Reg = 1 : thanh ghi lenh
	LCD_Write(data >> 4);
	LCD_Write(data);
}
void LCD_Init()
{
	HAL_GPIO_WritePin(LCD_PORT, LCD_RW, 0);    // chon che do ghi

	LCD_Send(cmd_reg, 0x33);                   // lenh de khoi tao
	LCD_Send(cmd_reg, 0x32);                  // lenh de khoi tao
	LCD_Send(cmd_reg, 0x28);                   // che do 4 bit, 2 hang, 5x7
	LCD_Send(cmd_reg, 0x0C);                   // hien thi man hinh va tat con tro
	LCD_Send(cmd_reg, 0x06);                  // tang con tro
	LCD_Send(cmd_reg, 0x01);                   // xoa toan man hinh
}
void LCD_Clear()                                //xoa toan man hinh
{
	LCD_Send(cmd_reg, 0x01);
}
void LCD_Location(uint8_t x, uint8_t y)         // chon vi tri
{
  if(x == 0)
	  LCD_Send(cmd_reg, 0x80 + y);
  else if(x == 1)
	  LCD_Send(cmd_reg, 0xC0 + y);
}
void LCD_Write_String(char* string)              // ghi chuoi ki tu
{
	for(uint8_t i = 0; i < strlen(string); i++)
	{
		LCD_Send(data_reg, string[i]);
	}
}
void LCD_Write_Number(int number)                 // ghi chu so
{
	char buffer[8];
	sprintf(buffer, "%d", number);
	LCD_Write_String(buffer);
}