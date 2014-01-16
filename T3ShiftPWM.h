#ifndef T3ShiftPWM_H
#define T3ShiftPWM_H

#include <Arduino.h>
#include "CT3ShiftPWM.h"

extern byte val;
extern const int T3ShiftPWM_latchPin;

void T3ShiftPWM_InterruptHandler(void);

CT3ShiftPWM T3ShiftPWM(T3ShiftPWM_latchPin, T3ShiftPWM_InterruptHandler);

#define add_one_pin_to_byte(sendInt, thres, ptr) { \
	unsigned char val = *ptr; \
	asm volatile("cmp %0, %1" :: "r" (thres), "r" (val) :); \
	asm volatile("rrx %0, %1" : "=r" (sendInt) : "r" (sendInt) :); \
}

void T3ShiftPWM_InterruptHandler(void) {
	// clear latch pin to signal write
	digitalWriteFast(T3ShiftPWM_latchPin, LOW);
	
	unsigned char * ptr = &T3ShiftPWM.m_values[T3ShiftPWM.m_outputCount];
	unsigned char counter = T3ShiftPWM.m_counter;
	
	for (unsigned int i = T3ShiftPWM.m_registerCount; i > 0; i--) {
		unsigned int tmp;
		unsigned char * tmpp;
		
		ptr -= 8;
		tmpp = ptr;
		
		add_one_pin_to_byte(tmp, counter, tmpp++);
		add_one_pin_to_byte(tmp, counter, tmpp++);
		add_one_pin_to_byte(tmp, counter, tmpp++);
		add_one_pin_to_byte(tmp, counter, tmpp++);
		
		add_one_pin_to_byte(tmp, counter, tmpp++);
		add_one_pin_to_byte(tmp, counter, tmpp++);
		add_one_pin_to_byte(tmp, counter, tmpp++);
		add_one_pin_to_byte(tmp, counter, tmpp++);
		
		// shift right 24 bits
		unsigned char sendByte = ~(tmp >> 24);
		val = sendByte;
		SPI.transfer(sendByte);
	}
	
	// set latch pin to signal end write
	digitalWriteFast(T3ShiftPWM_latchPin, HIGH);
	
	// increment the counter
	if (T3ShiftPWM.m_counter < T3ShiftPWM.m_maxBrightness) {
		T3ShiftPWM.m_counter++;
	}
	else {
		T3ShiftPWM.m_counter = 0;
	}
}

#endif
