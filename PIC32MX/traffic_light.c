#include <xc.h>
#include <sys/attribs.h>

#define num_timer 10
#define num_cycle 8

void delay(){
    int i, j;
    for(i = 0; i < 1000; i++)
    for(j = 0; j < 1000; j++);
}

//Walking has started, turn all intersections to red
void walk(){
	LATBbits.LATB15 = 1; //Turn on red
	LATBbits.LATB14 = 0;
	LATBbits.LATB13 = 0;
	LATAbits.LATA0 = 0;
	LATBbits.LATB12 = 1; //Turn on red
	LATBbits.LATB11 = 0;
	LATBbits.LATB10 = 0;  
	LATAbits.LATA1 = 0;
}

//Adjust 7 Segment to count down from 9 to zero
void walk_timer(int i) {
    
	if (i == 0)
	{
    	//Count 9
    	LATBbits.LATB0 = 0;
    	LATBbits.LATB1 = 0;
    	LATBbits.LATB2 = 0;
    	LATBbits.LATB3 = 1;
    	LATBbits.LATB8 = 1;
    	LATBbits.LATB5 = 0;
    	LATBbits.LATB6 = 0;
	}
    
	if (i == 1)
	{
    	//Count 8
    	LATBbits.LATB0 = 0;
    	LATBbits.LATB1 = 0;
    	LATBbits.LATB2 = 0;
    	LATBbits.LATB3 = 0;
    	LATBbits.LATB8 = 0;
    	LATBbits.LATB5 = 0;
    	LATBbits.LATB6 = 0;
	}
    
	if (i == 2)
	{
    	//Count 7
    	LATBbits.LATB0 = 0;
    	LATBbits.LATB1 = 0;
    	LATBbits.LATB2 = 0;
    	LATBbits.LATB3 = 1;
    	LATBbits.LATB8 = 1;
    	LATBbits.LATB5 = 1;
    	LATBbits.LATB6 = 1;
	}
    
	if (i == 3)
	{
    	//Count 6
    	LATBbits.LATB0 = 1;
    	LATBbits.LATB1 = 1;
    	LATBbits.LATB2 = 0;
    	LATBbits.LATB3 = 0;
    	LATBbits.LATB8 = 0;
    	LATBbits.LATB5 = 0;
    	LATBbits.LATB6 = 0;
	}
    
	if (i == 4)
	{
    	//Count 5
    	LATBbits.LATB0 = 0;
    	LATBbits.LATB1 = 1;
    	LATBbits.LATB2 = 0;
    	LATBbits.LATB3 = 0;
    	LATBbits.LATB8 = 1;
    	LATBbits.LATB5 = 0;
    	LATBbits.LATB6 = 0;
	}
	
if (i == 5)
	{
    	//Count 4
    	LATBbits.LATB0 = 1;
    	LATBbits.LATB1 = 0;
    	LATBbits.LATB2 = 0;
    	LATBbits.LATB3 = 1;
    	LATBbits.LATB8 = 1;
    	LATBbits.LATB5 = 0;
    	LATBbits.LATB6 = 0;
	} 
	
	if (i == 6)
	{
    	//Count 3
    	LATBbits.LATB0 = 0;
    	LATBbits.LATB1 = 0;
    	LATBbits.LATB2 = 0;
    	LATBbits.LATB3 = 0;
    	LATBbits.LATB8 = 1;
    	LATBbits.LATB5 = 1;
    	LATBbits.LATB6 = 0;
	}
    
	if (i == 7)
	{
    	//Count 2
    	LATBbits.LATB0 = 0;
    	LATBbits.LATB1 = 0;
    	LATBbits.LATB2 = 1;
    	LATBbits.LATB3 = 0;
    	LATBbits.LATB8 = 0;
    	LATBbits.LATB5 = 1;
    	LATBbits.LATB6 = 0;
	}
    
	if (i == 8)
	{
    	//Count 1
    	LATBbits.LATB0 = 1;
    	LATBbits.LATB1 = 0;
    	LATBbits.LATB2 = 0;
    	LATBbits.LATB3 = 1;
    	LATBbits.LATB8 = 1;
    	LATBbits.LATB5 = 1;
    	LATBbits.LATB6 = 1;
	}
    
	if (i == 9)
	{
    	//Count 0
    	LATBbits.LATB0 = 0;
    	LATBbits.LATB1 = 0;
    	LATBbits.LATB2 = 0;
    	LATBbits.LATB3 = 0;
    	LATBbits.LATB8 = 0;
    	LATBbits.LATB5 = 0;
    	LATBbits.LATB6 = 1;
	}
}

/* Run through all 8 traffic cycles 
 * <r,r,r,r,b,g,y,r>  
 * <b,g,y,r,r,r,r,r> 
 * red for stop, yellow to prepare to stop, green for go, blue for left turn signal
 */
