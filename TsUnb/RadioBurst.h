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
 * @brief	Contains class for storing Radio Burst data for ETSI TS 103 357 TS-UNB
 *
 * @authors	Joerg Robert
 * @file	TsUnbRadioBurst.h
 *
 * This file implements the basic AES-128 encryption algorithms according to
 * the NIST standard available here: https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf/
 *
 */


#ifndef TSUNB_RADIO_BURST_H_
#define TSUNB_RADIO_BURST_H_

#include <inttypes.h>

#include "../Utils/BitAccess.h"

namespace TsUnbLib {
namespace TsUnb {

//! Length of a TS-UNB Radio burst
#define TSUNB_RADIO_BURST_PAYLOAD_LEN	36

//! Number of TS-UNB core bursts
#define TSUNB_RADIO_BURST_CORE_BURSTS	24

//! Symbol length of a TS-UNB midamble
#define TSUNB_RADIO_BURST_MIDAMBLE_LEN	12

//! Number of data symbols for one TS-UNB radio burst
#define TSUNB_RADIO_BURST_DATA_LEN		24


/**
 * @brief	Implementation of Radio Bursts
 *
 * This template class implements radio burst for ETSI TS 103 357 TS-UNB.
 * Each radio burst is a short chunk of data with a well-defined time and frequency
 * position within each TS-UNB packet. Each burst contains 24 payload symbols and
 * a 12 symbol long mid-amble, which results in a total length of 36 symbols.
 * In order to align the data to bytes a head and tail bits can be added. Furthermore
 * the head bits are required for the potential transmitter ramp-up time and for the
 * MSK decoding based on a matched filter. Therefore, two head bits and two tail bits
 * are recommended.
 *
 * The template parameter HEAD_BITS defines the number of head bits (2 are recommended).
 *
 * The template parameter TAIL_BITS defines the number of tail bits (2 are recommended).
 *
 */
template<uint16_t HEAD_BITS = 2, uint16_t TAIL_BITS = 2>
class RadioBurst {
public:
	//! Total length of radio burst including head and tail bits
	static const uint16_t BURST_LENGTH = HEAD_BITS + TAIL_BITS + TSUNB_RADIO_BURST_PAYLOAD_LEN;

	//! total length of radio burst in bytes
	static const uint16_t BURST_LENGTH_BYTES = (BURST_LENGTH + 7) / 8;

	/**
	 * @brief Constructor
	 *
	 * Initializes radio burst
	 */
	RadioBurst() {
		puncture();
		T_RB = 0;
		for (uint16_t i = 0; i < BURST_LENGTH_BYTES; ++i) {
			data[i] = 0;
		}
	}

	/**
	 * @brief	Get length of radio burst in bits
	 *
	 * @return	Length of radio burst in bits
	 *
	 */
	uint16_t getBurstLength(void) const {
		if (carrierOffset != 0xFFFF)
			return BURST_LENGTH;
		else
			return 0;
	}

	/**
	 * @brief	Get byte length of radio burst
	 *
	 * @return	Length of radio burst in bytes
	 *
	 */
	uint16_t getBurstLengthBytes(void) const {
		if (carrierOffset != 0xFFFF)
			return BURST_LENGTH_BYTES;
		else
			return 0;
	}


	/**
	 * @brief	Get pointer to radio burst data
	 *
	 * @return 	Pointer to radio burst data
	 *
	 */
	const uint8_t* getBurst(void) const {
		return data;
	}


	/**
	 * @brief	Write bit to subpacket
	 *
	 * This method writes a bit to the correct position (includes interleaving) 
	 * in the subpacket and increases its internal index.
	 * Caution: This counter value is also used for T_RB. After writing
	 * the value T_RB the method writeSubPacketBit() can no longer be used.
	 *
	 * @param	bit			Value of the bit, i.e. 0 or 1
	 * @param	burstIdx	Number of this radio burst in radio burst structure
	 *
	 */
	void writeSubPacketBit(const uint8_t bit, const uint16_t burstIdx) {
		uint16_t bitIdx = getSubPkgBitIdx(burstIdx, T_RB);
		writeBit(bit, bitIdx + HEAD_BITS, data);
		T_RB++;
	}

	/**
	 * @brief	Write bit to subpacket at position Idx
	 *
	 * This method writes a bit to a specific position in the 
	 * subpacket. This does NOT include the interleaving.
	 * This method is intended for writing e.g. the Sync Burst bits. 
	 * in the subpacket and increases its internal index.
	 *
	 * @param	bit			Value of the bit, i.e. 0 or 1
	 * @param	bitIdx		Bit position in burst
	 *
	 */
	void writeBitIdx(const uint8_t bit, const uint16_t bitIdx) {
		writeBit(bit, bitIdx + HEAD_BITS, data);
	}
	
	/**
	 * @brief	Set value T_RB
	 *
	 * Set the value T_RB. T_RB is the time between the start of this radio burst and
	 * the start of the following radio burst in multiples of symbols.
	 * Caution: This value is also used for counting the number of written bits.
	 *
	 * @param	t		Time T_RB
	 *
	 */
	void set_T_RB(const uint16_t t) {
		T_RB = t;
	}


