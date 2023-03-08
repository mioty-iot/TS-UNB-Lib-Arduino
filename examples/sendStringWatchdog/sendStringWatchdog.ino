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
 * @brief Example project for the LIKE Arduino node
 *
 * This code including the watchdog should not be used in combination with the
 * Arudino bootloader. If you want to use this code, please only use a programmer.
 * Otherwise the code may not work correctly (e.g. bootloader does not start).
 *
 * @authors Clemens Neumueller, Joerg Robert
 *
 */


#include <ArduinoTsUnb.h>

// Please do not forget to update the MAC_EU64 and the MAC_NETWORK_KEY!
// Always use a seperate key for each devices, as the system may not work otherwise.
// If you do not have an EUI64, please generate a random EUI for test purposes.

//! This is the node specific MAC address, replac with real address
#define MAC_EUI64			0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08

//! This is the node specific network key, replace with real key
#define MAC_NETWORK_KEY		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10


//! Transmit power in dBm, please keep in mind local regulations
#define TRANSMIT_PWR		10

using namespace TsUnbLib::Arduino;

// Select preset depending on TX chip. This example uses Arduino PIN 8 for Chip Select.
// Please check that you use the correct RFM69 device. The Rfm69hw uses the BOOST pin for the transmisson. 
TsUnb_EU1_Rfm69w_t<8> TsUnb_Node;
//TsUnb_EU1_Rfm69hw_t<8> TsUnb_Node;




//The setup function is called once at startup of the sketch
void setup() {
	// Init watchdog configuration, system is reset if wdtReset() is not called every 8s
	// Please note that the watchdog should NOT be used if the system uses the Arduino boot loader
	wdtOff();
	wdtEnable();
	delay(100);

	// Blink the LED
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(100);
	digitalWrite(LED_BUILTIN, LOW);

	// Init the parameters
	TsUnb_Node.init();
	TsUnb_Node.Tx.setTxPower(TRANSMIT_PWR);
	TsUnb_Node.Mac.setNetworkKey(MAC_NETWORK_KEY);
	TsUnb_Node.Mac.setAddress(MAC_EUI64);

	// TS-Unb ignores packets with an PkgCnt already received
	// We use this function to configure the PkgCnt from the
	// EEPROM
	TsUnb_Node.Mac.extPkgCnt = initExtPkgCnt();


	// Blink LED while not transmitting
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(1000);
	digitalWrite(LED_BUILTIN, LOW);
	wdtReset();
}

// The loop function is called in an endless loop
void loop() {
	wdtEnable(); // Enable 8s supervision

	// Send the text "Hello"
	char str[] = "Hello";
	TsUnb_Node.send((uint8_t *)str, sizeof(str) / sizeof(str[0]) - 1);

	// We store the current PkgCnt to the EEPROM to 
	// avoid the repetion of packets with the same count.
	// This value is only written every 256 packets to
	// save energy.
	updateExtPkgCnt(TsUnb_Node.Mac.extPkgCnt);

	wdtReset();


	// Blink the LED twice to indicate end of transmission 
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(100);
	digitalWrite(LED_BUILTIN, LOW);
	delay(100);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(100);
	digitalWrite(LED_BUILTIN, LOW);

	// Sleep the device for 5 seconds using the watchdog timer (very low energy consumption)
	wdtSleep(5);
}

