#include "DS3232.h"
#include "I2C_Master.h"
#include "USART.h"

//extern volatile unsigned char I2C_buf[I2C_BUFFER_SIZE];		//Transceiver buffer
unsigned char RTC_buf[ TWI_BUFFER_SIZE ];    // Transceiver buffer

extern unsigned char TWI_buf[ TWI_BUFFER_SIZE ];

unsigned char I2C_seconde;
unsigned char I2C_minute;
unsigned char I2C_heure;
unsigned char I2C_jour;
unsigned char I2C_date;
unsigned char I2C_mois;
unsigned char I2C_annee;

char Day_Name[7][9]={"Dimanche","Lundi","Mardi","Mercredi","Jeudi","Vendredi","Samedi"};

// This function initializes the RTC clock with the provided
// values for the hour, date, and day. It populates the RTC_buf
// buffer with this data formatted for communication with the
// RTC clock via I2C.
void RTC_Clock_Set( unsigned char Sec,
unsigned char Min,
unsigned char Hrs,
unsigned char Day,
unsigned char Date,
unsigned char Month,
unsigned char Year)
{
	RTC_buf[0] = ((RTC_ADDRESS<<1)|0);		//Salve Address + W
	RTC_buf[1] = SECONDE;					//First Address of DS1307 register
	RTC_buf[2] = convertchartoBCD(SECONDE, Sec);
	RTC_buf[3] = convertchartoBCD(MINUTE, Min);
	RTC_buf[4] = convertchartoBCD(HEURE, Hrs);
	RTC_buf[5] = convertchartoBCD(JOUR, Day);
	RTC_buf[6] = convertchartoBCD(DATE, Date);
	RTC_buf[7] = convertchartoBCD(MOIS, Month);
	RTC_buf[8] = convertchartoBCD(ANNEE, Year);
	TWI_Start_Transceiver_With_Data(RTC_buf, 9);
	//Usart0_Tx_String("Clock Set");Usart0_Tx(0X0D);
}


// This function initializes the RTC alarm with the
// 	provided values for the exact time and day of the
// 	month. It populates the RTC_buf buffer with this data
// 	 formatted for communication with the RTC
// 	 clock via I2C.
void RTC_Alarm_Set(         unsigned char Sec,
unsigned char Min,
unsigned char Hrs,
unsigned char Date)
{
	// Configuration des registres pour autoriser l'alarme
	RTC_buf[0] = ((RTC_ADDRESS << 1) | 0); // Salve Address + W
	RTC_buf[1] = CONTROL;                  // Adresse du registre de contrôle
	// REGISTRE CONTROLE : Bit2 : Enables interrupt
	//					   Bit0 : Enables interrupts related to Alarm 1
	RTC_buf[2] = 0b00000101;
	// REGISTRE Status Register	: Bit 0 : Set to 0 to indicate that the alarm has not yet matched the time of the DS3232.
	RTC_buf[3] = 0b00000000;
	TWI_Start_Transceiver_With_Data(RTC_buf, 4);

	// Alarm Configuration
	RTC_buf[0] = ((RTC_ADDRESS << 1) | 0); // Salve Address + W
	RTC_buf[1] = SecAlarm1;                 // Adresse du registre d'alarme de seconde
	RTC_buf[2] = convertchartoBCD(SECONDE, Sec);
	RTC_buf[3] = convertchartoBCD(MINUTE, Min);
	RTC_buf[4] = convertchartoBCD(HEURE, Hrs);
	RTC_buf[5] = convertchartoBCD(DATE, Date);
	TWI_Start_Transceiver_With_Data(RTC_buf, 6);
	//Usart0_Tx_String("Alarm set");Usart0_Tx(0X0D);
}


// This function initializes the date of the RTC.
// It populates the RTC_buf buffer with this data
//  formatted for communication with the RTC
//  clock via I2C.
void RTC_Clock_Set_Date(	unsigned char Date,
unsigned char Month,
unsigned char Year)
{
	RTC_buf[0] = ((RTC_ADDRESS << 1) | 0); // Salve Address + W
	RTC_buf[1] = 4;                        // Adresse du registre de Date
	RTC_buf[2] = convertchartoBCD(DATE, Date);
	RTC_buf[3] = convertchartoBCD(MOIS, Month);
	RTC_buf[4] = convertchartoBCD(ANNEE, Year);
	TWI_Start_Transceiver_With_Data(RTC_buf, 5);
	//Usart0_Tx_String("Clock Set");Usart0_Tx(0X0D);
}

