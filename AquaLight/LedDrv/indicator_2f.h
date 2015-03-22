//***************************************************************************
//
//  Author(s)...: Pashgan    http://ChipEnable.Ru   
//
//  Target(s)...: AVR
//
//  Compiler....: любой
//
//  Description.: Драйвер 7-ми сегментного индикатора 
//
//  Data........: 17.12.13  Создан  
//
//***************************************************************************
#ifndef INDICATOR_H
#define INDICATOR_H

#include "compilers_4.h"
#include "port_macros.h"

/********************** настройки ***************************/

/*количество цифр индикатора*/
#define IND_AMOUNT_NUM    4

/*какая используется развертка*/
/*
0 - 
1 - segment
*/
#define IND_SCAN_SEGMENT  0

/*используется ли фунция моргания*/
#define IND_USE_BLINK     1

/*период моргания индикатора*/
#define IND_DUR_BLINK    1000

/*куда подключены сегменты индикатора*/
#define IND_SEG_PORT   IND_SEG, E, _VIRT

#define IND_SEG_0      B, 4, _LOW
#define IND_SEG_1      B, 5, _LOW
#define IND_SEG_2      C, 0, _LOW
#define IND_SEG_3      C, 1, _LOW
#define IND_SEG_4      C, 2, _LOW
#define IND_SEG_5      C, 3, _LOW
#define IND_SEG_6      C, 4, _LOW
#define IND_SEG_7      C, 5, _LOW

/*куда подключены разряды индикатора*/
#define IND_DIG_PORT   IND_DIG, F, _VIRT

#define IND_DIG_0      D, 1, _LOW
#define IND_DIG_1      D, 2, _LOW
#define IND_DIG_2      D, 3, _LOW
#define IND_DIG_3      D, 4, _LOW
#define IND_DIG_4      B, 4, _NONE
#define IND_DIG_5      B, 5, _NONE
#define IND_DIG_6      B, 6, _NONE
#define IND_DIG_7      B, 7, _NONE


/******************** Пользовательские функции **********************************/

void IND_Init(void);
void IND_OutSym(char *str, uint8_t pos);
void IND_OutUint(uint16_t value, uint8_t comma);
void IND_OutInt(int16_t value, uint8_t comma);
void IND_OutUintFormat(uint16_t value, uint8_t comma, uint8_t firstPos, uint8_t lastPos);
void IND_OutIntFormat(int16_t value, uint8_t comma, uint8_t firstPos, uint8_t lastPos);
void IND_BlinkMask(uint8_t value);
void IND_Update(void);

#endif //INDICATOR_H