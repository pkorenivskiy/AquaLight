/*
 * AqvaTmp.c
 *
 * Created: 05.11.2014 3:22:05
 *  Author: p.korenovskiy
 */ 

//#define F_CPU	32768UL
#define F_CPU	8000000UL

#include <avr/io.h>
#include <avr/iom8.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "avrBitOp.h"

#include "LedDrv/indicator_2f.h"

unsigned char fUpd = 0;

#define BTN1DOWN	(!CHECKBIT(PIND, PD6))
#define BTN2DOWN	(!CHECKBIT(PIND, PD7))

#define MAXPWM		720
#define DLDUR		60 * 60 // 60 min daylight duration
#define SSDUR		84 * 60 // 90 min sunset duration
#define DLUPDPER	DLDUR / MAXPWM
#define SSUPDPER	SSDUR / MAXPWM

volatile unsigned int g_nBtn1 = 0;
volatile unsigned int g_nBtn2 = 0;

volatile unsigned char g_chBtn1State = 0;
volatile unsigned char g_chBtn2State = 0;

volatile struct TIME
{
	char hh;
	char mm;
	char ss;
	char fUpd;
	char fCng;
} g_Time;

// Display mode
// 0: HH:mm
// 1: mm:ss
// 2: set hour
// 3: set min
// 4: PWM value
// 5: set PWM value
char g_prgMode = 0;

unsigned char g_uchLightUpd = 0;

ISR(TIMER2_OVF_vect)
{
	cli();
	
	if (++g_Time.ss == 60)
	{
		g_Time.ss = 0;
		if (++g_Time.mm == 60)
		{
			g_Time.mm = 0;
			if(++g_Time.hh == 24)
			g_Time.hh = 0;
		}
	}
	
	g_Time.fUpd = 1;
	
	sei();
}

ISR(TIMER0_OVF_vect)
{
	cli();
	g_nBtn1++;
	g_nBtn2++;
	sei();
}

void initTimer0()
{
	TCCR0 = (1 << CS02) | (0 << CS01) | (1 << CS00);
	TIMSK |= (1 << TOIE0); //  Разрешить прерывание по переполнению Т0
	//TIFR = (1 << TOV0);
	TCNT0 = 0;
}

void initPwm() // timer1 http://cxem.net/mc/mc231.php
{
	//TCCR1A = (1 << COM1A1) | (0 << COM1A0); //Clear OC1A/OC1B on Compare Match, set OC1A/OC1B at BOTTOM
	TCCR1A = (1 << WGM11) | (1 << WGM10);
	TCCR1B = (0 << CS12) |(1 << CS11) |(1 << CS10) | (0 << WGM13) | (1 << WGM12); //Делитель= /1
	OCR1A = 0;			//Начальная яркость нулевая
}

// real time clock
void initTimer2()
{
	TCCR2 = (1 << CS22) | (0 << CS21) | (1 << CS20); // clkT2S/128 (From prescaler)
	TIMSK = (1 << TOIE2); //  Разрешить прерывание по переполнению Т2
	
	ASSR = (1 << AS2);
}

void initTimers()
{
	cli();
	
	initPwm();
	
	initTimer2();
	initTimer0();
	
	sei();
}

