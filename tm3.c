//modified 5.12.2010
//add LCD get status timeout - if LCD not responce - then ignore it and continue main execution
//function WaitStatusLCD

#include "tm3.h"
#include <stdlib.h>
#include "ee_var.h"
#include "font_6x8r.h"
//
//
char	string[256];
//
//LCD definition
#define	CHARBASE	0x0000			//char generator base adress
#define	TEXTBASE	0x0800			//text home adress
#define	GRAPHBASE	0x0a00			//graph home adress
#define	COLS		0x14			//20 chars/row
#define	ROWS		0x10			//16 char/col
#define	GLCD_WIDTH	0xA0			//LCD width
//LCD Variables

//LCD routines
int8	k,l;
//
//Get LCD status
int8	GetStatusLCD(void){
	int8	i;				//temporary
	
	restart_wdt();
	set_tris_d(0xff);		//set port D as input
	output_high(C_D);		//command mode
	output_high(_WR);		//
	output_low(_RD);		//read mode
	output_low(_CE);		//strobe
//	delay_us(1);
	i=input_d();			//read status
	output_high(_CE);
	output_high(_RD);
	set_tris_d(0x00);		//set D as output
	restart_wdt();
	return i;
}
//
//wait LCD status
int8	WaitStatusLCD(void){
	int8	tmp;
	
	tmp=0;	//init timeout
	
	while (((GetStatusLCD()&0x03)!=0x03)){
		tmp++;
	};
	//timeout detection
	if (tmp==0x80){
		//set exit code
		//reset timeout flag
	}
	else tmp=0;	//set exit code OK
	return 0;
}
//
//set command
void	SetCommandLCD(int8 cmd){
	while ((GetStatusLCD()&0x03)!=0x03);
		output_high(C_D);		//command mode
		output_high(_RD);
		output_low(_WR);		//write mode
		output_d(cmd);			//set command
//		delay_us(1);
		output_low(_CE);		//strobe
//		delay_us(1);
		output_high(_CE);
		output_high(_WR);
}
//
//set data
void	SetDataLCD(int8 data){
	while ((GetStatusLCD()&0x03)!=0x03);
		output_low(C_D);		//set data mode
		output_high(_RD);
		output_low(_WR);		//write mode
		output_d(data);			//set data
//		delay_us(1);
		output_low(_CE);		//strobe
//		delay_us(1);
		output_high(_CE);
		output_high(_WR);
		output_high(C_D);
}
//
//set data word
void	SetData2LCD(int16 data){
	while ((GetStatusLCD()&0x03)!=0x03);
		output_low(C_D);		//set data mode
		output_high(_RD);
		output_low(_WR);		//write mode
		output_d(make8(data,0));			//set data low
		delay_us(5);
		output_low(_CE);		//strobe
		delay_us(5);
		output_high(_CE);
		output_high(_WR);
		output_high(C_D);
		
	while ((GetStatusLCD()&0x03)!=0x03);
			output_low(C_D);		//set data mode
			output_high(_RD);
			output_low(_WR);		//write mode
			output_d(make8(data,1));			//set data high
			delay_us(5);
			output_low(_CE);		//strobe
			delay_us(5);
			output_high(_CE);
			output_high(_WR);
			output_high(C_D);	
}
//
//set adress LCD
void	SetAddrLCD(int16 address){
	//low byte
	while ((GetStatusLCD()&0x03)!=0x03);
		output_low(C_D);		//set data mode
		output_high(_RD);
		output_low(_WR);		//write mode
		output_d(make8(address,0));			//set data low
		delay_us(5);
		output_low(_CE);		//strobe
		delay_us(5);
		output_high(_CE);
		output_high(_WR);
		output_high(C_D);
		//high byte
	while ((GetStatusLCD()&0x03)!=0x03);
			output_low(C_D);		//set data mode
			output_high(_RD);
			output_low(_WR);		//write mode
			output_d(make8(address,1));			//set data high
			delay_us(5);
			output_low(_CE);		//strobe
			delay_us(5);
			output_high(_CE);
			output_high(_WR);
			output_high(C_D);	
			//command
	while ((GetStatusLCD()&0x03)!=0x03);
				output_high(C_D);		//command mode
				output_high(_RD);
				output_low(_WR);		//write mode
				output_d(0x24);			//set command
				delay_us(5);
				output_low(_CE);		//strobe
				delay_us(5);
				output_high(_CE);
				output_high(_WR);
}
//
//pixel
void	glcd_pixel(int8 x, int8 y, int1 color){
	int8	yr;
	yr=8*ROWS-Y-1;
	SetAddrLCD(GRAPHBASE+(int16)X/8+(int16)yr*COLS);//set adress
	//Draw pixel
	if (color) 
	SetCommandLCD((0xf8|(7-(X-(X/8)*8)))); 
	else 
	SetCommandLCD((0xf0|(7-(X-(X/8)*8)))); 
}
//
//define graphics functions
#include "graph_lcd.h"
//
//Chartable load
void	CharTableLoad(void){
	int16 i;
	SetData2LCD(CHARBASE);	//set adres pointer to chargen area
	SetCommandLCD(0x24);	//
	//
	SetCommandLCD(0xb0);	//set automode start
	//
    for (i=0;i<FONTDATAMAX; ++i) {
     	SetDataLCD(fontdata_6x8[i]);
    	};
	//
	SetCommandLCD(0xb2);	//stop automode
}
//
//print string
void	PrintXY(int8 X, int8 Y, char *s){
	SetAddrLCD(TEXTBASE+(int16)X+(int16)Y*COLS); 
	SetCommandLCD(0xb0);	//set automode start
	while(*s){
		SetDataLCD(*s++);
		};
	SetCommandLCD(0xb2);	//stop automode
}
//
//clear text screen
void	ClrTxt(void){
	int16 i;
	SetAddrLCD(TEXTBASE); 
	SetCommandLCD(0xb0);	//set automode start
	for(i=1; i<GRAPHBASE; i++){
		SetDataLCD(0x00);
		};
	SetCommandLCD(0xb2);	//stop automode
}
//
//clear graph screen
void	ClrGrph(void){
	int16 i;
	SetAddrLCD(GRAPHBASE); 
	SetCommandLCD(0xb0);	//set automode start
	for(i=1; i<(GRAPHBASE+8*ROWS*COLS); i++){
		SetDataLCD(0x00);
		};
	SetCommandLCD(0xb2);	//stop automode
}
//
//


