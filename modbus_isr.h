//modbus realization
//
//values
enum modbus_exceptions{
	illegal_function=1,
	illegal_data_address=2,
	illegal_data_value=3,
	slave_device_failure=4,
	acknowledge=5,
	slave_device_busy=6
};
static 	int8	MB_Buffer[512];		//incoming buffer
static	int16	MB_Length;			//message length
static	int16	ValidReq;			//valid req count
static	int16	CRCReq;				//CRC error count
static	int16	ExcReq;				//ecxcept error count
//
//CRC table
/* Table of CRC values for high�order byte */
const int8 modbus_auchCRCHi[] = {
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
   0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
   0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
   0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
   0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
   0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
   0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
   0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
   0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
   0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
   0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
   0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
   0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
   0x40
};

/* Table of CRC values for low�order byte */
const int8 modbus_auchCRCLo[] = {
   0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
   0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
   0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
   0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
   0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
   0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
   0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
   0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
   0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
   0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
   0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
   0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
   0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
   0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
   0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
   0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
   0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
   0x40
};
//procedures
//
//
//UART preset
#USE RS232(baud=19200, xmit=TX, rcv=RX, enable=TR_DIR, parity=N)
void	RsInit(void){
	MB_Length=0;	//set clear buffer
	//select rate
/*	switch(Speed){
		case	1200:	setup_uart(1200);
						break;
		case	1800:	setup_uart(1800);
						break;
		case	2400:	setup_uart(2400);
						break;
		case	4800:	setup_uart(4800);
						break;
		case	7200:	setup_uart(7200);
						break;
		case	9600:	setup_uart(9600);
						break;
		case	14400:	setup_uart(14400);
						break;
		case	19200:	setup_uart(19200);
						break;
		default:		setup_uart(9600);
	};	*/
}
//

//usart settings

//usart interrupt vector - packet interception
#INT_RDA
void	Incoming(void){
	MB_Buffer[MB_Length]=getc();	//read stream
	MB_Length++;					//set new point
	output_toggle(STAT);
	set_timer1((int16)(0xffff-35000000/Speed));	//set wait time
	clear_interrupt(INT_TIMER1);		//clear timer 1 int
	enable_interrupts(INT_TIMER1);	//enable timer 1 interrupt
//	clear_interrupt(int_rda);		//clear interrupt flag
}
//
//calculate CRC16 : 1st - buffer, 2d - length of data
int16	ModbusCRC(int8 * buf, int8 len){
	int8	i,index;
	int8	CRC_Low=0xff,CRC_High=0xff;
	//
	for(i=0; i<len; i++){
		index=CRC_High^buf[i];
		CRC_High=CRC_Low^modbus_auchCRCHi[index];
		CRC_Low=modbus_auchCRCLo[index];
	}
	//
	return	make16(CRC_High,CRC_Low);
}
//read registers
static int16	CRCr,CRCc;
static int8	i,j;
//

