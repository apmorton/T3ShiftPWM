#ifndef CT3ShiftPWM_h
#define CT3ShiftPWM_h

#include <Arduino.h>
#include <IntervalTimer.h>
#include "../SPI/SPI.h"

typedef void (*ISR)();

class CT3ShiftPWM {
public:
	CT3ShiftPWM(int latchPin, ISR isr);
	~CT3ShiftPWM();

public:
	void Start(int ledFrequency, unsigned char maxBrightness);
	void ResumeInterrupt(void);
	void PauseInterrupt(void);
	void SetRegisterCount(unsigned char registerCount);
	bool SetInterruptHandler(ISR isr);
	void PrintInterruptLoad(void);
	void SetOne(int channel, unsigned char value);
	void SetAll(unsigned char value);

private:
	bool IsValidChannel(int channel);

	IntervalTimer *m_timer;
	ISR m_ISR;

public:
	const int m_latchPin;
	int m_ledFrequency;
	int m_period;
	unsigned char m_maxBrightness;
	unsigned char m_registerCount;
	int m_outputCount;
	unsigned char * m_values;
	unsigned char m_counter;
};



#endif
