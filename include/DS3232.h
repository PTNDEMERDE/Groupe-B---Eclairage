#define RTC_ADDRESS 0b1101000		//Adresse du RTC_DS3232

// Adresses de données RTC3232
enum {SECONDE,MINUTE,HEURE,JOUR,DATE,MOIS,ANNEE,
	SecAlarm1,MinAlarm1, HourAlarm1, DateAlarm1,
	MinAlarm2, HourAlarm2, DateAlarm2,
CONTROL, ControlStatus, AagingOffset, MSBTemps,LSBTemp};



// Initialisation de la clock lorsqu'on
void RTC_Clock_Set(        unsigned char Sec,
unsigned char Min,
unsigned char Hrs,
unsigned char Day,
unsigned char Date,
unsigned char Month,
unsigned char Year);

// This function sets the time for the next alarm.
// Example: RTC_Alarm_Set(s, min, h, DD);
// The DS3232 will generate a falling edge on the Wake-up pin when
// the day, hour, minute, and second entered in the function match
// with the day, hour, minute, and second of the DS3232.
void RTC_Alarm_Set(         unsigned char Sec,
unsigned char Min,
unsigned char Hrs,
unsigned char Date);

// This function allows setting the exact date of the DS3232.
// ex : RTC_Clock_Set_Date(JJ,MM,AA);
void RTC_Clock_Set_Date(	unsigned char Date,
unsigned char Month,
unsigned char Year);

// This function allows setting the exact time of the DS3232.
// ex : RTC_Clock_Set_Heure(s,min,h);
void RTC_Clock_Set_Heure(	unsigned char Sec,
unsigned char Min,
unsigned char Heure);

void RTC_Alarm_Set_Seconde( unsigned char Sec);

// This function returns the value of all registers of the clock.
void RTC_Clock_Read_All(void);

// This function will read the register at the address Adr_Pointer.
unsigned char RTC_Clock_Read_Byte(unsigned char Adr_Pointer);

unsigned char convertBCDtoChar(unsigned char AddressOfData, unsigned char Register_Data);

unsigned char convertchartoBCD(unsigned char addressofdata, unsigned char toconvert);