//Global variables
static	int16	Speed;		//host speed
static	int16	Addr;		//host address
static	int16	ValPot0;	//output range value
static	int16	ValPot1;	//output range value
static	int16	ValOut0;	//output value
static	int16	ValOut1;	//output value
//keyboard
static	int16	ScanCode;	//keyboard scan code
static	int16	Relay;		//relay state
//Relay
void	SetRelay(int16 rel){
	//out0
	if(rel&0x0001){
		output_high(OUT0);
	}else	output_low(OUT0);
	//out1
	if(rel&0x0002){
		output_high(OUT1);
	}else	output_low(OUT1);
	//out2
	if(rel&0x0004){
		output_high(OUT2);
	}else	output_low(OUT2);
}
//
//Peripherals setup
void	InitMcu(void){		//initialization of MCU
	
   //oscillator configuration
   setup_oscillator(OSC_32MHZ|OSC_NORMAL|OSC_31250);
   //disable PSP
   setup_psp(PSP_DISABLED);
   //WDT config
   setup_wdt(WDT_ON);
   //disable comparators
   setup_comparator(NC_NC_NC_NC);
   setup_vref(FALSE);
   //config ADC module
   setup_adc_ports(NO_ANALOGS);
   setup_adc(ADC_OFF);
   
   //port configuration
   port_b_pullups(true);
   set_tris_b(0xff);
   //set high all selectors
   output_high(DAC0);
   output_high(DAC1);
   output_high(POT0);
   output_high(POT1);
   output_high(STAT);
   output_low(TR_DIR);	//receive mode
   output_high(LCD_BL);
   output_high(_WR);
   output_high(_RD);
   output_high(_CE);
   output_high(C_D);
   
   output_low(OUT0);
   output_low(OUT1);
   output_low(OUT2);
   
   
   //timers configuration
   setup_timer_0(RTCC_INTERNAL | RTCC_DIV_64 | RTCC_8_BIT);
   
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);	//setup 1uS increment period
   
   setup_timer_2(T2_DISABLED,0,1);
   
   setup_timer_3(T3_DISABLED|T3_DIV_BY_1);
   //setup communications
   //setup usart
   output_low(TR_DIR);	//enable receiving
   //SPI configuration
   setup_spi(SPI_MASTER|SPI_CLK_DIV_4|SPI_H_TO_L);
}
//
//read registers from EE
void	BURead(void){	//back up read
	//reading
	Speed=make16(read_eeprom(EE_Speed+1),read_eeprom(EE_Speed)); //
	Addr=make16(read_eeprom(EE_Host+1),read_eeprom(EE_Host)); //
	ValPot0=make16(read_eeprom(EE_POT0+1),read_eeprom(EE_POT0)); //
	ValPot1=make16(read_eeprom(EE_POT1+1),read_eeprom(EE_POT1)); //
}
//
//write registers to EE
void	BUWrite(void){	//back up write
	setup_wdt(WDT_OFF);
	//writing
	//int16
	write_eeprom(EE_Speed,make8(Speed,0));		write_eeprom(EE_Speed+1,make8(Speed,1));
	write_eeprom(EE_Host,make8(Addr,0));		write_eeprom(EE_Host+1,make8(Addr,1));
	write_eeprom(EE_POT0,make8(ValPot0,0));		write_eeprom(EE_POT0+1,make8(ValPot0,1));
	write_eeprom(EE_POT1,make8(ValPot1,0));		write_eeprom(EE_POT1+1,make8(ValPot1,1));
	
	setup_wdt(WDT_ON);
}
//
//Setup analog channel 0
//Range
void	SetAoutRange0(int16 value){
	output_low(POT0);	//select chip
	delay_us(5);
	spi_write(0x11);	//send command
	restart_wdt();
	spi_write(make8(value,0));	//send LSB of value
	delay_us(5);
	output_high(POT0);
}
void	SetAoutRange1(int16 value){
	output_low(POT1);	//select chip
	delay_us(5);
	spi_write(0x11);	//send command
	restart_wdt();
	spi_write(make8(value,0));	//send LSB of value
	delay_us(5);
	output_high(POT1);
}
//level
void	SetOutDac0(int16 value){
	output_low(DAC0);	//select chip
	delay_us(5);
	spi_write((make8(value,1)&0x0f)|0x10);	//send high part
	spi_write(make8(value,0));				//send low part
	delay_us(5);
	output_high(DAC0);
}
void	SetOutDac1(int16 value){
	output_low(DAC1);	//select chip
	delay_us(5);
	spi_write((make8(value,1)&0x0f)|0x10);	//send high part
	spi_write(make8(value,0));				//send low part
	delay_us(5);
	output_high(DAC1);
}
//
static	int16	keyPort=0xffff;	//current keys state
static	int16	keyPrev=0xffff;	//previous state of keys
static	int8	debTimer=0;	//debounce timer
static	int8	keyRows[4];		

