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
 * @brief	Class for generating and transmitting data according to ETSI TS 103 357 TS-UNB
 *
 * @authors	Clemens Neumueller, Joerg Robert
 * @file	SimpleTsUnbNode.h
 *
 */



#ifndef SIMPLE_TS_UNB_NODE_H_
#define SIMPLE_TS_UNB_NODE_H_

#include <inttypes.h>

namespace TsUnbLib {
namespace TsUnb {


/**
 * @brief Template class for the generation and transmission of simple TS-UNB uplink-only data
 *
 * This template class implements a simple uplink-only TS-UNB node. The class has to be initialized
 * using the init() method before the transmission. Furthermore, the parameters of the MAC and TX
 * class have to be configure properly, e.g. setting the EUI-64 address. For this purpose the MAC and
 * TX instances are public.
 *
 * The template parameter MAC defines a class for the MAC encoding. This class has to offer an int16_t init() method,
 * a uin16_t MPDU_Length(payloadLength) to get the length of the MPDU data as function of the payload length,
 * and a uin16_t encode(MPDU, payload, payloadLength) method for the encoding where the return value is the length of the MPDU.
 *
 * The template parameter PHY defines a class for the PHY encoding. This class has to offer a
 * uint16_t numRadioBursts(MPDU_length) method to return the number of radio bursts as function of the MPDU length.
 * In addition, it has to offer a uint32_t encode(RadioBurst_T* const RadioBursts, const uint8_t* const MPDU, const uint16_t MPDU_Length,
 * const uint8_t TSMAPattern) method for generating the data bursts. The return value is the frequency register setting of the
 * transmitter, or 0 in case of an error.
 *
 *
 */
template<typename MAC, typename PHY, typename TX, bool SYNC_BURST = false>
class SimpleNode {
public:

	/**
	 * @brief Constructor, currently not used
	 */
	SimpleNode() {
	}

	/**
	 * @brief Destructor, currently not used
	 */
	~SimpleNode() {
	}

	/**
	 * @brief Initialization method
	 *
	 * This method initializes the node and the underlying Tx and Mac module.
	 * It should be called very early after the start-up of the program
	 * code to bring the transmitter into a defined state.
	 *
	 * @return 	0 in case of success, negative value in case of an initialization error
	 */
	int16_t init() {
		int16_t ret;
		ret = Tx.init();
		if (ret < 0)
			return ret;

		ret = Mac.init();
		return ret;
	}

	/**
	 * @brief Send method to transmit a TS-UNB packet
	 *
	 * This method transmit the requested payload data. It does the MAC and PHY
	 * encoding as well as the transmission of the data using the transmitter.
	 *
	 * @param	payload			Pointer to payload data
	 * @param payloadLength	Length of the payload data in bytes
	 * @param priotry  Uses low prioty uplink pattern if set 6
	 *
	 * @return	Non-negative number in case of success, negative number in case of error
	 */
	int16_t send(const uint8_t* const payload, const uint16_t payloadLength, 
			const uint8_t MPF_value = 0, const bool priority = false) {

		//! MPF field is present if MPF_value != 0
		const bool MPF_present = MPF_value != 0;

		uint16_t MPDU_length = Mac.MPDU_Length(payloadLength, MPF_present);

		if (MPDU_length == 0)
			return -1;

		uint8_t MPDU[MPDU_length];
		Mac.encode(MPDU, payload, payloadLength, MPF_present, MPF_value);

		//! PHY Instance.
		PHY Phy;

		uint16_t numRadioBursts = Phy.numRadioBursts(MPDU_length);
		if (SYNC_BURST == true)
			numRadioBursts++;

		//! Allocate memory for storing radio bursts
		typename PHY::RadioBurst_t Bursts[numRadioBursts];

		// Transmit frequency
		uint32_t freqReg;

		// We have to do a seperate handling if the Sync Burts is used
		if (SYNC_BURST == false) {
			// Normal mode without sync burst
			if (priority)
				freqReg = Phy.encode(Bursts, MPDU, MPDU_length, 6, MAC::MMODE);
			else
				freqReg = Phy.encode(Bursts, MPDU, MPDU_length, Phy.getTsmaPattern(Mac.getCounter()), MAC::MMODE);

			if (freqReg > 0)
				return Tx.transmit(Bursts, numRadioBursts, freqReg);
			else
				return -1;
		}
		else {
			// This is special handling in caes of a sync burst
			numRadioBursts += 1;
			
			// The first data Burst is Burst[1] as Burst[0] is the Sync Burst
			if (priority) {
				freqReg = Phy.encode(&Bursts[1], MPDU, MPDU_length, 6, MAC::MMODE);
				Phy.encodeSyncBurst(&Bursts[0], 6, Mac.getLsbShortAddress());
			}
			else {
				const uint8_t tsmaPattern = Phy.getTsmaPattern(Mac.getCounter());
				freqReg = Phy.encode(&Bursts[1], MPDU, MPDU_length, tsmaPattern, MAC::MMODE);
				Phy.encodeSyncBurst(&Bursts[0], tsmaPattern, Mac.getLsbShortAddress());
			}

		}
		if (freqReg > 0)
			return Tx.transmit(Bursts, numRadioBursts, freqReg);
		else
			return -1;

	}

	//! Instance of TX that is active during the complete lifetime of this class
	TX Tx;

	//! Instance of the MAC that is active during the complete lifetime of this class
	MAC Mac;


};

};	// namespace TsUnb
};	// namespace TsUnbLib

#endif // SIMPLE_TS_UNB_NODE_H_