void traffic(int i){
    
	if (i == 0)
{
   	//1st Traffic Set
   	LATBbits.LATB15 = 1; //Turn on red
   	LATBbits.LATB14 = 0;
   	LATBbits.LATB13 = 0;
    	LATAbits.LATA0 = 0;
   	//2nd Traffic Set
   	LATBbits.LATB12 = 0;
   	LATBbits.LATB11 = 0;
   	LATBbits.LATB10 = 0;
    	LATAbits.LATA1 = 1; //Turn on blue
}
    
if (i == 1)
{
   	//1st Traffic Set
   	LATBbits.LATB15 = 1; //Turn on red
   	LATBbits.LATB14 = 0;
   	LATBbits.LATB13 = 0;
    	LATAbits.LATA0 = 0;
   	//2nd Traffic Set
   	LATBbits.LATB12 = 0;
   	LATBbits.LATB11 = 0;
   	LATBbits.LATB10 = 1; //Turn on green
    	LATAbits.LATA1 = 0;
}
    
    if (i == 2)
    {
   	//1st Traffic Set
   	LATBbits.LATB15 = 1; //Turn on red
   	LATBbits.LATB14 = 0;
   	LATBbits.LATB13 = 0;
    	LATAbits.LATA0 = 0;
   	//2nd Traffic Set
   	LATBbits.LATB12 = 0;
   	LATBbits.LATB11 = 1; //Turn on yellow
   	LATBbits.LATB10 = 0;
    	LATAbits.LATA1 = 0;
    }
    
    if (i == 3)
    {
   	//1st Traffic Set
   	LATBbits.LATB15 = 1; //Turn on red
   	LATBbits.LATB14 = 0;
   	LATBbits.LATB13 = 0;
    	LATAbits.LATA0 = 0;
   	//2nd Traffic Set
   	LATBbits.LATB12 = 1; //Turn on red
   	LATBbits.LATB11 = 0;
   	LATBbits.LATB10 = 0;
    	LATAbits.LATA1 = 0;
    }
    
    if (i == 4)
    {
   	 //1st Traffic Set
   	 LATBbits.LATB15 = 0;
   	 LATBbits.LATB14 = 0;
   	 LATBbits.LATB13 = 0;
    	LATAbits.LATA0 = 1; //Turn on blue
   	//2nd Traffic Set
   	LATBbits.LATB12 = 1; //Turn on red
   	LATBbits.LATB11 = 0;
   	LATBbits.LATB10 = 0;
    	LATAbits.LATA1 = 0;
    }
    
    if (i == 5)
    {
   	//1st Traffic Set
   	LATBbits.LATB15 = 0;
   	LATBbits.LATB14 = 0;
   	LATBbits.LATB13 = 1; //Turn on green
    	LATAbits.LATA0 = 0;
   	//2nd Traffic Set
   	LATBbits.LATB12 = 1; //Turn on red
   	LATBbits.LATB11 = 0;
   	LATBbits.LATB10 = 0;
    	LATAbits.LATA1 = 0;
    }

    if (i == 6)
    {
   	//1st Traffic Set
   	LATBbits.LATB15 = 0;
   	LATBbits.LATB14 = 1; //Turn on yellow
   	LATBbits.LATB13 = 0;
    	LATAbits.LATA0 = 0;
   	//2nd Traffic Set
   	LATBbits.LATB12 = 1; //Turn on red
   	LATBbits.LATB11 = 0;
   	LATBbits.LATB10 = 0;
    	LATAbits.LATA1 = 0;
    }

if (i == 7)
{
//1st Traffic Set
   	LATBbits.LATB15 = 1; //Turn on red
   	LATBbits.LATB14 = 0;
   	LATBbits.LATB13 = 0;
    	LATAbits.LATA0 = 0;
   	//2nd Traffic Set
   	LATBbits.LATB12 = 1; //Turn on red
   	LATBbits.LATB11 = 0;
   	LATBbits.LATB10 = 0;
    	LATAbits.LATA1 = 0;
}

}

//ISR for External Interrupt 0; button to allow pedestrians to walk
void __ISR(3) button(void) {
	
//Set intersections to red to allow pedestrians to walk
walk();

	//Call walk_timer to write outputs to 7 segment display (From 9 to 0)
for (int i = 0; i < num_timer; i++)
    	{
        	walk_timer(i);
        	delay();
    	}

IFS0bits.INT0IF = 0; // Clear interrupt flag
}

void main() {
	
	//Set up pull down resistor for button
	CNPDBbits.CNPDB7 = 1;

	//Set outputs for Port A
    	TRISAbits.TRISA0 = 0;
	TRISAbits.TRISA1 = 0;

    	//Set up inputs & outputs for Port B
    	TRISB = 0x0080; //B1 through B4 & B10 through B15 are outputs

	__builtin_enable_interrupts(); //Built in function to handle interrupts in library
	INTCONbits.MVEC = 1; //Enable Multi Vector Mode
	CFGCONbits.JTAGEN = 0; //Turn off to prevent any interference 

	//Enable Interrupt 0
	IFS0bits.INT0IF = 0;
	IPC0bits.INT0IP = 1;
	INTCONbits.INT0EP = 1; //Rising Edge
	IEC0bits.INT0IE = 0;
	IEC0bits.INT0IE = 1;

	//Run through traffic cycles for 4-way intersection
while(1)
	{	 
    	//Call traffic function to run through each traffic cycle
for (int i = 0; i < num_cycle; i++)
    	{
        	traffic(i);
        	delay();
    	}

	}

}
