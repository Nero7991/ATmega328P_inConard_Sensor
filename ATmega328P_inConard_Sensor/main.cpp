/*
 * ATmega328P_inConard_Sensor.cpp
 *
 * Created: 3/30/2019 11:11:13 AM
 * Author : orencollaco
 */ 

#include "ProjectDef.h"
#include <avr/io.h>
#include "AVR.h"
#include "Essential.h"
#include "NRF24L01.h"
#include "Timer.h"
#include "Switch.h"
//extern SwitchClass Switch;

void runSetup();

void sensorTriggered(uint8_t Switch_ID);

void sensorNormal(uint8_t Switch_ID);

void timerDone(uint8_t Timer_ID);

volatile uint8_t SPIdata, SwitchID;
volatile bool TriggeredFlag, TimeOut, NormalFlag;
uint8_t Socket, State;
TimerClass Timer1, Timer2;
SwitchClass S1, S2, S3, S4, S5, S6;


int main(void)
{
	runSetup();
	_delay_ms(500);
	Notify(PSTR("Powering on..."));
	sei();
	TimeOut = false;
	NRF24L01 Radio(1,1,0);
	Radio.setTransmitAddress(0xEABABABAC2);
	Radio.setReceiveAddress(0xEABABABAC2, 0);
	//Radio.initialize();
	Timer1.begin();
	Timer1.initializeTimer();
	Timer2.initializeTimer();
	Timer1.setCallBackTime(100, 0, timerDone);
	S1.begin();
	S1.initializeSwitch(PORT_B, 0, &S1); //1
	S1.fallingEdge(sensorNormal);
	S1.risingEdge(sensorTriggered);
	Notify(PSTR("Done"));
	allowSleep(true);
	if(Radio.isRT_Max_Set())
	Radio.clearRT_Max();
	if(Radio.isTXFull())
	Radio.flushTX();
	if(Radio.isTX_DS_Set())
	Radio.clearTX_DS();
	while (1)
	{	
		if(TriggeredFlag){
			TriggeredFlag = false;
			#ifdef STATS
			printStringCRNL("Sensor triggered: ");
			printNumber(SwitchID);
			#endif
			Radio.fastTransferPayload(0x21);
			TimeOut = false;
			Timer2.setCallBackTime(50, 0, timerDone);
			while(Radio.isTX_DS_Set() != 0x20 && !TimeOut){
				
			}
			if(Radio.isTX_DS_Set() == 0x20){
				LED = 1;
				Radio.clearTX_DS();
				Timer1.setCallBackTime(50, 0, timerDone);
				Timer2.resetCallbackTimer();
				#ifdef STATS
				printStringCRNL("Payload transmitted.");
				#endif
				//Radio.flushTX();
			}
			else{
				TimeOut = false;
				//Radio.flushTX();
				#ifdef STATS
				printStringCRNL("Timed out");
				#endif
			}
			if(Radio.isRT_Max_Set())
			Radio.clearRT_Max();
			if(Radio.isTXFull())
			Radio.flushTX();
			if(Radio.isTX_DS_Set())
			Radio.clearTX_DS();
			#ifdef STATS
			Radio.printInfo();
			#endif
		}
		if(NormalFlag){
			NormalFlag = false;
			#ifdef STATS
			printStringCRNL("Sensor Normal: ");
			printNumber(SwitchID);
			#endif
			Radio.fastTransferPayload(0x20);
			TimeOut = false;
			Timer2.setCallBackTime(50, 0, timerDone);
			while(Radio.isTX_DS_Set() != 0x20 && !TimeOut){
				
			}
			if(Radio.isTX_DS_Set() == 0x20){
				LED = 1;
				Radio.clearTX_DS();
				Timer1.setCallBackTime(50, 0, timerDone);
				Timer2.resetCallbackTimer();
				#ifdef STATS
				printStringCRNL("Payload transmitted.");
				#endif
				//Radio.flushTX();
			}
			else{
				TimeOut = false;
				//Radio.flushTX();
				#ifdef STATS
				printStringCRNL("Timed out");
				#endif
			}
			if(Radio.isRT_Max_Set())
			Radio.clearRT_Max();
			if(Radio.isTXFull())
			Radio.flushTX();
			if(Radio.isTX_DS_Set())
			Radio.clearTX_DS();
			#ifdef STATS
			Radio.printInfo();
			#endif
		}
		if(sleepAllowed()){
			#ifdef STATS
			printStringCRNL("Sleeping...");
			#endif
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_enable();
			sei();
			getReadyToSleep();
			CE = 0;
			sleep_mode();
			CE = 1;
			_delay_ms(20);
		}
	}
}

void sensorTriggered(uint8_t Switch_ID){
	TriggeredFlag = true;
	SwitchID = Switch_ID;
	allowSleep(false);
}

void sensorNormal(uint8_t Switch_ID){
	NormalFlag = true;
	allowSleep(false);
}

void timerDone(uint8_t Timer_ID){
	#ifdef DEBUG_MAIN
	printStringCRNL("Timer done.");
	#endif
	 TimeOut = true;
	 LED = 0;
	 allowSleep(1);
}


void runSetup(){
	USART_Init(MYUBRR);
	Init_CTC_T1(2,1000);
	SPI_MasterInit();
	setPinDirection(PORT_D, 2, 1);
	setPinDirection(PORT_B, 6, 1);
	SENSOR_POWER = 1;
}