	/**
	 * @brief	Get the value T_RB
	 *
	 * Get the value T_RB. T_RB is the time between the start of this radio burst and
	 * the start of the following radio burst in multiples of symbols.
	 *
	 * @return	Time T_RB
	 *
	 */
	uint16_t get_T_RB() const {
		return T_RB;
	}


	/**
	 * @brief	Set the carrier offset
	 *
	 * This method sets the carrier offset between the frequency f_0 and
	 * the radio burst frequency in transmitter register values.
	 *
	 * @param	offset		Frequency offset wrt. f_0 in TX register values
	 *
	 */
	void setCarrierOffset(const uint16_t offset) {
		carrierOffset  = offset;
	}


	/**
	 * @brief	Get the carrier offset for the radio burst
	 *
	 * This method return the carrier offset between the frequency f_0 and
	 * the radio burst frequency in transmitter register values.
	 *
	 * @return	Frequency offset wrt. f_0 in TX register values
	 *
	 */
	uint16_t getCarrierOffset(void) const {
		return carrierOffset;
	}

	/**
	 * @brief	Add the midamble to the radio burst
	 *
	 * This method add the midable to the radio burst. It has to
	 * be called after writing all bits.
	 *
	 * @param	burstIdx	Number of this radio burst in radio burst structure
	 */
	void addMidamble(const uint16_t burstIdx) {
		if (burstIdx < TSUNB_RADIO_BURST_CORE_BURSTS) {
			// Core Bursts
			writeBit(0, 12 + HEAD_BITS, data);
			writeBit(1, 13 + HEAD_BITS, data);
			writeBit(1, 14 + HEAD_BITS, data);
			writeBit(1, 15 + HEAD_BITS, data);

			writeBit(0, 16 + HEAD_BITS, data);
			writeBit(1, 17 + HEAD_BITS, data);
			writeBit(0, 18 + HEAD_BITS, data);
			writeBit(0, 19 + HEAD_BITS, data);

			writeBit(0, 20 + HEAD_BITS, data);
			writeBit(0, 21 + HEAD_BITS, data);
			writeBit(1, 22 + HEAD_BITS, data);
			writeBit(0, 23 + HEAD_BITS, data);
		}
		else {
			// Extension Bursts
			writeBit(0, 12 + HEAD_BITS, data);
			writeBit(1, 13 + HEAD_BITS, data);
			writeBit(0, 14 + HEAD_BITS, data);
			writeBit(0, 15 + HEAD_BITS, data);

			writeBit(1, 16 + HEAD_BITS, data);
			writeBit(1, 17 + HEAD_BITS, data);
			writeBit(1, 18 + HEAD_BITS, data);
			writeBit(1, 19 + HEAD_BITS, data);

			writeBit(1, 20 + HEAD_BITS, data);
			writeBit(0, 21 + HEAD_BITS, data);
			writeBit(1, 22 + HEAD_BITS, data);
			writeBit(0, 23 + HEAD_BITS, data);
		}		
	}

	/**
	 * @brief	Differential encoding
	 *
	 * This method does the differential encoding. It
	 * has to be called after adding the midamble.
	 *
	 */
	void differentialMSKEncoding() {
		uint8_t shiftedData = 0;
		uint8_t firstBitLastByte = 0;
		for(uint16_t s = 0; s < BURST_LENGTH_BYTES; ++s) {
			shiftedData = firstBitLastByte | (data[s] >> 1);
			firstBitLastByte = data[s] << 7;
			data[s] ^= shiftedData;
		}
		// The first and last two bits of each burst are don't cares
		// Setting at least one initial bit to 1 is a workaround
		if (HEAD_BITS > 0)
			data[0] |= 0x80;

	}

	/**
	 * @brief	Puncture burst, i.e. it is not transmitted
	 */
	void puncture() {
		carrierOffset = 0xFFFF;	
	}

private:
	//! Storage for radio burst data
	uint8_t data[BURST_LENGTH_BYTES];

	//! Offset in transmitter register values relative to the system frequency f_0
	uint16_t carrierOffset;

	//! Delay between start time of two radio burst, also internally used for number of written bits calculation in symbol durations
	uint16_t T_RB;

	/**
	 * @brief Function to calculate the sub-packet index for the data interleaving
	 *
	 * @param	burstIdx	Index of the radio burst
	 * @param	bitIdx		Index of the bit within the burst
	 *
	 * @return	Position of the bit within the data[] array
	 */
	uint16_t getSubPkgBitIdx(const uint16_t burstIdx, const uint8_t bitIdx) const {
		if((burstIdx ^ bitIdx) & 1) 		// (pkt even & bit odd) | (pkt odd & bit even)
			return 24 + (bitIdx >> 1);		// 24 offset
		else								// (pkt odd & bit odd) | (pkt even & bit even)
			return 11 - (bitIdx >> 1);		// 11  offset
	}


};

};	// namespace TsUnb
};	// namespace TsUnbLib

#endif	//	TSUNB_RADIO_BURST_H_

