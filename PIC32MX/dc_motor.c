#include <xc.h>
#include <sys/attribs.h>

int dty;
void delay(){
	int i, j;
	for(i = 0; i < 200; i++)
	for(j = 0; j < 200; j++);
}

/*
void __ISR(3) buttonInterrupt(void){
	dty += 25;
	if(dty > 100){
    	dty = 0;
   	 
	OC4CONbits.ON ^= 1; // Toggle which OC is on
	OC2CONbits.ON ^= 1;
	}
	OC4RS = dty;
	OC2RS = dty;
	delay();
	IFS0bits.INT0IF = 0;
}
 * */

void main() {
	// Add MVEC and builtin enable interrupt
	//__builtin_enable_interrupts(); //Built in function in library
	//INTCONbits.MVEC = 1; //Multivector Mode
	RPB1R = 6; // Map to OC2
	OC2CONbits.ON = 0; // Ensure OC is off during setup
	OC2CONbits.OC32 = 0; // Don't need 32-bit mode
	OC2CONbits.OCTSEL = 0; // Select timer (Timer 2 in this case)
	OC2CONbits.OCM = 6; // PWM mode
	dty = 0;
	OC2R = 0;
	OC2RS = 0;
	T2CONbits.ON = 0;
	PR2 = 100;
	T2CONbits.ON = 1;
	OC2CONbits.ON = 1; // Enable OC when finished
	while(1){
    	dty += 25;
    	if(dty > 100)
    	dty = 0;
    	OC2RS = dty; // Write to OC register (duty cycle will be OC2RS/PR2)
    	delay();
	}
}
