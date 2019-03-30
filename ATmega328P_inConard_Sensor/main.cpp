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
void printSomething(uint8_t);
void runSetup();
void pullSlaveSelectLow(uint8_t SwitchID);
void pullSlaveSelectHigh(uint8_t SwitchID);
void portStateChange(uint8_t PortNo);
void switchPressed(uint8_t Switch_ID);
void timerDone(uint8_t Timer_ID);
volatile uint8_t SPIdata, SwitchID;
volatile bool SwitchPressedFlag, TimeOut;
uint8_t Socket, State;
TimerClass Timer1;
int main(void)
{
	runSetup();
	_delay_ms(500);
	Notify(PSTR("Powering on..."));
	sei();
	TimeOut = false;
	NRF24L01 Radio(1,1,0);
	SwitchClass S1, S2, S3, S4, S5, S6;
	Timer1.begin();
	S1.begin();
	S1.initializeSwitch(PORT_B, 1, &S1);
	S2.initializeSwitch(PORT_B, 0, &S2);
	S3.initializeSwitch(PORT_B, 6, &S3);
	S4.initializeSwitch(PORT_B, 7, &S4);
	S5.initializeSwitch(PORT_D, 6, &S5);
	S6.initializeSwitch(PORT_D, 7, &S6);
	S1.shortPress(switchPressed);
	S1.enableSamePtrMode(true);
	Timer1.initializeTimer();
	Timer1.setCallBackTime(100, 0, timerDone);
	Notify(PSTR("Done"));
	allowSleep(true);
	while (1)
	{	
		if(SwitchPressedFlag){
			SwitchPressedFlag = false;
			if(Radio.isTXFull())
			Radio.flushTX();
			if(Radio.isTX_DS_Set())
			Radio.clearTX_DS();
			#ifdef STATS
			printStringCRNL("Switch Pressed: ");
			#endif
			printNumber(SwitchID);
			Radio.fastTransferPayload(SwitchID);
			Timer1.setCallBackTime(25, 0, timerDone);
			while(!TimeOut && !Radio.isTX_DS_Set()){
				
			}
			if(TimeOut){
				TimeOut = false;
			}
			else{
				LED = 1;
				Timer1.setCallBackTime(200, 0, timerDone);
				printStringCRNL("Payload transmitted.");
				Radio.clearTX_DS();
			}
			#ifdef STATS
			Radio.printInfo();
			#endif
			if(sleepAllowed()){
				set_sleep_mode(SLEEP_MODE_PWR_DOWN);
				sleep_enable();
				sei();
				getReadyToSleep();
				#ifdef STATS
				printStringCRNL("Sleeping...");
				#endif
				//FLASH = 0;
				sleep_mode();
			}
		}
	}
}

void switchPressed(uint8_t Switch_ID){
	#ifdef DEBUG_MAIN
	printStringCRNL("press");
	#endif
	SwitchPressedFlag = true;
	SwitchID = Switch_ID;
}

void timerDone(uint8_t Timer_ID){
	#ifdef DEBUG_MAIN
	printStringCRNL("Timer done.");
	#endif
	 TimeOut = true;
	 LED = 0;
}

void portStateChange(uint8_t PortNo){
	printStringCRNL("Pin state changed");
	if(PortNo == PORT_C){
		setPinState(PORT_C, 2, getPinState(PORT_C, 0));
	}
}

void runSetup(){
	
	USART_Init(MYUBRR);
	Init_CTC_T1(2,1000);
	SPI_MasterInit();
	setPinDirection(PORT_D, 2, 1);
	//enableSPIInterrupt(true);
	//sei();
}


