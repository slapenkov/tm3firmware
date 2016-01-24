//TM-3 EEPROM storage structure
//define adresses of EEPROM locations
/*		LocName		Addr	Discr			*/
/*==========================================*/
#define	EE_Speed	0x00	//serial device speed
#define	EE_Host		0x02	//host address & serial number	<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<!!! Device ID!!!
#define	EE_POT0		0x04	//output range 0
#define	EE_POT1		0x06	//output range 1

//EEPROM data definition
#ROM	0xf00000={0x4B00,	//EE_Speed
				  0x0002,	//EE_Host				  
				  0x003C,	//EE_POT0
				  0x0044	//EE_POT1
}
//