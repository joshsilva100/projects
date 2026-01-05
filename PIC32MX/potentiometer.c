#include <xc.h>

main ()
{
	//Set inputs
	TRISBbits.TRISB0 = 1;
	TRISBbits.TRISB2 = 1;
	TRISBbits.TRISB3 = 1;

	//Set Outputs
	TRISBbits.TRISB12 = 0;  
	TRISBbits.TRISB13 = 0;
	TRISBbits.TRISB14 = 0;
    
	//Set ports to Analog
	ANSELAbits.ANSA0 = 1;
	ANSELAbits.ANSA1 = 1;
	ANSELBbits.ANSB15 = 1;
	ANSELBbits.ANSB0 = 1;
	ANSELBbits.ANSB2 = 1;
	ANSELBbits.ANSB3 = 1;
    
	//Setting up CMXCON Register
	CM1CONbits.ON = 1;
	CM2CONbits.ON = 1;
	CM3CONbits.ON = 1;
    
	CM1CONbits.CREF = 0;
	CM2CONbits.CREF = 1;
	CM3CONbits.CREF = 0;
    
	CM1CONbits.CCH = 0x3;
	CM2CONbits.CCH = 0x2;
	CM3CONbits.CCH = 0x2;
    
	CM1CONbits.CPOL = 0;
	CM2CONbits.CPOL = 1;
	CM3CONbits.CPOL = 1;
    
	//Setting up CVRCON Register
	CVRCONbits.ON = 1;
	CVRCONbits.CVRSS = 1;
	CVRCONbits.CVRR = 0;
	CVRCONbits.CVR = 0xf;
    
	while (1)
	{
    	//Set LEDS to outputs of comparators
    	LATBbits.LATB12 = CMSTATbits.C1OUT;
    	LATBbits.LATB13 = CMSTATbits.C2OUT;
    	LATBbits.LATB14 = CMSTATbits.C3OUT;
	}
}
