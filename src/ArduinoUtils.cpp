/* -----------------------------------------------------------------------------

Software License for the Fraunhofer TS-UNB-Lib

(c) Copyright  2019 - 2023 Fraunhofer-Gesellschaft zur Förderung der angewandten
Forschung e.V. All rights reserved.


1. INTRODUCTION

The Fraunhofer Telegram Splitting - Ultra Narrowband Library ("TS-UNB-Lib") is software
that implements only the uplink of the ETSI TS 103 357 TS-UNB standard ("MIOTY") for wireless 
data transmission in the field of IoT. Patent licenses for any patent claim regarding the 
ETSI TS 103 357 TS-UNB standard implementation (including those of Fraunhofer) may be 
obtained through Sisvel International S.A. 
(https://www.sisvel.com/licensing-programs/wireless-communications/mioty/license-terms)
or through the respective patent owners individually. The purpose of this TS-UNB-Lib is 
academic and non-commercial use. Therefore, Fraunhofer does not offer any support for the 
TS-UNB-Lib. Furthermore, the TS-UNB-Lib is NOT identical and on the same quality level as 
the commercially-licensed MIOTY software also available from Fraunhofer. Users are encouraged
to check the Fraunhofer website for additional applications information and documentation.


2. COPYRIGHT LICENSE

Redistribution and use in source and binary forms, with or without modification, are 
permitted without payment of copyright license fees provided that you satisfy the following 
conditions: You must retain the complete text of this software license in redistributions
of the TS-UNB-Lib software or your modifications thereto in source code form. You must retain 
the complete text of this software license in the documentation and/or other materials provided
with redistributions of the TS-UNB-Lib software or your modifications thereto in binary form.
You must make available free of charge copies of the complete source code of the TS-UNB-Lib 
software and your modifications thereto to recipients of copies in binary form. The name of 
Fraunhofer may not be used to endorse or promote products derived from this software without
prior written permission. You may not charge copyright license fees for anyone to use, copy or
distribute the TS-UNB-Lib software or your modifications thereto. Your modified versions of the
TS-UNB-Lib software must carry prominent notices stating that you changed the software and the
date of any change. For modified versions of the TS-UNB-Lib software, the term 
"Fraunhofer TS-UNB-Lib" must be replaced by the term
"Third-Party Modified Version of the Fraunhofer TS-UNB-Lib."


3. NO PATENT LICENSE

NO EXPRESS OR IMPLIED LICENSES TO ANY PATENT CLAIMS, including without limitation the patents 
of Fraunhofer, ARE GRANTED BY THIS SOFTWARE LICENSE. Fraunhofer provides no warranty of patent 
non-infringement with respect to this software. You may use this TS-UNB-Lib software or modifications
thereto only for purposes that are authorized by appropriate patent licenses.


4. DISCLAIMER

This TS-UNB-Lib software is provided by Fraunhofer on behalf of the copyright holders and contributors
"AS IS" and WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, including but not limited to the implied warranties
of merchantability and fitness for a particular purpose. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE for any direct, indirect, incidental, special, exemplary, or consequential damages,
including but not limited to procurement of substitute goods or services; loss of use, data, or profits,
or business interruption, however caused and on any theory of liability, whether in contract, strict
liability, or tort (including negligence), arising in any way out of the use of this software, even if
advised of the possibility of such damage.


5. CONTACT INFORMATION

Fraunhofer Institute for Integrated Circuits IIS
Attention: Division Communication Systems
Am Wolfsmantel 33
91058 Erlangen, Germany
ks-contracts@iis.fraunhofer.de

----------------------------------------------------------------------------- */

/**
 * @brief	Source file containing utils for Arduino
 *
 * @authors	Clemens Neumueller, Joerg Robert
 * @file	ArduinoUtils.cpp
 *
 */

// This code is only used in case of the AVR processor
#ifdef __AVR_ARCH__


#include <EEPROM.h>

#include "ArduinoUtils.h"

