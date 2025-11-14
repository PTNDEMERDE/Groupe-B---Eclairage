#ifndef EXPANDER_MCP23017_H_
#define EXPANDER_MCP23017_H_

#include <stdint.h>
#include <string.h>

// GPIO Ports :

#define LOW 0
#define HIGH 1

// Port A :
#define EXP_GPIOA0 0
#define EXP_GPIOA1 1
#define EXP_GPIOA2 2
#define EXP_GPIOA3 3
#define EXP_GPIOA4 4
#define EXP_GPIOA5 5
#define EXP_GPIOA6 6
#define EXP_GPIOA7 7

// Port B : 
#define EXP_GPIOB0 0
#define EXP_GPIOB1 1
#define EXP_GPIOB2 2
#define EXP_GPIOB3 3
#define EXP_GPIOB4 4
#define EXP_GPIOB5 5
#define EXP_GPIOB6 6
#define EXP_GPIOB7 7

// Control Byte with physical address of the expander, A0 -> GND, A1 -> GND, A2 -> GND :

#define EXPANDER_W_CTRL_BYTE 0b01000000 // 0 | 1 | 0 | 0 | A2 | A1 | A0 | R/W, 0 is a write instruction.
#define EXPANDER_R_CTRL_BYTE 0b01000001 // 0 | 1 | 0 | 0 | A2 | A1 | A0 | R/W, 1 is a read instruction.

// Register Addresses if IOCON.BANK = 0 :

// 8 bits R/W GPIO Port Direction Register, write a 1 for an input and 0 for an output :
// IO7 | IO6 | IO5 | IO4 | IO3 | IO2 | IO1 | IO0
// All bits set to 1 by default.
#define IODIRA 0x00
#define IODIRB 0x01
// 8 bits R/W GPIO Port Polarity Register, write a 1 for reflecting the opposite logic state of the input pin on the GPIO register bit and 0 for the same logic state :
// IP7 | IP6 | IP5 | IP4 | IP3 | IP2 | IP1 |  IP0
// All bits set to 0 by default.
#define IPOLA 0x02
#define IPOLB 0x03
// 8 bits R/W GPIO Interrupt-On-Change Pins, write a 1 for enabling the interrupt-on-change event on the GPIO input pin, the GPIO pin should be an input, write a 0 for disabling interrupt :
// GPINT7 | GPINT6 | GPINT5 | GPINT4 | GPINT3 | GPINT2 | GPINT1 | GPINT0
// All bits set to 0 by default.
#define GPINTENA 0x04
#define GPINTENB 0x05
// 8 bits R/W Default Value Register, used to set a default comparison value to trigger the interrupt :
// DEF7 | DEF6 | DEF5 | DEF4 | DEF3 | DEF2 | DEF1 | DEF0
// All bits set to 0 by default.
#define DEFVALA 0x06
#define DEFVALB 0x07
// 8 bits R/W Interrupt-On-Change Control Register, if a bit is set to 1, the pin value is compared against the associated bit in the DEFVAL register :
// IOC7 | IOC6  | IOC5 | IOC4 | IOC3 | IOC2 | IOC1 | IOC0
// All bits set to 0 by default.
#define INTCONA 0x08
#define INTCONB 0x09
// 8 bits R/W Expander Configuration Register :
// BANK | MIRROR | SEQOP | DISSLW | HAEN | ODR | INTPOL | U-0
// All bits set to 0 by default.
// BANK : If 0, the register are in the same bank (addresses are sequential), else the registers associated with each port are separated into different banks.
// MIRROR : If 0, the INT pins are not inter-connected, INTA is connected to PORTA and INTB to PORTB, else the INT pins are inter-connected (both trigger at the same time).
// SEQOP : If 0, sequential operation enabled, address pointer increments, else sequential operation disabled, address pointer doesn't increments.
// DISSLW : If 0, slew rate control for SDA output enable, else disabled.
// HAEN : Not used with I2C.
// ODR : If 0, the INT pins follow the active driver output (INTPOL bit sets the polarity), else INT pins are open-drain (overrides the INTPOL bit).
// INTPOL : IF 0, set the polarity of the INT output pins active low, else it's active-high.
// U-0 : Unimplemented bit, always read as 0 not writable.
#define IOCON1 0x0A
#define IOCON2 0x0B
// 8 bits R/W GPIO Pull-Up resistor register.
// PU7 | PU6 | PU5 | PU4 | PU3 | PU2 | PU1 | PU0
// All bits set to 0 by default.
// If 0, pull-up disabled, else, pull-up enabled.
#define GPPUA 0x0C
#define GPPUB 0x0D
// 8 bits R Interrupt Flag Register 
// INT7 | INT6 | INT5 | INT4 | INT3 | INT2 | INT1 | INT0
// All bits set to 0 by default.
// If 0, interrupt not pending, else pin caused interrupt.
#define INTFA 0x0E
#define INTFB 0x0F
// 8 bits R Interrupt Captured Value For Port Register
// ICP7 | ICP6 | ICP5 | ICP4 | ICP3 | ICP2 | ICP1 | ICP0
// The register captures the GPIO port value at the time the interrupt occurred. It is updated only when an interrupt occurs. The register remains unchanged until the interrupt is cleared via a read of INTCAP or GPIO.
#define INTCAPA 0x10
#define INTCAPB 0x11
// 8 bits R/W GPIO register.
// GP7 | GP6 | GP5 | GP4 | GP3 | GP2 | GP1 | GP0
// Read GPIOn register reads the actual value on the port. Writing to this register modifies the Output Latch (OLAT) register.
// All bits set to 0 by default.
#define GPIOA 0x12
#define GPIOB 0x13
// 8 bits R/W Output Latch Register 
// OL7 | OL6 | OL5 | OL4 | OL3 | OL2 | OL1 | OL0
// Read OLATn register only reads the latches not the actual value on the port.
// All bits set to 0 by default.
#define	OLATA 0x14
#define OLATB 0x15

/*
// Register Addresses if IOCON.BANK = 1 :

 #define IODIRA 0x00
 #define IODIRB 0x10

 #define IPOLA 0x01
 #define IPOLB 0x11

 #define GPINTENA 0x02
 #define GPINTENB 0x12

 #define DEFVALA 0x03
 #define DEFVALB 0x13

 #define INTCONA 0x04
 #define INTCONB 0x14

 #define IOCON1 0x05
 #define IOCON2 0x15

 #define GPPUA 0x06
 #define GPPUB 0x16

 #define INTFA 0x07
 #define INTFB 0x17

 #define INTCAPA 0x08
 #define INTCAPB 0x18

 #define GPIOA 0x09
 #define GPIOB 0x19

 #define	OLATA 0x0A
 #define OLATB 0x1A
*/

void Expander_Init();
void Expander_Write(unsigned char registerAddress,unsigned char dataToWrite);
unsigned char Expander_Read(unsigned char registerAddress);
void Expander_Gpio_Ctrl(unsigned char GPIOPort, unsigned char GPIOPin, unsigned char GPIOPortDirection);
#endif /* EXPANDER_MCP23017_H_ */