; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11

num EQU 0x0
	
LCD_OutDec
	PUSH {R4-R11}
	
	MOV R2, #0xFFFF
	SUB SP, #8
	STR R2, [SP, #num]
	LDR R1, [SP, #num]
	ADD SP, #8
	
	MOV R2, #0
	MOV R1, #1
	
	PUSH {LR,R0}
	
	MOV R10, #10		;ten
	MOV R11, #0			;counter
	
FindDigit

	UDIV R1, R0, R10	;isolates number to push onto stack into R3
	MUL R2, R1, R10
	SUB R3, R0, R2
	
	UDIV R0, R10		;adjusting original value after units digit accounted for
	
	PUSH {R3, R0}		;push local variable onto stack
	
	ADD R11, #1			;increment counter
	
	CMP R1, #0			;if there are still digits left to be displayed, go back and deal with them
	BNE FindDigit
	
	
	
WriteDigit
	POP {R3, R0}
	ADD R0, R3, #0x30	;convert to ascii
	
	BL ST7735_OutChar

	SUBS R11, #1		;if there are still digits left to be displayed, go back and deal with them
	BNE WriteDigit
	
	POP {LR, R0}
	POP {R4-R11}
	
	BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
	PUSH {LR, R0}
	PUSH {R4-R11}
	
	MOV R10, #10		;ten
	MOV R11, #0			;counter
	
	MOV R12, #10000		;deals with case of more than 4 digits
	CMP R0, R12
	BHS Stars
	
FixFindDigit
	UDIV R1, R0, R10	;isolates number to push onto stack into R3
	MUL R2, R1, R10
	SUB R3, R0, R2
	
	UDIV R0, R10		;adjusting original value after units digit accounted for
	
	PUSH {R3, R0}		;push local variable onto stack
	
	ADD R11, #1			;increment counter
	
	CMP R11, #4			;if there are still digits left to be displayed, go back and deal with them
	BNE FixFindDigit
	
	POP {R3, R0}
	ADD R0, R3, #0x30	;convert to ascii
	
	BL ST7735_OutChar

	SUB R11, #1
	
	MOV R0,#0x2E
	BL ST7735_OutChar
	
FixWriteDigit
	POP {R3, R0}
	ADD R0, R3, #0x30	;convert to ascii
	
	BL ST7735_OutChar

	SUBS R11, #1		;if there are still digits left to be displayed, go back and deal with them
	BNE FixWriteDigit
		
EndStars
	POP {R4-R11}
	POP {LR, R0}

	BX   LR

Stars
	MOV R0, #0x2A					;output "*.***"
	BL ST7735_OutChar
	MOV R0,#0x2E
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar

	B EndStars
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
