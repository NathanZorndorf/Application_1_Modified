/*  ============================================================================
 *   Author: Nathan Zorndorf 
 *   Description: UART Initialization and Test 
 */

#include <stdio.h>
#include <csl_general.h>
#include <csl_uart.h>
#include <usbstk5505.h>

// UART #define's
#define TIMEOUT_VALUE	1000
#define COUNT 			3 
#define ONE_HALF_SECOND	500000 // 500,000 microseconds in 0.5 seconds

// MIDI #define's 
#define NOTE_ON		144  // 0x90 - Note ON  for Channel 0 - This should be the first byte in a Note ON  sequence. 
#define NOTE_OFF 	128  // 0x80 - Note OFF for Channel 0 - This should be the first byte in a Note OFF sequence. 
#define NOTE_VALUE	96   // 0x60 - 
#define NOTE_ON_VELOCITY	100 // 0x64 - Velocity = 100 (Range is 1 - 127) - This should be the third byte in a Note ON  sequence. 
#define NOTE_OFF_VELOCITY	127 // 0x7F - Velocity = 127 (Range is 1 - 127) - This should be the third byte in a Note OFF sequence. 

int My_UART(int MIDI_Number){
	
	char pBuf[3];  // UART TX Buffer - char is 1 byte - ?
	
	pbuf[2] = NOTE_ON_VELOCITY; // hardcode the velocity value of each note to be 100
	
	Current_MIDI_Number = MIDI_Number; // make the input MIDI_Number equal to the current MIDI number
	
	if(Current_MIDI_Number != Previous_MIDI_Number) {
		pbuf[0] = NOTE_OFF; 			// turn OFF the PREVIOUS MIDI note
		pbuf[1] = Previous_MIDI_Number;	
		status 	= UART_fputc(hUart, pBuf[0], TIMEOUT_VALUE);
		status 	= UART_fputc(hUart, pBuf[1], TIMEOUT_VALUE);
		status 	= UART_fputc(hUart, pBuf[2], TIMEOUT_VALUE);
		
		pbuf[0] = NOTE_ON; 				// turn ON the CURRENT MIDI note
		pbuf[1] = Current_MIDI_Number;	
		status 	= UART_fputc(hUart, pBuf[0], TIMEOUT_VALUE);
		status 	= UART_fputc(hUart, pBuf[1], TIMEOUT_VALUE);
		status 	= UART_fputc(hUart, pBuf[2], TIMEOUT_VALUE);
	}

	Previous_MIDI_Number = Current_MIDI_Number;

}
