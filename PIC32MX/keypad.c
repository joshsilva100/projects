#include <xc.h>
//#include “config.h”

void main()     {
    int i;
    unsigned char k = 32;
    //Configure general purpose pins as digital or analog, input or output
    	//Set up Keypad as digital
    	ANSELB = 0;
    	ANSELA = 0;
    	//Set up LEDs as output
    	TRISAbits.TRISA0 = 0;
    	TRISAbits.TRISA1 = 0;
    	TRISAbits.TRISA2 = 0;
    	TRISAbits.TRISA3 = 0;
    	//Set up Keypads
    	TRISBbits.TRISB0 = 0;
    	TRISBbits.TRISB1 = 0;
    	TRISBbits.TRISB2 = 0;
    	TRISBbits.TRISB3 = 0;
    	TRISBbits.TRISB14 = 1;
    	TRISBbits.TRISB15 = 1;
    	TRISBbits.TRISB12 = 1;
    	TRISBbits.TRISB13 = 1;

    unsigned char mask[16] =
   	 { //Col 0 	1 	2 	3
  	   0xEE, 0xDE, 0xBE, 0x7E,  // Row 0
  	   0XED, 0xDD, 0xBD, 0x7D,  // Row 1
  	   0xEB, 0xDB, 0xBB, 0x7B,  // Row 2
  	   0xE7, 0xD7, 0xB7, 0x77   // Row 3
   	 };

    unsigned char key[16] =
   	 {15, 4, 3, 10,
    	    1, 5, 6, 11,
    	    2, 8, 9, 12,
    	    7, 0, 14, 13
   	 };
   	 //15 = *, 14 = #

    
    while(1)    {
   	 for(i = 0; i < 16; i++)    {
   		 //Output (mask[i] & 0x0F) to ROWS
        	LATB = mask[i];
        	if (((PORTB & 0xF000)>>8) == (mask[i] & 0xF0))  {
            	k = key[i];
            	break;
        	}
   	 }
   	 if (k != 32) {
   		 //Output k on LEDs
        		LATA = k;
   		 k = 32;
   	 }
   	 else    {
   		 //Output 0 on LEDs
        		LATA = 0;
   	 }
    }    

}
