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
 * @brief	TS-UNB abstractions for ATmega328p based Arduino systems
 *
 * @authors	Joerg Robert
 * @file	ArduinoTsUnb.h
 *
 */


#ifndef ARDUINO_TSUNB_H_
#define ARDUINO_TSUNB_H_

//! Frequency offste correciton of the processor to achieve precise symbol timing
#ifndef XTAL_PPM_OFFSET
#define XTAL_PPM_OFFSET		12
#endif


#include <inttypes.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <Arduino.h>
#include <SPI.h>

#include "ArduinoUtils.h"
#include "../TsUnb/RadioBurst.h"
#include "../TsUnb/FixedMac.h"
#include "../TsUnb/Phy.h"
#include "../TsUnb/SimpleNode.h"

namespace TsUnbLib {
namespace Arduino {
//! Flag to indicate trigger of time, set by timer interrupt
extern volatile bool TsUnbTimerFlag;

/**
 * @brief Platform dependent TS-UNB implementation for ATMega328p based Arduino systems.
 *
 * This class implents all plaform dependent methods for TS-UNB.
 * It mainly offer SPI communication and a timer to generate the TS-UNB symbol clock.
 *
 * @param CS_PIN              SPI chip select pin (default is 8)
 * @param SYMBOL_RATE_MULT    TS-UNB, symbol rate in multiples of 49.591064453125, set for 48 for 2380.371sym/s and 8 for 396.729sym/s. For higher rates the clock divider of the timer may have to be adjusted.
 * @param TIMING_OFFSET_PPM   Timing offset to correct XTAL frequency
 * @param CS_PULL_UP          Active pull of resistor (default is true).
 * @param SPI_INIT            Init SPI before use and de-init after use (default is true).
 * @param WDT_RST             Watchdog timer reset (default is true).
 */
template <uint8_t CS_PIN = 8, 
		uint16_t SYMBOL_RATE_MULT = 48,
		int16_t TIMING_OFFSET_PPM = 0,
		bool CS_PULL_UP = true, bool SPI_INIT = true, bool WDT_RESET = true>
class ArduinoTsUnb {
public:
	ArduinoTsUnb() {
	}

	~ArduinoTsUnb() {
	}

	/**
	 * @brief Bit duration in timer 1 counts
	 *
	 * This value is the duration of a single TS-UNB symbol, e.g. 1/2380.372s for the normal mode.
	 * The this constant assumes a CPU frequency of F_CPU and a clock divider for timer 1 of 256.
	 *
	 */
	static constexpr float TS_UNB_BIT_DURATION = (double)F_CPU / 256.0 / (49.591064453125 * (double)SYMBOL_RATE_MULT) * (1.0 + 1.0e-6 * TIMING_OFFSET_PPM);


	/**
	 * @brief Init the timer
	 */
	void initTimer() {
		cli();
		preciseTsUnbTimer = 0;
		TCCR1A = 0;		// No PWM
		TCCR1B = 0;		// No Timer
		TCNT1 = 0;
		OCR1A = OCR1B = 0;
		sei();
	}

	/**
	 * @brief Start the timer
	 */
	void startTimer() {
		cli();
		TsUnbTimerFlag = false;
		TCCR1A = 0;
		TIMSK1 |= (1 << OCIE1A);	// Timer match A Interrupt 
		TCCR1B |= (1 << CS12);		// Prescaler 1/256
		sei();
	}



	/**
	 * @brief Stop the timer to save energy
	 */
	void stopTimer() {
		cli();
		TCCR1A = 0;
		TIMSK1 = 0;
		TCNT1 = 0;
		sei();
	}

	/**
	 * @brief Add the counter compare value for the next interrupt
	 *
	 * @param count Delay in TX symbols
	 */
	void addTimerDelay(const int32_t count) {
		preciseTsUnbTimer += TS_UNB_BIT_DURATION * count;

		// Round to precise timer state to neared integer value and calculate value for compare match register 
		const uint16_t timerCompareMatch = (uint32_t)((int32_t)(preciseTsUnbTimer + 0.5f)) & 0xFFFF;
		cli();
		OCR1A = timerCompareMatch;
		sei();

		// Clip to range between 0 and 655356
		while (preciseTsUnbTimer >= 0x10000)
			preciseTsUnbTimer -= 0x10000;
	}

	/**
	 * @brief Wait until the timer values expires
	 */
	void waitTimer() const {
		do {
			if (TsUnbTimerFlag)
				break;
			set_sleep_mode(SLEEP_MODE_IDLE);
			sleep_mode();

		} while (true);
		TsUnbTimerFlag = false;

	}

	/**
	 * @brief Initialization of the SPI interface
	 */
	void spiInit(void) {
		digitalWrite(CS_PIN, HIGH);
		pinMode(CS_PIN, OUTPUT);
		if (SPI_INIT) 
			SPI.begin();
	}

	/**
	 * @brief Deinitialization of the SPI interface
	 */
	void spiDeinit(void) {
		if (CS_PULL_UP) {
			pinMode(CS_PIN, INPUT_PULLUP);
		}
		else {
			pinMode(CS_PIN, INPUT);
		}
		if (SPI_INIT) 
			SPI.end();

	}


	/**
	 * @brief Sends multiple bytes using SPI and sets the slave select pin accordingly
	 * 
	 * @param dataOut Bytes to be transmitted
	 * @param numBytes Number of bytes to be transmitted
	 */
	void spiSend(const uint8_t* const dataOut, const uint8_t numBytes) {
		SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
		digitalWrite(CS_PIN, LOW); 

		for(uint8_t i = 0; i < numBytes; ++i) {
			SPI.transfer(dataOut[i]);
		}
		digitalWrite(CS_PIN, HIGH); 

		SPI.endTransaction();
	}

	/**
	 * @brief Sends multiple and receives bytes using SPI and sets the slave select pin accordingly
	 *
	 * This method write and reads the SPI data. Please not that the read data has a delay of one byte.
	 * Hence, the first returned byte normally has no meaning.
	 * 
	 * @param dataInOut Bytes to be transmitted and buffer containing the read data
	 * @param numBytes  Number of bytes to be transmitted
	 */
	void spiSendReceive(uint8_t* const dataInOut, const uint8_t numBytes) {
		SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
		digitalWrite(CS_PIN, LOW); 

		for(uint8_t i = 0; i < numBytes; ++i) {
			dataInOut[i] = SPI.transfer(dataInOut[i]);
		}
		digitalWrite(CS_PIN, HIGH); 

		SPI.endTransaction();
	}

	/**
	 * @brief Resets watchdog time during transmissions
	 */
	void resetWatchdog() {
		if (WDT_RESET)
			wdt_reset();
	}


	//! Precise state of TsUnb timer
	float preciseTsUnbTimer;

};

};	// namespace Arduino
};	// namespace TsUnbLib
#include <ArduinoTsUnbTemplates.h>

#endif 	// ARDUINO_TSUNB_H_

