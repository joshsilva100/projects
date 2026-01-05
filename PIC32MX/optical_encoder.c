#include <xc.h>
#include <sys/attribs.h>

int count = 15;

// ISR for first interrupt (INT0)
void __ISR(3) example(void){
	// Check the values on both inputs and modify count accordingly
	(PORTBbits.RB7 == PORTBbits.RB9)?count++:count--;
	INTCONbits.INT0EP ^= 1; // Adjust interrupt edge polarity
	IFS0bits.INT0IF = 0; // Clear interrupt flag
}

// ISR for second interrupt (INT1)
void __ISR(7) example1(void){
	// Check the values on both inputs and modify count accordingly
	(PORTBbits.RB9 == PORTBbits.RB7)?count++:count--;
	INTCONbits.INT1EP ^= 1; // Adjust interrupt edge polarity
	IFS0bits.INT1IF = 0; // Clear interrupt flag
}

main(){
    
	CFGCONbits.JTAGEN = 0; //Disable to use B7-B9
    
	//Set outputs (LEDS)
TRISBbits.TRISB0 = 0;
TRISBbits.TRISB1 = 0;
TRISBbits.TRISB2 = 0;
TRISBbits.TRISB3 = 0;
    
    	//Set input (interrupts)
TRISBbits.TRISB7 = 1;
	TRISBbits.TRISB9 = 1;
    
   	 __builtin_enable_interrupts(); //Built in function in library
     	 
    	//Enable Multi Vector Mode
    	INTCONbits.MVEC = 1;
    
	//Enable Interrupt 0
IFS0bits.INT0IF = 0;
IPC0bits.INT0IP = 1;
INTCONbits.INT0EP = 1; //Rising Edge
IEC0bits.INT0IE = 0;
IEC0bits.INT0IE = 1;
    
	//Enable Interrupt 1
	INT1R = 4; //Remap External Interrupt to RB9 (5V Tolerant)
IFS0bits.INT1IF = 0;
IPC1bits.INT1IP = 1; 
INTCONbits.INT1EP = 1; //Rising Edge
IEC0bits.INT1IE = 0;
IEC0bits.INT1IE = 1;
    
	IFS0 = 0; //Initial Clear interrupt flags to be safe

	//Count only changes in interrupts
	while(1)
	{
    		// Restrict count to [0,15]
    		// Count could be increasing or decreasing (Handled by interrupts)
    		LATB = count; // Write count to output pins
   	 
    	if(count > 15)
    	{
        		count = 0;
    	}
   	 
    	else if(count < 0)
    	{
        		count = 15;
    	}
	}
}