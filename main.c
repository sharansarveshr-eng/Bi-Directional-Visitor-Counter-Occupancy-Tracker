/*
 * File:   main.c
 * Project: Bidirectional Visitor Counter
 * Microcontroller: PIC16F877A
 * LCD: LM016L (4-bit mode)
 */

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF

#define _XTAL_FREQ 20000000

#include <xc.h>
#include <stdio.h>

// LCD Pin Definitions ? PORTD (exactly as Gemini's working code)
#define RS RD0
#define EN RD1
#define D4 RD2
#define D5 RD3
#define D6 RD4
#define D7 RD5

// PIR sensors ? PORTB (exactly as Gemini's working code)
// PIR1 = RB0,  PIR2 = RB1

//---------------- LCD FUNCTIONS ----------------//
void Lcd_SetBit(char data)
{
    D4 = (data >> 0) & 1;
    D5 = (data >> 1) & 1;
    D6 = (data >> 2) & 1;
    D7 = (data >> 3) & 1;
}

void Lcd_Cmd(char cmd)
{
    RS = 0;
    Lcd_SetBit(cmd >> 4);
    EN = 1; __delay_ms(2); EN = 0;
    Lcd_SetBit(cmd);
    EN = 1; __delay_ms(2); EN = 0;
}

void Lcd_Clear()
{
    Lcd_Cmd(0x01);
    __delay_ms(2);
}

void Lcd_Init()
{
    Lcd_SetBit(0x00);
    __delay_ms(20);
    Lcd_Cmd(0x02);
    Lcd_Cmd(0x28);
    Lcd_Cmd(0x0C);
    Lcd_Cmd(0x06);
    Lcd_Clear();
}

void Lcd_Write_Char(char data)
{
    RS = 1;
    Lcd_SetBit(data >> 4);
    EN = 1; __delay_ms(2); EN = 0;
    Lcd_SetBit(data);
    EN = 1; __delay_ms(2); EN = 0;
}

void Lcd_Print_String(char *str)
{
    int i;
    for(i = 0; str[i] != 0; i++)
        Lcd_Write_Char(str[i]);
}

void Lcd_Set_Cursor(char row, char col)
{
    if(row == 1)
        Lcd_Cmd(0x80 + col);
    else
        Lcd_Cmd(0xC0 + col);
}

//---------------- WAIT FOR SECOND PIR ----------------//
// Waits up to 5 seconds for the given PIR pin to go HIGH
// pirNum: 1 = RB0 (PIR1),  2 = RB1 (PIR2)
// Returns 1 if triggered within time, 0 if timed out
char WaitForPIR(char pirNum)
{
    int timeout = 5000;   // 5000 x 1ms = 5 seconds
    while(timeout > 0)
    {
        if(pirNum == 1 && RB0 == 1) return 1;
        if(pirNum == 2 && RB1 == 1) return 1;
        __delay_ms(1);
        timeout--;
    }
    return 0;
}

//---------------- MAIN PROGRAM ----------------//
void main()
{
    int count = 0;
    char buffer[16];

    ADCON1 = 0x06;              // Disable analog
    OPTION_REGbits.nRBPU = 0;  // Enable PORTB pull-ups
    TRISD = 0x00;               // PORTD output ? LCD
    TRISB0 = 1;                 // RB0 input ? PIR1
    TRISB1 = 1;                 // RB1 input ? PIR2
    PORTB = 0x00;

    Lcd_Init();
    Lcd_Set_Cursor(1, 0);
    Lcd_Print_String("Visitor Counter ");
    Lcd_Set_Cursor(2, 0);
    Lcd_Print_String("Count: 0        ");

    while(1)
    {
        // -----------------------------------------------
        // ENTRY: PIR1 (RB0) triggers FIRST
        //        then PIR2 (RB1) within 5s => count++
        // -----------------------------------------------
        if(RB0 == 1 && RB1 == 0)
        {
            __delay_ms(100);            // debounce
            if(RB0 == 1)                // confirm PIR1 still HIGH
            {
                if(WaitForPIR(2))       // wait for PIR2
                {
                    count++;
                    Lcd_Set_Cursor(1, 0);
                    Lcd_Print_String("  >> ENTRY <<   ");
                    Lcd_Set_Cursor(2, 0);
                    sprintf(buffer, "Count: %d       ", count);
                    Lcd_Print_String(buffer);
                    __delay_ms(1000);
                    Lcd_Set_Cursor(1, 0);
                    Lcd_Print_String("Visitor Counter ");
                    while(RB0 == 1 || RB1 == 1);   // wait for both to go LOW
                    __delay_ms(200);
                }
            }
        }

        // -----------------------------------------------
        // EXIT: PIR2 (RB1) triggers FIRST
        //       then PIR1 (RB0) within 5s => count--
        // -----------------------------------------------
        else if(RB1 == 1 && RB0 == 0)
        {
            __delay_ms(100);            // debounce
            if(RB1 == 1)                // confirm PIR2 still HIGH
            {
                if(WaitForPIR(1))       // wait for PIR1
                {
                    if(count > 0) count--;
                    Lcd_Set_Cursor(1, 0);
                    Lcd_Print_String("  << EXIT  >>   ");
                    Lcd_Set_Cursor(2, 0);
                    sprintf(buffer, "Count: %d       ", count);
                    Lcd_Print_String(buffer);
                    __delay_ms(1000);
                    Lcd_Set_Cursor(1, 0);
                    Lcd_Print_String("Visitor Counter ");
                    while(RB0 == 1 || RB1 == 1);   // wait for both to go LOW
                    __delay_ms(200);
                }
            }
        }
    }
}