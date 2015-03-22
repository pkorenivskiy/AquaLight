//***************************************************************************
//
//  Author(s)...: Pashgan    http://ChipEnable.Ru   
//
//  Target(s)...: AVR
//
//  Compiler....: �����
//
//  Description.: ������� 7-�� ����������� ���������� 
//
//  Data........: 17.12.13  ������  
//
//***************************************************************************
#ifndef INDICATOR_H
#define INDICATOR_H

#include "compilers_4.h"
#include "port_macros.h"

/********************** ��������� ***************************/

/*���������� ���� ����������*/
#define IND_AMOUNT_NUM    8

/*����� ������������ ���������*/
#define IND_SCAN_SEGMENT  1

/*������������ �� ������ ��������*/
#define IND_USE_BLINK     1

/*������ �������� ����������*/
#define IND_DUR_BLINK    2000

/*���� ���������� �������� ����������*/
#define IND_SEG_PORT   IND_SEG, E, _VIRT

#define IND_SEG_0      B, 0, _LOW
#define IND_SEG_1      B, 1, _LOW
#define IND_SEG_2      B, 2, _LOW
#define IND_SEG_3      B, 3, _LOW
#define IND_SEG_4      A, 4, _LOW
#define IND_SEG_5      A, 5, _LOW
#define IND_SEG_6      A, 6, _LOW
#define IND_SEG_7      A, 7, _LOW

/*���� ���������� ������� ����������*/
#define IND_DIG_PORT   IND_DIG, F, _VIRT

#define IND_DIG_0      A, 0, _HI
#define IND_DIG_1      A, 1, _HI
#define IND_DIG_2      A, 2, _HI
#define IND_DIG_3      A, 3, _HI
#define IND_DIG_4      B, 4, _HI
#define IND_DIG_5      B, 5, _HI
#define IND_DIG_6      B, 6, _HI
#define IND_DIG_7      B, 7, _HI


/******************** ���������������� ������� **********************************/

void IND_Init(void);
void IND_OutSym(char *str, uint8_t pos);
void IND_OutUint(uint16_t value, uint8_t comma);
void IND_OutInt(int16_t value, uint8_t comma);
void IND_OutUintFormat(uint16_t value, uint8_t comma, uint8_t firstPos, uint8_t lastPos);
void IND_OutIntFormat(int16_t value, uint8_t comma, uint8_t firstPos, uint8_t lastPos);
void IND_BlinkMask(uint8_t value);
void IND_Update(void);

#endif //INDICATOR_H