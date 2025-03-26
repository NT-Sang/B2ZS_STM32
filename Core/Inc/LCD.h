#ifndef __LCD16X2_H__
#define __LCD16X2_H__

#include "stm32f1xx.h"

// dinh nghia cac chan
#define LCD_PORT    GPIOA

#define LCD_RS      GPIO_PIN_0
#define LCD_RW      GPIO_PIN_1
#define LCD_E       GPIO_PIN_2

#define LCD_D4      GPIO_PIN_3
#define LCD_D5      GPIO_PIN_4
#define LCD_D6      GPIO_PIN_5
#define LCD_D7      GPIO_PIN_6

#define cmd_reg     0
#define data_reg    1

// cac ham
void LCD_Write(uint8_t data);
void LCD_Send(uint8_t Reg, uint8_t data);
void LCD_Init();
void LCD_Clear();
extern void LCD_Location(uint8_t x, uint8_t y);
void LCD_Write_String(char* string);
extern void LCD_Write_Number(int number);


#endif