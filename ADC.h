// ADC.h
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0

// Last modified: 5/5/2016

// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void);

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void);
