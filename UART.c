// UART.c
// Runs on LM4F120/TM4C123
// Provide functions that setup and interact with UART
// Last Modified: 5/5/2016

#include <stdint.h>
#include "tm4c123gh6pm.h"
void EnableInterrupts(void);

// UART initialization function 
// Input: none
// Output: none
void UART_Init(void){ 
	SYSCTL_RCGCUART_R |= 0x0002; //activate UART1
	SYSCTL_RCGCGPIO_R |= 0x0004; // activate port C
	UART1_CTL_R &= ~0x0001; //disable UART
	UART1_IBRD_R = 50; //IBRD = int(80000000/(16*100000)) = 50
	UART1_FBRD_R = 0;  //FBRD = 0 (no digits after decimal place)
	UART1_LCRH_R = 0x0070; //8-bit word length, enable FIFO
	UART1_CTL_R |= 0x0301; // enable RXE,TXE, and UART
	
	UART1_IM_R |= 0x10;			//Arm RXRIS
	UART1_IFLS_R &= ~38;
	UART1_IFLS_R |= 0x10;
	NVIC_PRI1_R = (NVIC_PRI1_R&0xFF00FFFF)|0x00200000;				//bits 21-23 priority 1
	NVIC_EN0_R = NVIC_EN0_INT6;							// enable interrupt 6 in NVIC
	
	
	GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&=0xFF00FFFF)+0x00220000;//(GPIO_PORTC_PCTL_R&0xFFFFFF00)+0x00000011; // UART
	GPIO_PORTC_AMSEL_R &= ~0x30; 							//disable analog function on PC4,5
	GPIO_PORTC_AFSEL_R |= 0x30; 							//enable alt function on PC4,5
	GPIO_PORTC_DEN_R |= 0x30;							// enable digital I/O on PC4,5
}



//------------UART_InChar------------
// Wait for new input,
// then return ASCII code
// Input: none
// Output: char read from UART
// *** Need not be busy-wait if you know when to call
char UART_InChar(void){  

  while((UART1_FR_R&0x0010)!=0);//wait until RXFE is 0
	return((char)(UART1_DR_R&0xFF));
}

//------------UART_OutChar------------
// Wait for new input,
// then return ASCII code
// Input: none
// Output: char read from UART
void UART_OutChar(char data){  

	while((UART1_FR_R&0x0020)!=0);							//wait until TXFF is 0
	UART1_DR_R = data;
}
