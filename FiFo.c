// FiFo.c
// Runs on LM4F120/TM4C123
// Provide functions that implement the Software FiFo Buffer
// Last Modified: 4/12/2016 
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
// Declare state variables for FiFo
//        size, buffer, put and get indexes

// *********** FiFo_Init**********
// Initializes a software FIFO of a
// fixed size and sets up indexes for
// put and get operations
#define size 9
#define fail 0
#define success 1

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

char fifo[size];
uint8_t putI,getI;
uint32_t count=0;

void FiFo_Init() {
putI = getI = 0;

}

// *********** FiFo_Put**********
// Adds an element to the FIFO
// Input: Character to be inserted
// Output: 1 for success and 0 for failure
//         failure is when the buffer is full
uint32_t FiFo_Put(char data) {
	if(count == size){
		return fail;				//full
	}
	fifo[putI] = data;
	putI = (putI+1)%size;
	count++;
	return(success);
}

// *********** FiFo_Get**********
// Gets an element from the FIFO
// Input: Pointer to a character that will get the character read from the buffer
// Output: 1 for success and 0 for failure
//         failure is when the buffer is empty
uint32_t FiFo_Get(char *datapt)
{
	if(count==0){
		return (fail);		// empty
	}
	*datapt = fifo[getI];
	getI=(getI+1)%size;
	DisableInterrupts();
	count--;
	EnableInterrupts();
	return(*datapt);
	
}