// This function initializes the time of the RTC.
//  It populates the RTC_buf buffer with this data
//   formatted for communication with the RTC
//   clock via I2C.
void RTC_Clock_Set_Heure(	unsigned char Sec,
unsigned char Min,
unsigned char Heure)
{
	RTC_buf[0] = ((RTC_ADDRESS << 1) | 0);  // Salve Address + W
	RTC_buf[1] = SECONDE;                   // Adresse du registre de SECONDE
	RTC_buf[2] = convertchartoBCD(SECONDE, Sec);
	RTC_buf[3] = convertchartoBCD(MINUTE, Min);
	RTC_buf[4] = convertchartoBCD(HEURE, Heure);
	TWI_Start_Transceiver_With_Data(RTC_buf, 5);
	//Usart0_Tx_String("Clock Set");Usart0_Tx(0X0D);
}


// This function reads the data from the RTC clock.
// It prepares the RTC_buf buffer to request information
// via I2C, then initiates the transmission to obtain the
// clock data (excluding alarms). After the transmission,
// it waits for the I2C transmission to complete
// before proceeding.
void RTC_Clock_Read_All(void)
{
	RTC_buf[0] = ((RTC_ADDRESS<<1)|0);		//Salve Address + W
	RTC_buf[1] = 0x00;
	TWI_Start_Transceiver_With_Data(RTC_buf, 2);
	RTC_buf[0] = ((RTC_ADDRESS<<1)|1);		//Salve Address + R
	TWI_Start_Transceiver_With_Data(RTC_buf, 8);
	while (TWI_Transceiver_Busy());       // Wait until TWI is ready for next transmission.
	// Convert the retrieved BCD values to char.
	convertBCDtoChar(SECONDE,TWI_buf[1]);
	convertBCDtoChar(MINUTE,TWI_buf[2]);
	convertBCDtoChar(HEURE,TWI_buf[3]);
	convertBCDtoChar(JOUR,TWI_buf[4]);
	convertBCDtoChar(DATE,TWI_buf[5]);
	convertBCDtoChar(MOIS,TWI_buf[6]);
	convertBCDtoChar(ANNEE,TWI_buf[7]);
}

// This function initializes the RTC alarm.
// The alarm sounds when the seconds provided
// in the function match the RTC seconds. It
// populates the RTC_buf buffer with this data
// formatted for communication with the RTC
// clock via I2C.
void RTC_Alarm_Set_Seconde( unsigned char Sec)
{
	// Configuration des registres pour autoriser l'alarme
	RTC_buf[0] = ((RTC_ADDRESS << 1) | 0);  // Salve Address + W
	RTC_buf[1] = CONTROL;                   // Adresse du registre de contrôle
	// REGISTRE CONTROLE : Bit2 : Enables interrupt
	//					   Bit0 : Enables interrupts related to Alarm 1
	RTC_buf[2] = 0b00000101;
	// REGISTRE Status Register	: Bit 0 : Set to 0 to indicate that the alarm has not yet matched the time of the DS3232.
	RTC_buf[3] = 0b00000000;
	TWI_Start_Transceiver_With_Data(RTC_buf, 4);
	// Configuration de l'alarme
	RTC_buf[0] = ((RTC_ADDRESS << 1) | 0);  // Salve Address + W
	RTC_buf[1] = SecAlarm1;                 // Adresse du registre d'alarme de seconde
	RTC_buf[2] = convertchartoBCD(SECONDE, Sec);
	//A1M4=1  A1M3=1 A1M2=1 A1M1=0
	RTC_buf[3] = 0b10000000;
	RTC_buf[4] = 0b10000000;
	RTC_buf[5] = 0b10000000;
	TWI_Start_Transceiver_With_Data(RTC_buf, 6);
	//Usart0_Tx_String("Alarm sec set");Usart0_Tx(0X0D);
}