//add register data
void	AddRegData(int8 reg){
			restart_wdt();
			switch(reg){
				case 0x00:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x01:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x02:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x03:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x04:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x05:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x06:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x07:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x08:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x09:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x0A:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x0B:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x0C:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x0D:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x0E:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x0F:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x10:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x11:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x12:	//speed register
					MB_Buffer[j]=make8(Speed,1);	//high part
					j++;
					MB_Buffer[j]=make8(Speed,0);	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x13:	//host address register
					MB_Buffer[j]=make8(Addr,1);	//high part
					j++;
					MB_Buffer[j]=make8(Addr,0);	//low part
					j++;								
					MB_Length+=2;
					break;
				case 0x14:	//Valid request register
					MB_Buffer[j]=make8(ValidReq,1);	//high part
					j++;
					MB_Buffer[j]=make8(ValidReq,0);	//low part
					j++;								
					MB_Length+=2;
					break;
				case 0x15:	//CRC error register
					MB_Buffer[j]=make8(CRCReq,1);	//high part
					j++;
					MB_Buffer[j]=make8(CRCReq,0);	//low part
					j++;								
					MB_Length+=2;
					break;
				case 0x16:	//Exception register
					MB_Buffer[j]=make8(ExcReq,1);	//high part
					j++;
					MB_Buffer[j]=make8(ExcReq,0);	//low part
					j++;								
					MB_Length+=2;
					break;
				case 0x17:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x18:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x19:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x1A:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x1B:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x1C:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x1D:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x1E:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x1F:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x20:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x21:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x22:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x23:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x24:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x25:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x26:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x27:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x28:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x29:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x2A:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x2B:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x2C:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x2D:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x2E:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x2F:	//dummy command					
					MB_Buffer[j]=0x00;	//high part
					j++;
					MB_Buffer[j]=0x00;	//low part
					j++;
					MB_Length+=2;
					break;
				case 0x30:	//Scan code register
					MB_Buffer[j]=make8(ScanCode,1);	//high part
					j++;
					MB_Buffer[j]=make8(ScanCode,0);	//low part
					j++;
					//ScanCode=0;	//reset scan code after read
					MB_Length+=2;
					break;
				case 0x31:	//relay register
					MB_Buffer[j]=make8(Relay,1);	//high part
					j++;
					MB_Buffer[j]=make8(Relay,0);	//low part
					j++;								
					MB_Length+=2;
					break;
				case 0x32:	//output value 0 register
					MB_Buffer[j]=make8(ValOut0,1);	//high part
					j++;
					MB_Buffer[j]=make8(ValOut0,0);	//low part
					j++;								
					MB_Length+=2;
					break;
				case 0x33:	//output value 1 register
					MB_Buffer[j]=make8(ValOut1,1);	//high part
					j++;
					MB_Buffer[j]=make8(ValOut1,0);	//low part
					j++;								
					MB_Length+=2;
					break;
				case 0x34:	//Out range 0 register
					MB_Buffer[j]=make8(ValPot0,1);	//high part
					j++;
					MB_Buffer[j]=make8(ValPot0,0);	//low part
					j++;								
					MB_Length+=2;
					break;
				case 0x35:	//Out range 1 register
					MB_Buffer[j]=make8(ValPot1,1);	//high part
					j++;
					MB_Buffer[j]=make8(ValPot1,0);	//low part
					j++;								
					MB_Length+=2;
					break;
				default:	//no valid registers							
					MB_Buffer[1]|=0x80;
					MB_Buffer[2]=illegal_data_value;
					MB_Length=5;
					ExcReq++;
					ValidReq--;
			};	
}
//
//write registers
void	SetRegData(int8 reg){
			restart_wdt();
			switch(reg){
				case 0x00:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x01:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x02:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x03:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x04:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x05:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x06:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x07:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x08:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x09:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x0A:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x0B:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x0C:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x0D:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x0E:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x0F:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x10:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x11:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x12:	//set speed register
					j+=2;	//set next data
					break;
				case 0x13:	//host address register - no action
					//read data from buffer
					Addr=make16(MB_Buffer[j],MB_Buffer[j+1]);
					j+=2;	//set next data
					break;
				case 0x14:	//Valid request register	- no acton
					j+=2;	//set next data
					break;
				case 0x15:	//CRC error register - no action
					j+=2;	//set next data
					break;
				case 0x16:	//Exception register - no action
					j+=2;	//set next data
					break;
				case 0x17:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x18:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x19:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x1A:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x1B:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x1C:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x1D:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x1E:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x1F:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x20:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x21:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x22:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x23:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x24:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x25:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x26:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x27:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x28:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x29:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x2A:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x2B:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x2C:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x2D:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x2E:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x2F:	//dummy command					
					j+=2;	//set next data
					break;
				case 0x30:	//Scan code register -  no action
					j+=2;	//set next data
					break;
				case 0x31:	//relay register
					Relay=make16(MB_Buffer[j],MB_Buffer[j+1]);
					SetRelay(Relay);	//set discrete outs
					j+=2;	//set next data
					break;
				case 0x32:	//output value 0 register				
					ValOut0=make16(MB_Buffer[j],MB_Buffer[j+1]);
					SetOutDac0(ValOut0);	//set dac
					j+=2;	//set next data
					break;
				case 0x33:	//output value 1 register
					ValOut1=make16(MB_Buffer[j],MB_Buffer[j+1]);
					SetOutDac1(ValOut1);	//set dac
					j+=2;	//set next data
					break;
				case 0x34:	//Out range 0 register
					ValPot0=make16(MB_Buffer[j],MB_Buffer[j+1]);
					SetAoutRange0(ValPot0);	//set pot
					j+=2;	//set next data
					break;
				case 0x35:	//Out range 1 register
					ValPot1=make16(MB_Buffer[j],MB_Buffer[j+1]);
					SetAoutRange1(ValPot1);	//set pot
					j+=2;	//set next data
					break;
				default:	//no valid registers							
					MB_Buffer[1]|=0x80;
					MB_Buffer[2]=illegal_data_value;
					MB_Length=5;
					ExcReq++;
					ValidReq--;
			};	
}
//
//timer 2 interrupt vector - packet recognition
#INT_TIMER1
void	Reaction(void){
	int8	StAddr,EnAddr;	
	//end of tramsmittion process
	disable_interrupts(INT_RDA);
//	output_low(STAT);	//on led
	//
	if (MB_Length<4)	goto exit;	//exit if to small
	//detect adress
	if(MB_Buffer[0]!=make8(Addr,0))	if (MB_Buffer[0]!=0)	goto exit;	//if not broadcast & not qwn address - then exit
	//calculate CRC
	CRCc=ModbusCRC(MB_Buffer,MB_Length-2);
	CRCr=make16(MB_Buffer[MB_Length-2],MB_Buffer[MB_Length-1]);
	//
	if(CRCc!=CRCr){				//if error CRC then exit
		CRCReq++;	
		goto exit;
	};
	//proceed command
	//analizing function code
	switch(MB_Buffer[1]){
		//CMODE
		case	0x42:
				if(MB_Length==5){
					if(MB_Buffer[2]==2){
						reset_cpu();
						}else{
						MB_Buffer[1]|=0x80;
						MB_Buffer[2]=illegal_data_value;
						ExcReq++;
						MB_Length=5;
						};
					}else{
					MB_Buffer[1]|=0x80;
					MB_Buffer[2]=illegal_data_address;
					MB_Length=5;
					ExcReq++;
					};
				break;
		//get registers
		case	0x44:
				if(MB_Length==7){
					//valig req propose
					MB_Length=4;
					ValidReq++;
					//action
					//forming answer
					j=2;	//set start answer pointer
					StAddr=MB_Buffer[3];
					EnAddr=(MB_Buffer[3]+MB_Buffer[4]);
					for(i=StAddr; i<EnAddr; i++){
						AddRegData(i);		//get registers
					};
					}else{
					MB_Buffer[1]|=0x80;
					MB_Buffer[2]=illegal_data_address;
					MB_Length=5;
					ExcReq++;
					};		
				break;
		//set registers
		case	0x45:
				if((MB_Length>=8)&&(!(MB_Length&0x01))){
					//action
					//action
					j=4;	//set start answer pointer
					StAddr=MB_Buffer[3];
					EnAddr=MB_Length-2;
					//valig req propose
					MB_Length=4;
					ValidReq++;
					for(i=StAddr; j<EnAddr; i++){
						SetRegData(i);		//get registers
					};
					//forming answer
					}else{
					MB_Buffer[1]|=0x80;
					MB_Buffer[2]=illegal_data_address;
					MB_Length=5;
					ExcReq++;
					};		
				break;
		//GetLCDStatus
		case	0x46:
				if(MB_Length==4){
					MB_Buffer[2]=GetStatusLCD();
					MB_Length=5;
					ValidReq++;
					}else{
					MB_Buffer[1]|=0x80;
					MB_Buffer[2]=illegal_data_address;
					MB_Length=5;
					ExcReq++;
					};		
				break;
		//SetLCDCommand
		case	0x47:
				if(MB_Length==5){
					SetCommandLCD(MB_Buffer[2]);
					MB_Length=4;
					ValidReq++;
					}else{
					MB_Buffer[1]|=0x80;
					MB_Buffer[2]=illegal_data_address;
					MB_Length=5;
					ExcReq++;
					};		
				break;
		//SetLCDData
		case	0x48:
				if(MB_Length==5){
					SetDataLCD(MB_Buffer[2]);
					MB_Length=4;
					ValidReq++;
					}else{
					MB_Buffer[1]|=0x80;
					MB_Buffer[2]=illegal_data_address;
					MB_Length=5;
					ExcReq++;
					};		
				break;
		//write to EEPROM command
		case	0x65:
				if(MB_Length==4){
					BUWrite();
					MB_Length=4;
					ValidReq++;
					}else{
					MB_Buffer[1]|=0x80;
					MB_Buffer[2]=illegal_data_address;
					MB_Length=5;
					ExcReq++;
					};		
				break;
		//ClrScr
		case	0x66:
				if(MB_Length==4){
					ClrTxt();
//					ClrGrph();
					MB_Length=4;
					ValidReq++;
					}else{
					MB_Buffer[1]|=0x80;
					MB_Buffer[2]=illegal_data_address;
					MB_Length=5;
					ExcReq++;
					};		
				break;
		//SetCursor
		case	0x67:
				if(MB_Length==6){
					if((MB_Buffer[2]<COLS)&(MB_Buffer[3]<ROWS)){	//if coord in screen then on cursor
						SetCommandLCD(0x9f);	//on cursor
						SetDataLCD(MB_Buffer[2]);			//set x
						SetDataLCD(MB_Buffer[3]);			//set y
						SetCommandLCD(0x21);	//set position
						}
						else	SetCommandLCD(0x9C);	//else off cursor
					MB_Length=4;
					ValidReq++;
					}else{
					MB_Buffer[1]|=0x80;
					MB_Buffer[2]=illegal_data_address;
					MB_Length=5;
					ExcReq++;
					};		
				break;
		//PrintXY
		case	0x68:
				if(MB_Length>6){
					for(i=4;i< MB_Length-2; i++){
						string[i-4]=MB_Buffer[i];
					};
					string[MB_Length-6]=0;
					PrintXY(MB_Buffer[2],MB_Buffer[3],string);
					MB_Length=4;
					ValidReq++;
					}else{
					MB_Buffer[1]|=0x80;
					MB_Buffer[2]=illegal_data_address;
					MB_Length=5;
					ExcReq++;
					};		
				break;
		//Draw line/Point
		case	0x69:
				switch(MB_Length){
					case 7:	//point
						//coord validation
						i=0;
						if(MB_Buffer[2]>159)	i=0xff;
						if(MB_Buffer[3]>127)	i=0xff;
						if(i==0){
//							glcd_pixel(MB_Buffer[2],MB_Buffer[3],bit_test(MB_Buffer[4],0));
							MB_Length=4;
							ValidReq++;
							}
							else{								
								MB_Buffer[1]|=0x80;
								MB_Buffer[2]=illegal_data_value;
								MB_Length=5;
								ExcReq++;
								};
						break;
					case 9:	//line
						//coord validation
						i=0;
						if(MB_Buffer[2]>159)	i=0xff;
						if(MB_Buffer[3]>127)	i=0xff;
						if(MB_Buffer[4]>159)	i=0xff;
						if(MB_Buffer[5]>127)	i=0xff;
						if(i==0){
//							glcd_line(MB_Buffer[2],MB_Buffer[3],MB_Buffer[4],MB_Buffer[5],bit_test(MB_Buffer[6],0));
							MB_Length=4;
							ValidReq++;
							}
							else{								
								MB_Buffer[1]|=0x80;
								MB_Buffer[2]=illegal_data_value;
								MB_Length=5;
								ExcReq++;
								};
						break;
					default:					
						MB_Buffer[1]|=0x80;
						MB_Buffer[2]=illegal_data_address;
						MB_Length=5;
						ExcReq++;
					};
				break;
		//set speed
		case	0x6A:
				if(MB_Length==5){
					MB_Length=4;
					ValidReq++;
					switch(MB_Buffer[2]){
						case	0x00:	//1200
							Speed=1200;
							setup_uart(1200);
							break;
						case	0x01:	//1800
							Speed=1800;
//							setup_uart(1800);
							break;
						case	0x02:	//2400
							Speed=2400;
//							setup_uart(2400);
							break;
						case	0x03:	//4800
							Speed=4800;
//							setup_uart(4800);
							break;
						case	0x04:	//7200
							Speed=7200;
//							setup_uart(7200);
							break;
						case	0x05:	//9600
							Speed=9600;
//							setup_uart(9600);
							break;
						case	0x06:	//14400
							Speed=14400;
//							setup_uart(14400);
							break;
						case	0x07:	//19200
							Speed=19200;
//							setup_uart(19200);
							break;
						default:
							MB_Buffer[1]|=0x80;
							MB_Buffer[2]=illegal_data_value;
							MB_Length=5;
							ExcReq++;
					};
					//pause for master
					delay_ms(100);
					}else{
					MB_Buffer[1]|=0x80;
					MB_Buffer[2]=illegal_data_address;
					MB_Length=5;
					ExcReq++;
					};		
				break;
		//send id
		case	0x6B:
				if(MB_Length==4){
					MB_Buffer[2]="M";
					MB_Buffer[3]="W";
					MB_Buffer[4]="T";
					MB_Buffer[5]="-";
					MB_Buffer[6]="T";
					MB_Buffer[7]="M";
					MB_Buffer[8]="3";
					MB_Buffer[9]=" ";
					MB_Buffer[10]="v";
					MB_Buffer[11]="1";
					MB_Buffer[12]=".";
					MB_Buffer[13]="0";
					MB_Buffer[14]="0";
					MB_Length=16;
					ValidReq++;
					}else{
					MB_Buffer[1]|=0x80;
					MB_Buffer[2]=illegal_data_address;
					MB_Length=5;
					ExcReq++;
					};		
				break;
		//not recognized
		default:		//not recognized command
		MB_Buffer[1]|=0x80;	//set error code
		MB_Buffer[2]=illegal_function;
		MB_Length=5;
		ExcReq++;
	};	
	//send answer
	if(MB_Buffer[0]!=0){	//if unicast req - then send answer
	CRCc=ModbusCRC(MB_Buffer,MB_Length-2);	//calc CRC
	MB_Buffer[MB_Length-2]=make8(CRCc,1);	//high CRC
	MB_Buffer[MB_Length-1]=make8(CRCc,0);	//low CRC
	//send
	for(i=0; i<MB_Length; i++){
		putc(MB_Buffer[i]);
//		output_low(STAT);	//toggle led
		restart_wdt();
		};
	};	
	//
	
exit:	//exit
//	output_high(STAT);	//on led
	MB_Length=0;
	clear_interrupt(INT_RDA);	
	disable_interrupts(INT_TIMER1);	
	enable_interrupts(INT_RDA);
}
//