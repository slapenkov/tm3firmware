#include <18F4520.h>
#device ICD=TRUE
#device adc=10
#FUSES WDT                      //Watch Dog Timer 
#FUSES WDT1                     //Watch Dog Timer uses 1:1 Postscale
#FUSES INTRC_IO                 //Internal RC Osc, no CLKOUT
#FUSES NOPROTECT                //Code not protected from reading
#FUSES NOBROWNOUT               //No brownout reset
#FUSES BORV20                   //Brownout reset at 2.7V
#FUSES PUT                      //Power Up Timer
#FUSES NOCPD                    //No EE protection
#FUSES STVREN                   //Stack full/underflow will cause reset
#FUSES NODEBUG                  //No Debug mode for ICD
#FUSES LVP                      //Low Voltage Programming on B3(PIC16) or B5(PIC18)
#FUSES NOWRT                    //Program memory not write protected
#FUSES NOWRTD                   //Data EEPROM not write protected
#FUSES IESO                     //Internal External Switch Over mode enabled
#FUSES FCMEN                    //Fail-safe clock monitor enabled
#FUSES PBADEN                   //PORTB pins are configured as analog input channels on RESET
#FUSES NOWRTC                   //configuration not registers write protected
#FUSES NOWRTB                   //Boot block not write protected
#FUSES NOEBTR                   //Memory not protected from table reads
#FUSES NOEBTRB                  //Boot block not protected from table reads
#FUSES NOCPB                    //No Boot Block code protection
#FUSES LPT1OSC                  //Timer1 configured for low-power operation
#FUSES NOMCLR                   //Master Clear pin enabled
#FUSES XINST                    //Extended set extension and Indexed Addressing mode enabled

#use delay(clock=32000000,RESTART_WDT)
#define	_WR		PIN_A0
#define	_RD		PIN_A1
#define	_CE		PIN_A2
#define	C_D		PIN_A3
#define	DAC0	PIN_A4
#define	DAC1	PIN_A5
#define	POT0	PIN_A6
#define	POT1	PIN_A7

#define	KB_A	PIN_B0
#define	KB_B	PIN_B1
#define	KB_C	PIN_B2
#define	KB_D	PIN_B3
#define	KB_1	PIN_B4
#define	KB_2	PIN_B5
#define	KB_3	PIN_B6
#define	KB_4	PIN_B7

#define	OUT2	PIN_C0
#define	STAT	PIN_C1
#define	TR_DIR	PIN_C2
#define	SCK		PIN_C3
#define	SDI		PIN_C4
#define	SDO		PIN_C5
#define	TX		PIN_C6
#define	RX		PIN_C7

#define	OUT0	PIN_E0
#define	OUT1	PIN_E1
#define	LCD_BL	PIN_E2

#bit	TMR0IF=0xff2.2