int main(void)
{	
	DDRB = 0xFF; // turn all in OUT
	PORTB = 0x00; // turn all OFF
	
	DDRD = (1 << PD4) | (1 << PD3) | (1 << PD2) | (1 << PD1); // LED // 0x1E; 0b00011110
	DDRD |= (0 << PD6) | (0 << PD7); // buttons
	PORTD = 0x00;
	
	DDRC = 0xFF;
	PORTC = 0x00;
	
	initTimers();

	g_Time.hh = 0;
	g_Time.mm = 0;
	g_Time.ss = 0;
	g_Time.fCng = 0;
	
	struct TIME g_DayLight = { 7, 00, 0, 0, 0 };
	struct TIME g_SunSet = { 22, 00, 0, 0, 0 };
	
	IND_Init();

	while(1)
    {
		if (g_Time.fUpd == 1)
		{
			int nOut = g_Time.mm;
			int nComma = -1;
			if (g_prgMode == 0)
			{
				nOut = g_Time.hh * 100;
				nOut += g_Time.mm;
				nComma = g_Time.ss & 1 ? 2 : -1;
			}
			else if (g_prgMode == 1)
			{
				nOut = g_Time.mm * 100;
				nOut += g_Time.ss;
				nComma = 2;
			}
			else if (g_prgMode == 2)
			{
				nOut = g_Time.hh * 100;
				nOut += g_Time.mm;
				nComma = 2;
			}
			else if (g_prgMode == 3)
			{
				nOut = g_Time.hh * 100;
				nOut += g_Time.mm;
				nComma = 2;
			}
			else if (g_prgMode == 4)
			{
				nOut = OCR1A;
				nComma = 4;
			}
			else if (g_prgMode == 5)
			{
				nOut = OCR1A;
				nComma = g_Time.ss & 1 ? 4 : -1;
			}
			IND_OutInt(nOut, nComma);
						
			g_Time.fUpd = 0;
			g_uchLightUpd++;
			
			if (g_Time.hh == g_DayLight.hh && g_Time.mm == g_DayLight.mm && g_Time.ss == 0)
			{
				g_DayLight.fUpd = 1;
				g_uchLightUpd = 0;
				SETBIT(TCCR1A, COM1A1);
			}
			else if (g_Time.hh == g_SunSet.hh && g_Time.mm == g_SunSet.mm && g_Time.ss == 0)
			{
				g_SunSet.fUpd = 1;
				g_uchLightUpd = 0;
			}
			
			if (g_DayLight.fUpd == 1 && g_uchLightUpd == DLUPDPER)
			{
				if (++OCR1A == MAXPWM)
				{
					g_DayLight.fUpd = 0;
				}
				g_uchLightUpd = 0;
			}
			else if (g_SunSet.fUpd == 1 && g_uchLightUpd == SSUPDPER)
			{
				if (--OCR1A == 0)
				{
					g_SunSet.fUpd = 0;
					CLEARBIT(TCCR1A, COM1A1);
				}
				g_uchLightUpd = 0;
			}
		}
		
		if (++fUpd == 100)
		{
			IND_Update();
			fUpd = 0;
		}			
		
		// Buttons
		if (BTN1DOWN)
		{
			if (g_chBtn1State == 0)
			{
				if (g_nBtn1 > 50)
				{
					g_chBtn1State = 2;
				}
			}
		}
		else
		{
			g_chBtn1State = (g_nBtn1 > 1 && g_chBtn1State == 0) ? 1 : 0;
			g_nBtn1 = 0;
		}

		if (g_chBtn1State == 1) // click
		{
			if (g_prgMode == 0)
			{
				g_prgMode = 1;
			}
			else if (g_prgMode == 1)
			{
				g_prgMode = 4;
			}
			else if (g_prgMode == 4)
			{
				g_prgMode = 0;
			}
			else if (g_prgMode == 2)
			{
				if (++g_Time.hh == 24)
					g_Time.hh = 0;
				g_Time.fUpd = 1;
				g_Time.fCng = 1;
			}
			else if (g_prgMode == 3)
			{
				if (++g_Time.mm == 60)
					g_Time.mm = 0;
				g_Time.fUpd = 1;
				g_Time.fCng = 1;
			}
			else if (g_prgMode == 5)
			{
				if (OCR1A < 10)
					OCR1A = 0;
				else
					OCR1A -= 10;
			}
			g_chBtn1State = 3;
		}
		else if (g_chBtn1State == 2) // long down
		{
			if (g_prgMode == 0)
			{
				IND_BlinkMask(0x03);
				g_prgMode = 2;
				g_Time.fUpd = 1;
			}
			else if (g_prgMode == 2)
			{
				IND_BlinkMask(0x0C);
				g_prgMode = 3;
				g_Time.fUpd = 1;
			}
			else if (g_prgMode == 3)
			{
				IND_BlinkMask(0x00);
				if (g_Time.fCng == 1)
				{				
					g_Time.ss = 0;
					g_prgMode = 0;
				}
				else
				{
					g_prgMode = 5;
				}
				g_Time.fUpd = 1;
				g_Time.fCng = 0;
			}
			else if (g_prgMode == 5)
			{
				g_prgMode = 0;
				g_Time.fUpd = 1;
				g_Time.fCng = 0;
			}
			g_chBtn1State = 3;
		}
		
		if (BTN2DOWN)
		{
			if (g_chBtn2State == 0)
			{
				if (g_nBtn2 > 50)
				{
					g_chBtn2State = 2;
				}
			}
		}
		else
		{
			g_chBtn2State = (g_nBtn2 > 1 && g_chBtn2State == 0) ? 1 : 0;
			g_nBtn2 = 0;
		}

		if (g_chBtn2State == 1) // click
		{
			if (g_prgMode == 2)
			{
				if (g_Time.hh-- == 0)
					g_Time.hh = 23;
				g_Time.fUpd = 1;
				g_Time.fCng = 1;
			}
			else if (g_prgMode == 3)
			{
				if (g_Time.mm-- == 0)
				g_Time.mm = 59;
				g_Time.fUpd = 1;
				g_Time.fCng = 1;
			}
			else if (g_prgMode == 5)
			{
				if (OCR1A > MAXPWM - 10)
					OCR1A = MAXPWM;
				else
					OCR1A += 10;
			}
			g_chBtn2State = 3;
		}
		else if (g_chBtn2State == 2) // long click
		{
			if (g_prgMode == 4)
			{
				if (g_DayLight.fUpd == 0 && OCR1A == 0)
				{
					g_DayLight.fUpd = 1;
					g_uchLightUpd = 0;
					OCR1A = 1;
					SETBIT(TCCR1A, COM1A1);
				}
				else if (g_DayLight.fUpd == 1 && OCR1A > 0 && OCR1A < MAXPWM)
				{
					g_DayLight.fUpd = 0;
					OCR1A = MAXPWM;
				}
				else if (g_DayLight.fUpd == 0 && OCR1A == MAXPWM)
				{
					OCR1A = MAXPWM - 1;
					g_SunSet.fUpd = 1;
					g_uchLightUpd = 0;
				}
				else if (g_SunSet.fUpd == 1 && OCR1A < MAXPWM)
				{
					g_SunSet.fUpd = 0;
					OCR1A = 0;
					CLEARBIT(TCCR1A, COM1A1);
					CLEARBIT(PORTB, PB1);
				}
			}
			g_chBtn2State = 3;
		}
    }
}
