// UART.c
// Runs on LM4F120 or TM4C123
// Last Modified: 5/5/2016

// Analog Input connected to PE2=ADC1
// displays on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats
// UART1 on PC4-5
// * Start with where you left off in Lab8. 
// * Get Lab8 code working in this project.
// * Understand what parts of your main have to move into the UART1_Handler ISR
// * Rewrite the SysTickHandler
// * Implement the s/w FiFo on the receiver end 

#include <stdint.h>

#include "ST7735.h"
#include "TExaS.h"
#include "ADC.h"
#include "print.h"
#include "tm4c123gh6pm.h"
#include "UART.h"
#include "FiFo.h"
int main_fifo(void);
#define fail 0
#define success 1
#define size 6

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on Nokia
// main3 adds your convert function, position data is no Nokia

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
uint32_t Data;      // 12-bit ADC
uint32_t Position;  // 32-bit fixed-point 0.001 cm
int32_t TxCounter = 0;
int32_t RxCounter = 0;
uint32_t Mailbox,flag;
char GetData[10];

void SysTick_Init(){
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = 2000000;																		// waits 25 ms for each interrupt
	NVIC_ST_CURRENT_R = 0;
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000;		//priority 2
	NVIC_ST_CTRL_R = 0x000000007; 																// enable core clock with interrupts
}


void heartbeat(void){
	uint32_t count;
	count = 0;
	while(count >0){
		count--;
	}
	uint32_t mask;
	mask = GPIO_PORTF_DATA_R;
	mask &= 0x4;
	if(mask == 0x04){
		GPIO_PORTF_DATA_R &= 0xFB;
	}
	else{
		GPIO_PORTF_DATA_R |= 0x4;
	}
}

void UART1_Handler(){
	uint32_t ch;
	heartbeat();
	heartbeat();
	while((UART1_FR_R&UART_FR_RXFE)==0){		//fills fifo until no more new information is placed
		FiFo_Put(UART_InChar());
	}
	RxCounter++;
	UART1_ICR_R = 0x10;				//acknowledges the interrupt by clearing the flag which requested the interrupt
	heartbeat();
	
	}







void PortF_Init(void){
  uint8_t wait2;
	SYSCTL_RCGCGPIO_R |= 0x20;			
	wait2++;
	wait2++;
	
	GPIO_PORTF_DIR_R |= 0x0E;			//PF1,2,3, output
	GPIO_PORTF_DEN_R |= 0x0E;
}

// Get fit from excel and code the convert routine with the constants
// from the curve-fit
uint32_t Convert(uint32_t input){
  uint32_t position;
	position = (131*input-10496)/256;
	//position = (2111*input-41);
	return(position);
}


// final main program for bidirectional communication
// Sender sends using SysTick Interrupt
// Receiver receives using RX
int main(void){ 
	DisableInterrupts();

  
  TExaS_Init();       // Bus clock is 80 MHz 
	SysTick_Init();
	PortF_Init();
  ADC_Init();    // initialize to sample ADC1
  UART_Init();       // initialize UART
	ST7735_InitR(INITR_REDTAB);			//0x4000D000,0x4000D044
	ST7735_SetCursor(0,0);
  LCD_OutFix(0);
  ST7735_OutString("     cm");
//Enable SysTick Interrupt by calling SysTick_Init()
	FiFo_Init();
  EnableInterrupts();
	ST7735_SetCursor(0,0);
	
  while(1){
		
		//main_fifo();				//debugging fifo
		uint8_t i = 0;				//index to array
		//letter = UART1_DR_R;
		if(FiFo_Get(&GetData[0])==0x02){
			while(i<5){
			ST7735_OutChar((FiFo_Get(&GetData[i+1])));		//getting elements previously stored in array
			i++;
			
		}
			  ST7735_OutString("     cm");
				ST7735_SetCursor(0,0);
				i = 0; 						//resetting i
	}
}
	}
void SysTick_Handler(void){ // every 25 ms
	#define fresh  1
  #define stale  0
	uint32_t holder;
	heartbeat();
	Mailbox = ADC_In();
	//heartbeat();
	Mailbox = Convert(Mailbox);
	
	UART_OutChar(0x02);					//start of transmission
	UART_OutChar((Mailbox/1000) + 0x30);			//first digit
	UART_OutChar(0x2E);					// '.'
	holder = Mailbox%1000;
	holder = holder/100;
	UART_OutChar (holder+0x30);				//second digit
	holder = Mailbox%100;
	holder = holder/10;
	UART_OutChar (holder+0x30);				//third digit
	holder = Mailbox%10;
	UART_OutChar (holder+0x30);				//fourth digit
	UART_OutChar (0x0D);					//carriage
	UART_OutChar (0x03);					//end of transmission
	
	TxCounter++;
	flag = fresh;
	//heartbeat();
 //Similar to Lab9 except rather than grab sample and put in mailbox
 //        format message and transmit 
}


uint32_t Status[20];             // entries 0,7,12,19 should be false, others true
char GetData[10]; 		 // entries 1 2 3 4 5 6 7 8 should be 1 2 3 4 5 6 7 8
int main_fifo(void){ // Make this main to test FiFo
  FiFo_Init();
  for(;;){
    Status[0]  = FiFo_Get(&GetData[0]);  // should fail,    empty
    Status[1]  = FiFo_Put(1);            // should succeed, 1 
    Status[2]  = FiFo_Put(2);            // should succeed, 1 2
    Status[3]  = FiFo_Put(3);            // should succeed, 1 2 3
    Status[4]  = FiFo_Put(4);            // should succeed, 1 2 3 4
    Status[5]  = FiFo_Put(5);            // should succeed, 1 2 3 4 5
    Status[6]  = FiFo_Put(6);            // should succeed, 1 2 3 4 5 6
    Status[7]  = FiFo_Put(7);            // should fail,    1 2 3 4 5 6 
    Status[8]  = FiFo_Get(&GetData[1]);  // should succeed, 2 3 4 5 6
    Status[9]  = FiFo_Get(&GetData[2]);  // should succeed, 3 4 5 6
    Status[10] = FiFo_Put(7);            // should succeed, 3 4 5 6 7
    Status[11] = FiFo_Put(8);            // should succeed, 3 4 5 6 7 8
    Status[12] = FiFo_Put(9);            // should fail,    3 4 5 6 7 8 
    Status[13] = FiFo_Get(&GetData[3]);  // should succeed, 4 5 6 7 8
    Status[14] = FiFo_Get(&GetData[4]);  // should succeed, 5 6 7 8
    Status[15] = FiFo_Get(&GetData[5]);  // should succeed, 6 7 8
    Status[16] = FiFo_Get(&GetData[6]);  // should succeed, 7 8
    Status[17] = FiFo_Get(&GetData[7]);  // should succeed, 8
    Status[18] = FiFo_Get(&GetData[8]);  // should succeed, empty
    Status[19] = FiFo_Get(&GetData[9]);  // should fail,    empty
  }
}