// This function reads the requested byte from the RTC.
// It populates the RTC_buf buffer with the address of the
// desired byte, then listens to the RTC and fills the
// TWI Buffer.
unsigned char RTC_Clock_Read_Byte(unsigned char Adr_Pointer)
{
	RTC_buf[0] = ((RTC_ADDRESS<<1)|0);		//Salve Address + W
	RTC_buf[1] = Adr_Pointer;
	TWI_Start_Transceiver_With_Data(RTC_buf, 2);
	RTC_buf[0] = ((RTC_ADDRESS<<1)|1);		//Salve Address + R
	TWI_Start_Transceiver_With_Data(RTC_buf, 2);
	while ( TWI_Transceiver_Busy() );       // Wait until TWI is ready for next transmission.
	unsigned char Byte = convertBCDtoChar(SECONDE,TWI_buf[1]);
	// FOR DEBUG
	// Usart0_Tx(Byte);Usart0_Tx(0X0D);
	return Byte;
}

// Conversion of BCD to decimal.
unsigned char convertBCDtoChar(unsigned char AddressOfData, unsigned char Register_Data)
{
	unsigned char dizaine = 0;
	Usart0_Tx_String(dizaine);
	unsigned char unite = 0;
	unsigned char valeur = 0;
	// Selon le type de donnée,
	switch (AddressOfData)
	{
		case SECONDE :
		//Usart0_Tx_String("Secondes : ");
		// Isolation of the tens in BCD and shifting by 4 bits to convert it into a tens digit.
		dizaine = ((Register_Data & 0b01110000)>> 4);
		// Isolation des unités dans le BCD
		unite = Register_Data & 0b00001111;;
		// The return value is the tens digit multiplied by 10, to which the units digit is added.
		valeur = dizaine * 10 + unite;
		break;

		case MINUTE :
		// FOR DEBUG
		// Usart0_Tx_String("Minutes : ");
		dizaine = ((Register_Data & 0b01110000)>> 4);
		unite = Register_Data & 0b00001111;
		valeur = dizaine * 10 + unite;
		break;

		case HEURE :
		// FOR DEBUG
		// Usart0_Tx_String("Heures : ");
		// If in 12-hour or 24-hour mode, isolate one (12h) or two (24h) bits.
		if((Register_Data & 0b01000000) != 0)
		{
			dizaine = ((Register_Data & 0b00010000)>> 4);
		}
		else
		{
			dizaine = ((Register_Data & 0b00110000)>> 4);
		}
		unite = Register_Data & 0b00001111;
		valeur = dizaine * 10 + unite;
		break;

		case JOUR :
		// FOR DEBUG
		// Usart0_Tx_String("Jour : ");
		valeur = Register_Data;
		break;
		
		case DATE :
		// FOR DEBUG
		// Usart0_Tx_String("Date : ");
		dizaine = ((Register_Data & 0b00110000)>> 4);
		unite = Register_Data & 0b00001111;
		valeur = dizaine * 10 + unite;
		break;
		
		case MOIS :
		// FOR DEBUG
		// Usart0_Tx_String("Mois : ");
		dizaine = ((Register_Data & 0b00010000)>> 4);
		unite = Register_Data & 0b00001111;
		valeur = dizaine * 10 + unite;
		break;

		case ANNEE :
		// FOR DEBUG
		// Usart0_Tx_String("ANNEE : ");
		dizaine = ((Register_Data & 0b11110000)>> 4);
		unite = Register_Data & 0b00001111;
		valeur = dizaine * 10 + unite;
		break;
		default :
		valeur = Register_Data;
		break;
	}
	// FOR DEBUG
	// Usart0_Tx(dizaine+0b110000);Usart0_Tx(unite+0b110000);Usart0_Tx(0X0D);
	return valeur;
}

// Conversion of a number to BCD.
unsigned char convertchartoBCD(unsigned char addressofdata, unsigned char toconvert)
{
	unsigned char dizaine = 0;
	unsigned char unite = 0;
	unsigned char valeurretour = 0;
	
	// Obtain the units digit of the number using modulo.
	unite = toconvert % 10;
	// Obtain the tens digit of the number.
	dizaine = toconvert/10;

	switch (addressofdata)
	{
		// Shift by 4 bits for the tens digit and add the units to the last
		// 4 bits for seconds, hours, minutes, dates, months, years.
		case SECONDE :
		case HEURE :
		case MINUTE :
		case DATE :
		case MOIS :
		case ANNEE :
		valeurretour = ((dizaine << 4) | (unite));
		break;

		// In other cases, no modification to the data.
		default :
		valeurretour = toconvert;
		break;
	}
	return valeurretour;
}