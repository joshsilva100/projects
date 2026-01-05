#include <xc.h>
#include <sys/attribs.h>

void __ISR(3) example(void) {
	LATB = 15;
	delay();
	IFS0bits.INT0IF = 0;
}

void delay(){
	int i, j;
	for(i = 0; i < 800; i++)
    	for(j = 0; j < 800; j++);
}


main () {
	int count = 15;
	//Set outputs
	TRISBbits.TRISB0 = 0;
	TRISBbits.TRISB1 = 0;
	TRISBbits.TRISB2 = 0;
	TRISBbits.TRISB3 = 0;
    
	//Set input
	TRISBbits.TRISB7 = 1;
    
	__builtin_enable_interrupts() //Built in function in library
	//Enable Multi Vector Mode
	INTCONbits.MVEC = 1;
	//Enable Interrupts
	IFS0bits.INT0IF = 0;
	IPC0bits.INT0IP = 1;
	INTCONbits.INT0EP = 1; //Rising Edge
 	IEC0bits.INT0IE = 0;
	IEC0bits.INT0IE = 1;
    
	while(1){
    	LATB = count;    // Write count to output pins
    	count--;
    	if(count < 0)
        	count = 15;
    	delay();
	}

}