namespace TsUnbLib {
namespace Arduino {


//! Global register for timing interrupt counters
volatile uint16_t wdtCycles;



void sleepMCU() {
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	//power_all_disable();
	sleep_mode();
	sleep_disable();
	//power_all_enable();
}


void wdtSleep(uint16_t seconds) {
	// SPI lanes have to be on a defined state in order to not wake the TRX
	uint8_t singleSeconds = seconds & 7;
	seconds >>= 3; // divide by 8
	// Turn off the ADC
	ADCSRA = 0;
	// turn off brown-out enable in software
	MCUCR = bit (BODS) | bit (BODSE);
	MCUCR = bit (BODS); 
	MCUSR &= ~(1<<WDRF);
	if(seconds)
	{
		cli();  // Disable interrups
		wdt_reset();    // Reset WDT
		wdtCycles = 0;
		WDTCSR |= (1<<WDCE) | (1<<WDE);
		WDTCSR = (1<<WDIE) | (1<<WDP0) | (1<<WDP3); // 1024k cycles, 8 seconds, enable wdt
		sei();  // Enable interrups
		while(wdtCycles < seconds)
		{
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_enable();
			sleep_mode();
			interrupts ();		// guarantees next instruction executed
			sleep_cpu ();		// sleep within 3 clock cycles of above    
		}
	}
	if(singleSeconds)
	{
		cli();  // Disable interrups
		wdt_reset();    // Reset WDT
		wdtCycles = 0;
		WDTCSR |= (1<<WDCE) | (1<<WDE);
		WDTCSR = (1<<WDIE) | (1<<WDP1) | (1<<WDP2); // 128k cycles, 1 seconds, enable wdt
		sei();  // Enable interrups
		while(wdtCycles < singleSeconds)
		{
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_enable();
			// power_all_disable();
			sleep_mode();
			interrupts ();             // guarantees next instruction executed
			sleep_cpu ();              // sleep within 3 clock cycles of above    
		}
	}
	sleep_disable();
	power_all_enable();
	wdtOff(); // disable wdt
}

void wdtEnable() {
	wdtCycles = 0;
	cli();  // Disable interrupts
	wdt_reset();    // Reset WDT
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	WDTCSR = (1<<WDE) | (1<<WDP0) | (1<<WDP3); // 1024k cycles, 8 seconds, enable wdt with reset!
	sei();  // Enable interrutps
}

/**
 * @brief Interrupt function for watchdog to create timer
 */
ISR(WDT_vect) {
	wdt_reset();    // Reset WDT
	++wdtCycles;
}

void wdtOff() {
	cli();  // Disable interrups
	wdt_reset();    // Reset WDT
	/* Clear WDRF in MCUSR */
	MCUSR &= ~(1<<WDRF);
	/* Write logical one to WDCE and WDE */
	/* Keep old prescaler setting to prevent unintentional time-out */
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	/* Turn off WDT */
	WDTCSR = 0x00;
	sei();  // Enable interrups

}

uint32_t initExtPkgCnt(void) {
	uint32_t extPkgCnt;
	extPkgCnt = (uint32_t)EEPROM.read(0) << 16;
	extPkgCnt |= (uint32_t)EEPROM.read(1) << 8;
	extPkgCnt |= (uint32_t)EEPROM.read(2) << 0;

	// Reset if not initialized or add 0x100 if already initialized to avoid double counters
	if (extPkgCnt == 0xFFFFFF) {
		extPkgCnt = 0;
	}
	else {
		extPkgCnt += 0x100;
	}

	// Limit to 24 bit
	extPkgCnt &= 0xFFFFFF;

	updateExtPkgCnt(extPkgCnt, true);

	return extPkgCnt;
}

bool updateExtPkgCnt(uint32_t extPkgCnt, bool forceWrite) {
	if (((extPkgCnt & 0xFF) == 0) || forceWrite) {
		EEPROM.write(0, (uint8_t)(extPkgCnt >> 16));
		EEPROM.write(1, (uint8_t)(extPkgCnt >> 8));
		EEPROM.write(2, (uint8_t)(extPkgCnt >> 0));

		return true;
	}
	else {
		return false;
	}
}

};	// namespace Arduino
};	// namespace TsUnbLib



#endif // __AVR_ARCH__

