// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 3/6/2015 
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void){ 
	uint32_t delay;
	
	
	SYSCTL_RCGCGPIO_R |= 0x10;
	while((SYSCTL_PRGPIO_R&0x10) == 0){};
		GPIO_PORTE_DIR_R &= ~0x02;
		GPIO_PORTE_AFSEL_R |= 0x02;
		GPIO_PORTE_DEN_R &= ~0x02;
		GPIO_PORTE_AMSEL_R |= 0x02;		
		SYSCTL_RCGCADC_R |= 0x01;			// activate ADC0
		delay = SYSCTL_RCGCADC_R;
		delay = SYSCTL_RCGCADC_R;
		delay = SYSCTL_RCGCADC_R;
		
		ADC0_PC_R = 0x01;							//configure for 125k
		ADC0_SSPRI_R = 0x0123;				//seq 3 is highest priority
		ADC0_ACTSS_R &= ~0xF000;			//disable sample sequencer 3
		ADC0_EMUX_R &= ~0xF000;				//seq 3 is software trigger
		ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+1; // Ain1 (PE2)
		ADC0_SSCTL3_R = 0x0006;				//noTS0 D0, yes IE0 END0
		ADC0_IM_R &= ~0x0008;					//disable ss3
		ADC0_ACTSS_R |= 0x0008;				//enable sample sequencer 3


}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void){  
	uint32_t data;
	
	ADC0_PSSI_R |= 0x08;				//setting bit 3 (initiation)
	while(ADC0_RIS_R == 0){};		// wait until not busy
		
		data = ADC0_SSFIFO3_R;
		ADC0_ISC_R |= 0x8;				//clears bit 3 in RIS to make it busy again
		
		return(data);

}


