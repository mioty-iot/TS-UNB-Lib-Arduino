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
 * @brief	TS-UNB configs for different frequency ranges and TX chips for ATmega328p based Arduino systems
 *
 * @authors	Joerg Robert
 * @file	ArduinoTsUnbTemplates.h
 *
 */


#ifndef ARDUINO_TSUNB_TEMPLATES_H_
#define ARDUINO_TSUNB_TEMPLATES_H_

#include <../Trx/Rfm69hw.h>

namespace TsUnbLib {
namespace Arduino {
	
/////////////////////////////////
// Configuration for the RFM69w
/////////////////////////////////

//! RFM69hw in EU0 configuration, no Sync Burst
template<uint8_t CS_PIN = 8>
using TsUnb_EU0_Rfm69w_t = TsUnb::SimpleNode<TsUnb::FixedUplinkMac, 
		TsUnb::Phy<14224261, 14224261, 39, 39, TsUnb::TsUnb_UPG1, 3, TsUnb::RadioBurst <2,2> >,
		Trx::Rfm69hw<ArduinoTsUnb<CS_PIN, 48, XTAL_PPM_OFFSET>, false, 10, TsUnb::RadioBurst <2,2> >, false>;

//! RFM69hw in EU1 configuration, no Sync Burst
template<uint8_t CS_PIN = 8>
using TsUnb_EU1_Rfm69w_t = TsUnb::SimpleNode<TsUnb::FixedUplinkMac, 
		TsUnb::Phy<14224261, 14222623, 39, 39, TsUnb::TsUnb_UPG1, 3, TsUnb::RadioBurst <2,2> >,
		Trx::Rfm69hw<ArduinoTsUnb<CS_PIN, 48, XTAL_PPM_OFFSET>, false, 10, TsUnb::RadioBurst <2,2> >, false>;

//! RFM69hw in EU2 configuration (includes bugfix wrt. V 1.1.1 of the TS-UNB specification, correct are 867.625MHz and 866.825MHz), no Sync Burst
template<uint8_t CS_PIN = 8>
using TsUnb_EU2_Rfm69w_t = TsUnb::SimpleNode<TsUnb::FixedUplinkMac, 
		TsUnb::Phy<14215168, 14202061, 468, 39, TsUnb::TsUnb_UPG1, 3, TsUnb::RadioBurst <2,2> >,
		Trx::Rfm69hw<ArduinoTsUnb<CS_PIN, 48, XTAL_PPM_OFFSET>, false, 10, TsUnb::RadioBurst <2,2> >, false>;

//! RFM69hw in EU0 configuration, no Sync Burst
template<uint8_t CS_PIN = 8>
using TsUnb_EU0_LowLatency_Rfm69w_t = TsUnb::SimpleNode<TsUnb::FixedUplinkMac, 
		TsUnb::Phy<14224261, 14224261, 39, 39, TsUnb::TsUnb_UPG3, 3, TsUnb::RadioBurst <2,2> >,
		Trx::Rfm69hw<ArduinoTsUnb<CS_PIN, 48, XTAL_PPM_OFFSET>, false, 10, TsUnb::RadioBurst <2,2> >, false>;

//! RFM69hw in EU1 configuration with Low Latency Uplink Pattern Group 3, no Sync Burst
template<uint8_t CS_PIN = 8>
using TsUnb_EU1_LowLatency_Rfm69w_t = TsUnb::SimpleNode<TsUnb::FixedUplinkMac, 
		TsUnb::Phy<14224261, 14222623, 39, 39, TsUnb::TsUnb_UPG3, 3, TsUnb::RadioBurst <2,2> >,
		Trx::Rfm69hw<ArduinoTsUnb<CS_PIN, 48, XTAL_PPM_OFFSET>, false, 10, TsUnb::RadioBurst <2,2> >, false>;

//! RFM69hw in EU2 configuration with Low Latency Uplink Pattern Group 3, no Sync Burst
template<uint8_t CS_PIN = 8>
using TsUnb_EU2_LowLatency_Rfm69w_t = TsUnb::SimpleNode<TsUnb::FixedUplinkMac, 
		TsUnb::Phy<14215168, 14202061, 468, 39, TsUnb::TsUnb_UPG3, 3, TsUnb::RadioBurst <2,2> >,
		Trx::Rfm69hw<ArduinoTsUnb<CS_PIN, 48, XTAL_PPM_OFFSET>, false, 10, TsUnb::RadioBurst <2,2> >, false>;



/////////////////////////////////
// Configuration for the RFM69hw
/////////////////////////////////

//! RFM69hw in EU0 configuration, no Sync Burst
template<uint8_t CS_PIN = 8>
using TsUnb_EU0_Rfm69hw_t = TsUnb::SimpleNode<TsUnb::FixedUplinkMac, 
		TsUnb::Phy<14224261, 14224261, 39, 39, TsUnb::TsUnb_UPG1, 3, TsUnb::RadioBurst <2,2> >,
		Trx::Rfm69hw<ArduinoTsUnb<CS_PIN, 48, XTAL_PPM_OFFSET>, true, 10, TsUnb::RadioBurst <2,2> >, false>;

//! RFM69hw in EU1 configuration, no Sync Burst
template<uint8_t CS_PIN = 8>
using TsUnb_EU1_Rfm69hw_t = TsUnb::SimpleNode<TsUnb::FixedUplinkMac, 
		TsUnb::Phy<14224261, 14222623, 39, 39, TsUnb::TsUnb_UPG1, 3, TsUnb::RadioBurst <2,2> >,
		Trx::Rfm69hw<ArduinoTsUnb<CS_PIN, 48, XTAL_PPM_OFFSET>, true, 10, TsUnb::RadioBurst <2,2> >, false>;

//! RFM69hw in EU2 configuration (includes bugfix wrt. V 1.1.1 of the TS-UNB specification, correct are 867.625MHz and 866.825MHz), no Sync Burst
template<uint8_t CS_PIN = 8>
using TsUnb_EU2_Rfm69hw_t = TsUnb::SimpleNode<TsUnb::FixedUplinkMac, 
		TsUnb::Phy<14215168, 14202061, 468, 39, TsUnb::TsUnb_UPG1, 3, TsUnb::RadioBurst <2,2> >,
		Trx::Rfm69hw<ArduinoTsUnb<CS_PIN, 48, XTAL_PPM_OFFSET>, true, 10, TsUnb::RadioBurst <2,2> >, false>;

//! RFM69hw in EU0 configuration, no Sync Burst
template<uint8_t CS_PIN = 8>
using TsUnb_EU0_LowLatency_Rfm69hw_t = TsUnb::SimpleNode<TsUnb::FixedUplinkMac, 
		TsUnb::Phy<14224261, 14224261, 39, 39, TsUnb::TsUnb_UPG3, 3, TsUnb::RadioBurst <2,2> >,
		Trx::Rfm69hw<ArduinoTsUnb<CS_PIN, 48, XTAL_PPM_OFFSET>, true, 10, TsUnb::RadioBurst <2,2> >, false>;

//! RFM69hw in EU1 configuration with Low Latency Uplink Pattern Group 3, no Sync Burst
template<uint8_t CS_PIN = 8>
using TsUnb_EU1_LowLatency_Rfm69hw_t = TsUnb::SimpleNode<TsUnb::FixedUplinkMac, 
		TsUnb::Phy<14224261, 14222623, 39, 39, TsUnb::TsUnb_UPG3, 3, TsUnb::RadioBurst <2,2> >,
		Trx::Rfm69hw<ArduinoTsUnb<CS_PIN, 48, XTAL_PPM_OFFSET>, true, 10, TsUnb::RadioBurst <2,2> >, false>;

//! RFM69hw in EU2 configuration with Low Latency Uplink Pattern Group 3, no Sync Burst
template<uint8_t CS_PIN = 8>
using TsUnb_EU2_LowLatency_Rfm69hw_t = TsUnb::SimpleNode<TsUnb::FixedUplinkMac, 
		TsUnb::Phy<14215168, 14202061, 468, 39, TsUnb::TsUnb_UPG3, 3, TsUnb::RadioBurst <2,2> >,
		Trx::Rfm69hw<ArduinoTsUnb<CS_PIN, 48, XTAL_PPM_OFFSET>, true, 10, TsUnb::RadioBurst <2,2> >, false>;






};	// namespace Arduino
};	// namespace TsUnbLib
#endif // ARDUINO_TSUNB_TEMPLATES_H_

