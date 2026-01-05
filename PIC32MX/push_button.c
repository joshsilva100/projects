#include <xc.h>

main ()
{
    
	//Set inputs
	TRISBbits.TRISB14 = 1; //Set B5 as input
	TRISBbits.TRISB15 = 1; //Set B6 as input
	TRISBbits.TRISB10 = 1; //Set B7 as input

	//Set Outputs
	TRISAbits.TRISA1 = 0; //Set A2 as output
	TRISBbits.TRISB0 = 0; //Set B0 as output
	TRISBbits.TRISB1 = 0; //Set B1 as output

	//Set Ports to Digital
	ANSELA = 0;
	ANSELB = 0;

	//Read from DIO and Write to Latch
	while(1)
	{
    	LATBbits.LATB0 = PORTBbits.RB15;
    	LATBbits.LATB1 = PORTBbits.RB14;

    	//Button Pressed, send a HIGH
    	if (PORTBbits.RB10 == 1)
    	{
        		LATAbits.LATA1 = 1;
    	}
   	 
    	//Button Not Pressed, send a LOW
    	else
    	{
        		LATAbits.LATA1 = 0;
    	}

	}

}