#use fast_io(B)
//keyboard scanning
void	KeyScan(void){
	int8	i;
	output_b(0);	//clear latch of B
	//read keys
	for(i=0;i<4;i++){
		restart_wdt();
		set_tris_b(0xff^(0x01<<i));	//select row
		delay_us(10);	//wait
		keyRows[i]=input_b()&0xf0;			//read port
	};
	set_tris_b(0xff);
	//combine
	keyPort=make16((keyRows[0]|swap(keyRows[1])),(keyRows[2]|swap(keyRows[3])));
	//debounce
	if(keyPort^keyPrev){	
		debTimer=0;	//if state change -reset timer
		ScanCode=0;	//reset scancode
		};
	if(debTimer>19){		//if debounced
		if((keyPort^0xffff)!=0){
			ScanCode=keyPort^0xffff;
			};			// if key pressed - store inversed
		}else	debTimer++;	//else inc
	keyPrev=keyPort;		//store state
	restart_wdt();
}
//
//ISR
#include "modbus_isr.h"

//

void	main() {
	int8	symbol;
	int16	sym_count=0;
	
	InitMcu();			//hardware init
	BURead();			//restore backups
	RsInit();			//usart init
	//setup range&init value
	SetAoutRange0(ValPot0);
	SetAoutRange1(ValPot1);
	SetOutDac0(0x00);
	SetOutDac1(0x00);
	
	//LCD init
	SetData2LCD(CHARBASE>>11);	//send offset 0x0000h
	SetCommandLCD(0x22);	//set graphics home
	CharTableLoad();		//load charset table WIN 1251
	SetData2LCD(GRAPHBASE);	//send adress 0x0000h
	SetCommandLCD(0x42);	//set graphics home
	SetData2LCD(COLS);		//set col for 160x128
	SetCommandLCD(0x43);	//set graphics area
	SetData2LCD(TEXTBASE);	//set adress 0x0180h
	SetCommandLCD(0x40);	//set text home
	SetData2LCD(COLS);		//set cols
	SetCommandLCD(0x41);	//set text area
	SetCommandLCD(0x88);	//set LCD mode: OR, internal SG
	SetCommandLCD(0x9f);	//set view mode: graphics & text, cursor, blink
	SetCommandLCD(0xa0);	//set 1-row cursor
	ClrTxt();
	ClrGrph();
//	SetCommandLCD(0x9c);		//off cursor
	//greetings
	sprintf(string,"LCD Terminal Module");
	PrintXY(0,0,string);
	sprintf(string,"TM-3 v1.2");
	PrintXY(0,1,string);
	sprintf(string,"ID:");	
	PrintXY(0,3,string);
	sprintf(string,"%LX",Addr);
	PrintXY(3,3,string);
	sprintf(string,"Baud:");
	PrintXY(10,3,string);
	sprintf(string,"%lu",Speed);
	PrintXY(15,3,string);
	sprintf(string,"Подготовка");
	PrintXY(5,6,string);
	sprintf(string,"к работе...");
	PrintXY(5,7,string);
	sprintf(string,"Пожалуйста,ожидайте.");
	PrintXY(0,9,string);
	sprintf(string,"2013 OOO MBT");
	PrintXY(4,15,string);
	
	/*for(k=0; k<20; k++){
		for(l=0; l<16; l++)
		{
			sprintf(string,"%c",(int8)(k+20*l));
			restart_wdt();
			PrintXY(k,l,string);
		};
	};
   */
   //interrupt configuration
   disable_interrupts(INT_TIMER1);
   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);
   
	while(TRUE){
		restart_wdt();
		//keyboard debouncing
		if(TMR0IF){
			TMR0IF=0;
			KeyScan();
			if (sym_count>300){
				sym_count=0;
				output_toggle(STAT);
				}
				else sym_count++;
			
//			output_toggle(STAT);
			//screen processing
			
			/*SetAddrLCD(TEXTBASE); 
			SetCommandLCD(0xb0);	//set automode start
			symbol=rand();
			for (sym_count=0; sym_count<320; sym_count++){
			SetDataLCD(symbol++);
				};
			SetCommandLCD(0xb2);	//stop automode						*/
			};		
	}
}