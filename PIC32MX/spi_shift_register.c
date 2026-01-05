#include <xc.h>
#include <sys/attribs.h>

#define numChars 12

char spiChars[18] = {0, 1, 4, 8, 16, 32, 64, 128, 255, 254, 253, 251, 247, 239, 223, 191, 127};
int i = 0;

void delay(){
	int i, j;
	for(i = 0; i < 200; i++)
	for(j = 0; j < 200; j++);
}

//ISR for External Interrupt 0; button
void __ISR(3) button(void) {
	//LATBbits.LATB0 = 1;
	for (int n = 0; n < 150; n++) {
    	for (int j = 0; j < 312; j++) {
       	 
    	}
	}
	//LATBbits.LATB0 = 0;
	SPI1BUF = spiChars[i];
	i++;
	if (i > numChars - 1) {
    	i = 0;
	}
	IFS0bits.INT0IF = 0; // Clear interrupt flag
}

void main() {
 
	LATBbits.LATB0 = 1; delay(250); LATBbits.LATB0 = 0; //Pin B0 used
	LATB = 0;
    
//	__builtin_enable_interrupts(); //Built in function in library
    
	RPA0R = 3; //SS1 pin 2
	RPA1R = 3; //SDO1 pin 3
    
    //Enable Multi Vector Mode
//    INTCONbits.MVEC = 1;
//	CFGCONbits.JTAGEN = 0;
    
	//Enable Interrupt 0
//    IFS0bits.INT0IF = 0;
    IPC0bits.INT0IP = 1;
    INTCONbits.INT0EP = 1; //Rising Edge
	 IEC0bits.INT0IE = 0;
    IEC0bits.INT0IE = 1;
    
	//SPI Setup
    
	//Disable SPI Interrupts
//	IEC1bits.SPI1EIE = 0; //Disable SPI Fault
	IEC1bits.SPI1RXIE = 0; //Disable SPI Receive Done
	IEC1bits.SPI1TXIE = 0; // Disable SPI Transfer Done
    
	SPI1CONbits.ON = 0; // Disable SPI module during setup
    
	SPI1BUF = 0; //Clear the receive buffer
    
	SPI1CONbits.ENHBUF = 0; // Don’t want enhanced buffer mode
    
	//Clear SPI flags
//	IFS1bits.SPI1EIF = 0;
	IFS1bits.SPI1RXIF = 0;
	IFS1bits.SPI1TXIF = 0;
    
	//Set SPI Priorities
	IPC7bits.SPI1IP = 1;
    
	//Enable SPI Interrupts
//	IEC1bits.SPI1EIE = 1; //Enable SPI Fault
	IEC1bits.SPI1RXIE = 1; //Enable SPI Receive Done
	IEC1bits.SPI1TXIE = 1; // Enable SPI Transfer Done
    
	SPI1BRG = 1000; // Should result in baud rate slow enough to observe
    
	SPI1STATbits.SPIROV = 0; // In SPI1STAT, clear the SPIROV bit  
    
	//Settings for SPICON
//	SPI1CONbits.MODE16 = 0; //Set to 8 bits 
SPI1CONbits.MCLKSEL = 0; //Use PB clock
	SPI1CONbits.CKP = 0; // Clock Polarity, idle at high lvl & active at low lvl
	SPI1CONbits.CKE = 1; // SPI Clock Edge, transition from active clock to idle
	SPI1CONbits.SMP = 1; //Input data sampled at end of data output time
	SPI1CONbits.DISSDO = 0; //SDOX pin is controlled by the module
	SPI1CONbits.MSTEN = 1; //Write the desired settings to SPIxCON register
	SPI1CONbits.MSSEN = 1; //Slave Select SPI support enabled
	SPI1CONbits.ON = 1; // Enable SPI module after setup is complete
    
	while(1)
	{
//    	for (int n = 0; n < 150; n++) {
//        	for (int j = 0; j < 312; j++) {
//
//        	}
//    	}
////    	LATBbits.LATB0 = 0;
//    	SPI1BUF = spiChars[i];
//    	i++;
//    	if (i > numChars - 1) {
//        	i = 0;
//    	}
	}
    
}