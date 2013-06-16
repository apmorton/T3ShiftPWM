#include <Arduino.h>
#include "CT3ShiftPWM.h"

CT3ShiftPWM::CT3ShiftPWM(int latchPin, ISR isr) :
		m_latchPin(latchPin) {
	m_timer = new IntervalTimer();
	m_ledFrequency = 0;
	m_maxBrightness = 0;
	m_period = 0;
	m_registerCount = 0;
	m_outputCount = 0;
	m_values = 0;
	
	SetInterruptHandler(isr);
}

CT3ShiftPWM::~CT3ShiftPWM() {
	if(m_values>0){
		free(m_values);
	}
}

void CT3ShiftPWM::Start(int ledFrequency, unsigned char maxBrightness) {
	m_ledFrequency = ledFrequency;
	m_maxBrightness = maxBrightness;
	
	SPI.begin();
	pinMode(m_latchPin, OUTPUT);
	digitalWrite(m_latchPin, HIGH);
	m_period = 1000000.0 / (ledFrequency * (maxBrightness + 1));
	ResumeInterrupt();
}

void CT3ShiftPWM::ResumeInterrupt(void) {
	m_timer->begin(m_ISR, m_period);
}

void CT3ShiftPWM::PauseInterrupt(void) {
	m_timer->end();
}

void CT3ShiftPWM::SetRegisterCount(unsigned char registerCount) {
	m_registerCount = registerCount;
	m_outputCount = registerCount * 8;
	
	m_values = (unsigned char *) malloc(m_outputCount);
	SetAll(0);
}

bool CT3ShiftPWM::SetInterruptHandler(ISR isr) {
	m_ISR = isr;
}

void CT3ShiftPWM::PrintInterruptLoad(void) {
	unsigned long start1,end1,time1,start2,end2,time2,k;
	double load, cycles_per_int, interrupt_frequency;

	ResumeInterrupt();
	
	//run with interrupt enabled
	start1 = micros();
	for(k=0; k<100000; k++){
		delayMicroseconds(1);
	}
	end1 = micros();
	time1 = end1-start1;

	PauseInterrupt();

	// run with interrupt disabled
	start2 = micros();
	for(k=0; k<100000; k++){
		delayMicroseconds(1);
	}
	end2 = micros();
	time2 = end2-start2;

	ResumeInterrupt();

	// ready for calculations
	load = (double)(time1-time2)/(double)(time1);
	interrupt_frequency = m_ledFrequency * (m_maxBrightness + 1);
	cycles_per_int = load*(F_CPU/interrupt_frequency);

	//Ready to print information
	Serial.print("Load of interrupt: "); Serial.println(load,10);
	Serial.print("Clock cycles per interrupt: "); Serial.println(cycles_per_int);
	Serial.print("Interrupt frequency: "); Serial.print(interrupt_frequency);   Serial.println(" Hz");
	Serial.print("PWM frequency: "); Serial.print(interrupt_frequency/(m_maxBrightness+1)); Serial.println(" Hz");
}

void CT3ShiftPWM::SetOne(int channel, unsigned char value) {
	if(IsValidChannel(channel) ){
		m_values[channel] = value;
	}
}

void CT3ShiftPWM::SetAll(unsigned char value) {
	for(int i = 0; i < m_outputCount; i++){
		m_values[i] = value;
	}
}

bool CT3ShiftPWM::IsValidChannel(int channel) {
	return (0 <= channel && channel < m_outputCount);
}
