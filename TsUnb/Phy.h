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
 * @brief	ETSI TS 103 357 TS-UNB Physical layer for the uplink
 *
 * @authors	Clemens Neumueller, Joerg Robert
 * @file	Phy.h
 *
 */



#ifndef TSUNB_PHY_H_
#define TSUNB_PHY_H_

#include <stdint.h>

// Special memory handling for AVR micro controllers (e.g. for Arduino)
#ifdef __AVR_ARCH__
#include <avr/pgmspace.h>
#endif

#include "../Utils/BitAccess.h"

namespace TsUnbLib {
namespace TsUnb {


//! Minimum length of a PSDU in bytes
#define TSUNBPHY_MIN_PSDU_LENGTH	20

//! Maximum length of a PSDU in bytes
#define TSUNBPHY_MAX_PSDU_LENGTH	255


//! Additional overhead introduced by the PHY
#define TSUNBPHY_OVERHEAD			4

//! Position of header CRC
#define TSUNBPHY_HEADER_CRC_POS		0

//! Position of payload CRC
#define TSUNBPHY_PAYLOAD_CRC_POS	1

//! Position of PSI field
#define TSUNBPHY_PAYLOAD_PSI_POS	2

//! Start of data
#define TSUNBPHY_PAYLOAD_DATA_POS	3

//! Polynomial of the 8 bit CRC
#define TSUNBPHY_CRC8_POLY			0x9B

//! Initialization vector of the 8 bit CRC
#define TSUNBPHY_CRC8_INIT			0xFF

//! Polynomial of the 2 bit CRC
#define TSUNBPHY_CRC2_POLY			0x03

//! Initialization vector of the 2 bit CRC
#define TSUNBPHY_CRC2_INIT			0x03

//! Inverse rate of convolutional code
#define TSUNBPHY_CONV_RATE 			3

//! Degree of the convolutional code's polynomials
#define TSUNBPHY_CONV_POLY_M		6

//! Polynomial G1 of the convolutional code g(x)=x^6 + x^5 + x^3 + x^2 + 1
#define TSUNBPHY_CONV_POLY_G1		0x5B

//! Polynomial G2 of the convolutional code g(x)=x^6 + x^4 + x + 1
#define TSUNBPHY_CONV_POLY_G2		0x65

//! Polynomial G3 of the convolutional code g(x)=x^6 + x^4 + x^3 + x^2 + x + 1
#define TSUNBPHY_CONV_POLY_G3		0x7D

//! Number of core bursts
#define TSUNBPHY_NUM_CORE_BURSTS	24

//! Number of bits in core interleaver
#define TSUNBPHY_NUM_BITS_CORE_ILV	288

//! Number of shift bits for the interleaver
#define TSUNBPHY_NUM_BITS_SHIFT		48

//! Polynomial for the Galois-LFSR used to determine the hop time and frequency offsets for the extension frames
#define TSUNBPHY_EXT_FRAME_POLY		0xB4F3

//! Number of TSMA patterns in UPG1 and UPG2
#define TSUNBPHY_UNB_NUM_P			8

//! Extension frame spacing constant for the UPG1
#define TSUNBPHY_TIME_SPACING_UPG1	337


//! Extension frame spacing constant for the UPG2
#define TSUNBPHY_TIME_SPACING_UPG2	337

//! Extension frame spacing constant for the UPG3
#define TSUNBPHY_TIME_SPACING_UPG3	66

/**
 * @brief Length of the periodic TSMA pattern cycle
 */
#define TSUNBPHY_TSMA_PATTERN_CYCLE  15


//! ENUM for the different uplink pattern groups
enum TsUnbUPGMode {
	TsUnb_UPG1,		//!< Uplink pattern group 1
	TsUnb_UPG2,		//!< Uplink pattern group 2
	TsUnb_UPG3 		//!< Uplink pattern group 3
};


/**
 * @brief Implementation of ETSI TS 103 357 TS-UNB Uplink Physical Layer (PHY)
 *
 * This class implements the ETSI TS 103 357 TS-UNB Uplink Physical Layer (PHY).
 * The support of different modes is achieved using template parameters.
 *
 * The template parameter CHAN_A is the frequency of the channel A as transmitter register setting.
 * The template parameter CHAN_B is the frequency of the channel B as transmitter register setting.
 * CHAN_A and CHAN_B are identical if only a single channel is used.
 *
 * The template parameter B_c is the actual carrier spacing step size as transmitter register setting.
 *
 * The template parameter B_c0 is the frequency offset step as transmitter register setting.
 *
 * The template parameter TSUNB_UPG is the uplink pattern group according to the enum TsUnbUPGMode.
 *
 *
 * The template parameter n_co is the a parameter for the additional frequency offset (6.4.7.1.5).
 * It can take the values 3 (crystal tolerance >= 10ppm) or 11 (crytal tolerance < 10ppm).
 *
 * The template class RadioBurst_T defines a radio burst data structure. The actual implementation has to
 * offer the methods: uint16_t getBurstLength(void), uint8_t* getBurst(void), uint16_t get_channel(void).
 *
 */
template <uint32_t CHAN_A = 14224261, uint32_t CHAN_B = 14222623,
		uint32_t B_c = 39, uint32_t B_c0 = 39, TsUnbUPGMode TSUNB_UPG = TsUnb_UPG1,
		uint8_t n_co = 3, class RadioBurst_T = TsUnb::RadioBurst <> >
class Phy {

public:
	//! Type of the radio bursts
	typedef RadioBurst_T RadioBurst_t;

	/**
	 * @brief Constructor, which is currently not used
	 */
	Phy() {
	};

	/**
	 * @brief Destructor, which is currently not used
	 */
	~Phy() {
	};


	/**
	 * @brief Encoding of TS-UNB radio burst
	 *
	 * This method encodes the data for a TS-UNB radio burst. It takes the MPDU data
	 * and creates the necessary radio bursts including convolutional encoding, modulation,
	 * midamble insertion and MSK precoding. The bits in each radio burst can be used as
	 * input for MSK modulators.
	 *
	 * @param	RadioBursts	Pointer to already allocated array for writing the output burst. The length of the array can be calculated using the numRadioBursts() method.
	 * @param	MPDU		Pointer to MPDU input data
	 * @param	MPDU_Length	MPDU length in bytes
	 * @param	TSMAPattern	TSMA Pattern for the modulation, caution: index starts with 0 (standard starts with 1)
	 * @param	MMODE       Used MacMode
	 *
	 * @return	Frequency f_0 of the radio bursts in register setting. Returns 0 in case of error.
	 */
	uint32_t encode(RadioBurst_T* const RadioBursts, const uint8_t* const MPDU,
			const uint16_t MPDU_Length,	const uint8_t TSMAPattern = 0, const uint8_t MMODE = 0) {

		if (MPDU_Length > TSUNBPHY_MAX_PSDU_LENGTH)
			return 0;

		const uint16_t numBursts = numRadioBursts(MPDU_Length);


		/*
		 * Copy data to local buffer and set fields
		 */
		uint8_t PhyPayload[numBursts];
		for (uint16_t i = 0; i < MPDU_Length; ++i) {
			PhyPayload[TSUNBPHY_PAYLOAD_DATA_POS + i] = MPDU[i];
		}
		PhyPayload[TSUNBPHY_PAYLOAD_PSI_POS] = (uint8_t)MPDU_Length;


		// The MMODE is copied at the end of the payload data for CRC calculation.
		// It is copied to the correct position if stuffing is required later.
		PhyPayload[TSUNBPHY_PAYLOAD_DATA_POS + MPDU_Length] = (MMODE & 0x03) << 6;

		// Calculate the payload CRC
		PhyPayload[TSUNBPHY_PAYLOAD_CRC_POS] =
				calcCRC8(&PhyPayload[TSUNBPHY_PAYLOAD_DATA_POS], MPDU_Length * 8 + 2);


		// Stuff in case of short PSDU and bring the MMODE to the right position
		if (MPDU_Length < TSUNBPHY_MIN_PSDU_LENGTH) {
			// We have to stuff the data
			for (uint16_t i = MPDU_Length;i < TSUNBPHY_MIN_PSDU_LENGTH; ++i) {
				PhyPayload[TSUNBPHY_PAYLOAD_DATA_POS + i] = 0;
			}

			// Finally copy the MMODE to the right position at the end of the stuffing data
			PhyPayload[TSUNBPHY_PAYLOAD_DATA_POS + TSUNBPHY_MIN_PSDU_LENGTH] = (MMODE & 0x03) << 6;
		}

		// Calculate the header CRC
		PhyPayload[TSUNBPHY_HEADER_CRC_POS] = calcCRC8(&PhyPayload[TSUNBPHY_PAYLOAD_CRC_POS], 16);


		//! Payload CRC, later required as LFSR seed
		const uint8_t payloadCrc = PhyPayload[TSUNBPHY_PAYLOAD_CRC_POS];

		//! LFSR seed for burst positions in case of extension frame
		uint16_t lfsrSeed =
				0x8000u | (uint16_t) PhyPayload[TSUNBPHY_HEADER_CRC_POS] <<	8 | payloadCrc;


		/*
		 * Whiten the data
		 */
		whitenData(PhyPayload, numBursts);

		/*
		 * Do the convolutional encoding.
		 * The coding is quite complex due to the bit shift in the interleaver.
		 * To minimize memory consumption the interleaving and the convolutional
		 * encoding are done in a single step.
		 */
		//! Number of payload bits
		const uint16_t payloadBits = numBursts * 8;

		//! Register for convolutional encoder
		uint8_t convReg = 0;

		// The code termination is achieved by means of the zero bits in the MMODE field.
		// Therefore we have to restore our tail bits that we lost during the whitening.
		PhyPayload[numBursts - 1] &= 0xC0;



		// To avoid an additional memory for the cylic shift of the interleaver we
		// use some kind of tail biting covolutional code. For this purpose we
		// first have to bring the code to the correct register state.
		for (uint16_t inBitIdx = 0; inBitIdx < 8; ++inBitIdx) {
			convReg <<= 1;

			const uint16_t shiftBitIdx = payloadBits + inBitIdx - (TSUNBPHY_NUM_BITS_SHIFT / 3 + 8);
			if (readBit(shiftBitIdx, PhyPayload))
				convReg |= 1;
		}

		// Here we to the actual convolutional encoding
		// We normally would have to reset the convolution encoder at the end
		// of the payload data before starting to encode the initial header data,
		// which is caused by the cyclic shift. However, the MMODE only takes 2 bits
		// and we have 6 zeros in this byte. This actually terminates the code and
		// we start again from zero.
		for (uint16_t inBitIdx = 0; inBitIdx < payloadBits;	++inBitIdx) {

			// Consider bit shift due to the interleaver and correct if necessary
			int16_t shiftBitIdx = (int16_t) inBitIdx - TSUNBPHY_NUM_BITS_SHIFT / 3;
			if (shiftBitIdx < 0)
				shiftBitIdx += payloadBits;

			convReg <<= 1;
			if (readBit(shiftBitIdx, PhyPayload))
				convReg |= 1;


			// Get the one output from each branch of the convolutional encoder
			uint8_t outBits[3];
			outBits[0] = convEncode_parity(TSUNBPHY_CONV_POLY_G1 & convReg);
			outBits[1] = convEncode_parity(TSUNBPHY_CONV_POLY_G2 & convReg);
			outBits[2] = convEncode_parity(TSUNBPHY_CONV_POLY_G3 & convReg);

			// And write the bits directly onto their correct positions on the radio bursts
			for (uint8_t i = 0; i < 3; ++i) {
				uint16_t outBitIdx = inBitIdx * 3 + i;
				const uint16_t burstIdx = getRadioBurstIdx(outBitIdx, numBursts);
				RadioBursts[burstIdx].writeSubPacketBit(outBits[i], burstIdx);
			}
		}


		/*
		 * Add the midamble and do the differntial MSK encoding
		 */
		for (uint16_t burstIdx = 0; burstIdx < numBursts; ++burstIdx) {
			RadioBursts[burstIdx].addMidamble(burstIdx);
			RadioBursts[burstIdx].differentialMSKEncoding();
		}


		/*
		 * Add the TSMA pattern
		 */
		if (TSUNB_UPG == TsUnb_UPG3) {
			addTsmaPattern(numBursts, 0, lfsrSeed, RadioBursts);
		}
		else {
			addTsmaPattern(numBursts, TSMAPattern % TSUNBPHY_UNB_NUM_P,
					lfsrSeed, RadioBursts);
		}

		/*
		 * Return frequency f_0
		 */
		return calcFreqReg(payloadCrc);
	}


	/** 
	 * Returns number of radio bursts as function of the payloadLength
	 * Return 0 in case of error
	 */ 
	uint16_t numRadioBursts(const uint16_t MPDU_Length) const {
		if (MPDU_Length > TSUNBPHY_MAX_PSDU_LENGTH)
			return 0;

		if (MPDU_Length < TSUNBPHY_MIN_PSDU_LENGTH)
			return TSUNBPHY_MIN_PSDU_LENGTH + TSUNBPHY_OVERHEAD;
		else
			return MPDU_Length + TSUNBPHY_OVERHEAD;

	}

	/** 
	 * @brief Encoding of TS-UNB Sync Burst
	 * 
	 * @param	RadioBurst 			Pointer to existing radio burst to output encoded data
	 * @param	TSMAPattern			TSMA Pattern for the modulation, caution: index starts with 0 (standard starts with 1)
	 * @param	LSB_ShortAddress	
	 */ 
	void encodeSyncBurst(RadioBurst_T* const RadioBurst, const uint8_t TSMAPattern, const uint8_t LSB_ShortAddress) {
		
		// Data array for the Sync Burst
		// Note: The TSMAPattern rages from 0 (p=1) to 7 (p=8)
		uint8_t syncBurst[5] = {0x33, 0x3d, (uint8_t)(0x30 + (TSMAPattern & 0x07)), LSB_ShortAddress, 0};
		switch (TSUNB_UPG) {
		case TsUnb_UPG1:
			break;
		case TsUnb_UPG2:
			syncBurst[4] |= 0x40;
			break;
		case TsUnb_UPG3:
			syncBurst[4] |= 0x80;
			break;
		}
	
		//! CRC register, initialized with init state
		uint8_t crc2_reg = TSUNBPHY_CRC2_INIT;

		for (uint16_t bitIdx = 20; bitIdx <= 33; ++bitIdx) {
			// Get msb
			uint8_t msb = 0;
			if (crc2_reg & 0x03)
				msb = 1;

			// Check if input bit set, if yes xor MSB
			if (readBit(bitIdx, syncBurst))
				msb ^= 1;

			crc2_reg <<= 1;

			if (msb)
				crc2_reg ^= TSUNBPHY_CRC2_POLY;
		}
		
		RadioBurst->writeBitIdx((crc2_reg >> 1) & 0x01, 34);
		RadioBurst->writeBitIdx((crc2_reg >> 0) & 0x01, 35);
		
		for (uint8_t bitIdx = 0; bitIdx < RadioBurst_T::BURST_LENGTH; ++bitIdx) {
			const uint8_t bit = readBit(bitIdx, syncBurst);
			RadioBurst->writeBitIdx(bit, bitIdx);
		}

		RadioBurst->differentialMSKEncoding();
		RadioBurst->setCarrierOffset((uint16_t)24 * B_c);
		
		switch (TSUNB_UPG) {
		case TsUnb_UPG1:
		case TsUnb_UPG2:
			RadioBurst->set_T_RB(337);
			break;
		case TsUnb_UPG3:
			RadioBurst->set_T_RB(66);
			break;
		}		

		return;
	}
	
	
	/**
	 * @brief	Get the current TSMA pattern
	 *
	 * @param	counter		Input counter variable
	 * 
	 * @return	Current TSMA pattern
	 * 
	 */
	uint8_t getTsmaPattern(const uint32_t counter) const {
		/**
         * @brief Sequence of the uplink TSMA patterns
         */
        const
#ifdef __AVR_ARCH__
        static PROGMEM
#endif
        uint8_t TS_UNB_TSMA_PATTERN_ORDER [TSUNBPHY_TSMA_PATTERN_CYCLE] = {
		        0, 1, 2, 3, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 5};

#ifdef __AVR_ARCH__
		return (uint8_t)pgm_read_byte(&(TS_UNB_TSMA_PATTERN_ORDER[counter % TSUNBPHY_TSMA_PATTERN_CYCLE]));
#else
		return TS_UNB_TSMA_PATTERN_ORDER[counter % TSUNBPHY_TSMA_PATTERN_CYCLE];
#endif
	}


private:
	/**
	 * @brief Calculation of CRC8
	 *
	 * This method implements a CRC8 calculation
	 *
	 * @param	inputBytes		Pointer to byte input memory
	 * @param	numInputBits	Number of input bits for the calcuation of the CRC
	 *
	 * @return	Calculated CRC8
	 *
	 */
	uint8_t calcCRC8(const uint8_t* const inputBytes, const uint16_t numInputBits) const {
		//! CRC register, initialized with init state
		uint8_t crc8_reg = TSUNBPHY_CRC8_INIT;

		for (uint16_t bitIdx = 0; bitIdx < numInputBits; ++bitIdx) {
			// Get msb
			uint8_t msb = 0;
			if (crc8_reg & 0x80)
				msb = 1;

			// Check if input bit set, if yes xor MSB
			if (readBit(bitIdx, inputBytes))
				msb ^= 1;

			crc8_reg <<= 1;

			if (msb)
				crc8_reg ^= TSUNBPHY_CRC8_POLY;
		}

		return crc8_reg;
	}

	/**
	 * @brief Whiten the transmit signal
	 *
	 * This method whitens the transmit signal
	 *
	 * @param	inputBytes		Pointer to input bytes
	 * @param	numBytes		Number of input bytes
	 *
	 */
	void whitenData(uint8_t* const inputBytes, const uint16_t numBytes) const {
		uint16_t reg = 0x1FF;

		for (uint16_t byte = 0; byte < numBytes; ++byte) {
			for (uint8_t bit = 0; bit < 8; ++bit) {
				reg <<= 1;
				reg ^= 0x1 & (reg >> 9 ^ reg >> 4);
			}
			inputBytes[byte] ^= (uint8_t) reg;
		}
	}


	/**
	 * @brief	Return parity of the input data
	 *
	 * @param	reg		Register value
	 *
	 * @return	Parity bit, i.e. 0 or 1
	 *
	 */
	uint8_t convEncode_parity(uint8_t reg) const {
		reg ^= reg >> 4;
		reg ^= reg >> 2;
		reg ^= reg >> 1;

		return reg & 1;
	}

	/**
	 * @brief Calculate LFSR for TSMA pattern
	 *
	 * @param	seed	Input seed (i.e. current LFSR state)
	 *
	 * @return	New LFSR state
	 *
	 */
	uint16_t tsmaLfsr(uint16_t seed) const {
		uint16_t lsb = seed & 1;
		seed >>= 1;

		if (lsb)
			seed ^= TSUNBPHY_EXT_FRAME_POLY;

		return seed;
	}


	uint16_t getRadioBurstIdx(const uint16_t bitIdx, const uint16_t numPackets) const {
		if (bitIdx < TSUNBPHY_NUM_BITS_CORE_ILV) {	// Core frame (first 288 bit)
			return bitIdx % TSUNBPHY_NUM_CORE_BURSTS;
		}
		else {		// Extension frame
			uint16_t groupIdx = bitIdx - TSUNBPHY_NUM_BITS_CORE_ILV;
			const uint16_t groupLen =
					numPackets - (TSUNBPHY_NUM_CORE_BURSTS >> 1);

			const uint16_t group = groupIdx / groupLen;
			groupIdx -= group * groupLen;

			if (groupIdx < (TSUNBPHY_NUM_CORE_BURSTS >> 1))	// First 12 bit
				return (groupIdx << 1) + (group & 1);
			else
				return groupIdx + (TSUNBPHY_NUM_CORE_BURSTS >> 1);
		}
	}


	/**
	 * @brief Add the TSMA pattern to the radio bursts
	 *
	 * This method adds the TSMA pattern to the radio bursts, i.e. configure
	 * the T_RB and the C_RB.
	 *
	 * @param	numBursts		Number of radio bursts
	 * @param	TSMAPattern		The selected TSMA pattern
	 * @param	lfsrSeed		The seed of the LFSR generator (required for extension frames)
	 * @param	RadioBursts		Pointer to radio burst memory
	 *
	 */
	void addTsmaPattern(const uint16_t numBursts, const uint8_t TSMAPattern,
			uint16_t lfsrSeed, RadioBurst_T* const RadioBursts) const {

		for (uint16_t i = 0; i < TSUNBPHY_NUM_CORE_BURSTS; ++i) {
			RadioBursts[i].setCarrierOffset((uint16_t)get_C_RB(TSMAPattern, i) * B_c);

			if (i != TSUNBPHY_NUM_CORE_BURSTS - 1)
				RadioBursts[i].set_T_RB(get_T_RB(TSMAPattern, i));
		}


		for (uint16_t i = TSUNBPHY_NUM_CORE_BURSTS; i < numBursts; ++i) {
			lfsrSeed = tsmaLfsr(lfsrSeed);
			RadioBursts[i].setCarrierOffset(((lfsrSeed >> 8) % 25) * B_c);

			uint16_t extentionFrameTimeSpacing;

			switch (TSUNB_UPG) {
			case TsUnb_UPG1:
				extentionFrameTimeSpacing =	TSUNBPHY_TIME_SPACING_UPG1;
				break;

			case TsUnb_UPG2:
				extentionFrameTimeSpacing =	TSUNBPHY_TIME_SPACING_UPG2;
				break;

			case TsUnb_UPG3:
				extentionFrameTimeSpacing =	TSUNBPHY_TIME_SPACING_UPG3;
				break;
			}

			RadioBursts[i -	1].set_T_RB(extentionFrameTimeSpacing +	(lfsrSeed % 128));
		}

		RadioBursts[numBursts - 1].set_T_RB(0);
	}


	/**
	 * @brief Calculates the frequency register setting for the transmit frequency f_0
	 *
	 * This method calculates the register setting to achieve the frequency f_0 in the
	 * transmitter.
	 *
	 * @param	payloadCrc		Payload CRC as the frequency depends on the CRC
	 *
	 * @return	Transmitter frequency register value for the frequency f_0
	 *
	 */
	uint32_t calcFreqReg(const uint8_t payloadCrc) const {
		// Calculate frequency according to 6.4.7.1.1
		uint32_t f_c;

		if ((payloadCrc & 0x80u) == 0)
			f_c = CHAN_A;
		else
			f_c = CHAN_B;

		// The only valid values for nco are 3 and 11
		const int8_t v_c0 = (int8_t) (payloadCrc & 0x7Fu);
		const int32_t C_RF = (v_c0 % n_co) - (n_co >> 1);
		const uint32_t f_0 = f_c - 12 * B_c + C_RF * B_c0;

		return f_0;
	}


	/**
	 * @brief Returns radio burst time set of uplink T_rb according to 6.4.7.1.6.1
	 *
	 * @param	TSMAPattern		Used TSMA pattern
	 * @param	burstIdx		Number of the burst
	 *
	 * @return	Radio burst time set T_RB
	 *
	 */
	uint16_t get_T_RB(const uint8_t TSMAPattern, const uint16_t burstIdx) const {
		// Just to be sure
		if (burstIdx >= 24)
			return 0;
		if (TSMAPattern >= 8)
			return 0;

		switch (TSUNB_UPG) {
		case TsUnb_UPG1:{
			// We use this uplink pattern
			//{{330, 387, 388, 330, 387, 354, 330, 387, 356, 330, 387, 432, 330, 387, 352, 330, 387, 467, 330, 387, 620, 330, 387},
			//{330, 387, 435, 330, 387, 409, 330, 387, 398, 330, 387, 370, 330, 387, 361, 330, 387, 472, 330, 387, 522, 330, 387},
			//{330, 387, 356, 330, 387, 439, 330, 387, 413, 330, 387, 352, 330, 387, 485, 330, 387, 397, 330, 387, 444, 330, 387},
			//{330, 387, 352, 330, 387, 382, 330, 387, 381, 330, 387, 365, 330, 387, 595, 330, 387, 604, 330, 387, 352, 330, 387},
			//{330, 387, 380, 330, 387, 634, 330, 387, 360, 330, 387, 393, 330, 387, 352, 330, 387, 373, 330, 387, 490, 330, 387},
			//{330, 387, 364, 330, 387, 375, 330, 387, 474, 330, 387, 355, 330, 387, 478, 330, 387, 464, 330, 387, 513, 330, 387},
			//{330, 387, 472, 330, 387, 546, 330, 387, 501, 330, 387, 356, 330, 387, 359, 330, 387, 359, 330, 387, 364, 330, 387},
			//{330, 387, 391, 330, 387, 468, 330, 387, 512, 330, 387, 543, 330, 387, 354, 330, 387, 391, 330, 387, 368, 330, 387}}

			// For the TSMA uplink patterns only every 3rd position is different
			const uint8_t burstIdxMod = ((uint8_t)burstIdx) % 3;
			switch (burstIdxMod) {
			case 0:
				return 330;
			case 1:
				return 387;
			case 2:
				// Here we store the coefficients for every third position	
				typedef uint16_t ETSI_TS_UNB_TRB_t[(TSUNBPHY_NUM_CORE_BURSTS - 1) / 3];
				//! Time offset sequence for the non-identical delays of the TSMA uplink pattern of UPG1
				const
#ifdef __AVR_ARCH__
						static PROGMEM
#endif
						ETSI_TS_UNB_TRB_t ETSI_TS_UNB_TRB_UPG1[TSUNBPHY_UNB_NUM_P] = {
								{388, 354, 356, 432, 352, 467, 620},
								{435, 409, 398, 370, 361, 472, 522},
								{356, 439, 413, 352, 485, 397, 444},
								{352, 382, 381, 365, 595, 604, 352},
								{380, 634, 360, 393, 352, 373, 490},
								{364, 375, 474, 355, 478, 464, 513},
								{472, 546, 501, 356, 359, 359, 364},
								{391, 468, 512, 543, 354, 391, 368}};

#ifdef __AVR_ARCH__
				return (uint16_t)pgm_read_word(&(ETSI_TS_UNB_TRB_UPG1[TSMAPattern][(uint8_t)burstIdx / 3]));
#else
				return ETSI_TS_UNB_TRB_UPG1[TSMAPattern][burstIdx / 3];
#endif
			}
		} break; 	// TsUnb_UPG1

		case TsUnb_UPG2: {
			// We use this uplink pattern
			//{{373, 319, 545, 373, 319, 443, 373, 319, 349, 373, 319, 454, 373, 319, 578, 373, 319, 436, 373, 319, 398, 373, 319},
			//{373, 319, 371, 373, 319, 410, 373, 319, 363, 373, 319, 354, 373, 319, 379, 373, 319, 657, 373, 319, 376, 373, 319},
			//{373, 319, 414, 373, 319, 502, 373, 319, 433, 373, 319, 540, 373, 319, 428, 373, 319, 467, 373, 319, 409, 373, 319},
			//{373, 319, 396, 373, 319, 516, 373, 319, 631, 373, 319, 471, 373, 319, 457, 373, 319, 416, 373, 319, 354, 373, 319},
			//{373, 319, 655, 373, 319, 416, 373, 319, 367, 373, 319, 400, 373, 319, 415, 373, 319, 342, 373, 319, 560, 373, 319},
			//{373, 319, 370, 373, 319, 451, 373, 319, 465, 373, 319, 593, 373, 319, 545, 373, 319, 380, 373, 319, 365, 373, 319},
			//{373, 319, 393, 373, 319, 374, 373, 319, 344, 373, 319, 353, 373, 319, 620, 373, 319, 503, 373, 319, 546, 373, 319},
			//{373, 319, 367, 373, 319, 346, 373, 319, 584, 373, 319, 579, 373, 319, 519, 373, 319, 351, 373, 319, 486, 373, 319}}
			const uint8_t burstIdxMod = ((uint8_t)burstIdx) % 3;
			switch (burstIdxMod) {
			case 0:
				return 373;
			case 1:
				return 319;
			case 2:
				// Here we store the coefficients for every third position	
				typedef uint16_t ETSI_TS_UNB_TRB_t[(TSUNBPHY_NUM_CORE_BURSTS - 1) / 3];
				//! Time offset sequence for the non-identical delays of the TSMA uplink pattern of UPG2
				const
#ifdef __AVR_ARCH__
						static PROGMEM
#endif
						ETSI_TS_UNB_TRB_t ETSI_TS_UNB_TRB_UPG2[TSUNBPHY_UNB_NUM_P] = {
								{545, 443, 349, 454, 578, 436, 398},
								{371, 410, 363, 354, 379, 657, 376},
								{414, 502, 433, 540, 428, 467, 409},
								{396, 516, 631, 471, 457, 416, 354},
								{655, 416, 367, 400, 415, 342, 560},
								{370, 451, 465, 593, 545, 380, 365},
								{393, 374, 344, 353, 620, 503, 546},
								{367, 346, 584, 579, 519, 351, 486}};

#ifdef __AVR_ARCH__
				return (uint16_t)pgm_read_word(&(ETSI_TS_UNB_TRB_UPG2[TSMAPattern][burstIdx / 3]));
#else
				return ETSI_TS_UNB_TRB_UPG2[TSMAPattern][burstIdx / 3];
#endif
			}
		} break;	// TsUnb_UPG2

		case TsUnb_UPG3: {
			typedef uint8_t ETSI_TS_UNB_TRB_t[TSUNBPHY_NUM_CORE_BURSTS - 1];
			//! Time offset sequence of the TSMA uplink pattern of UPG3
			const
#ifdef __AVR_ARCH__
					static PROGMEM
#endif
					ETSI_TS_UNB_TRB_t ETSI_TS_UNB_TRB_UPG3[1] = {
							{66, 66, 66, 66, 66, 66, 66, 66, 66, 123, 66, 66, 66, 66, 60, 66, 66, 198, 66, 66, 255, 66, 66}
			};

#ifdef __AVR_ARCH__
			return (uint16_t)pgm_read_byte(&(ETSI_TS_UNB_TRB_UPG3[0][burstIdx]));
#else
			return ETSI_TS_UNB_TRB_UPG3[0][burstIdx];
#endif
		} break;	// TsUnb_UPG3
		}

		return 0;	// We should normally never reach this point
	}



	/**
	 * @brief Returns radio burst carrier set of uplink C_RB according to 6.4.7.1.6.1
	 *
	 * @param	TSMAPattern		Used TSMA pattern
	 * @param	burstIdx		Number of the burst
	 *
	 * @return	Radio burst time set C_RB
	 *
	 */
	uint8_t get_C_RB(const uint8_t TSMAPattern, const uint16_t burstIdx) const {
		typedef uint8_t ETSI_TS_UNB_CRB_t[TSUNBPHY_NUM_CORE_BURSTS];

		switch (TSUNB_UPG) {
		case TsUnb_UPG1: {
			//! Carrier sequence of the TSMA uplink pattern of UPG1
			const
#ifdef __AVR_ARCH__
		static PROGMEM
#endif
		ETSI_TS_UNB_CRB_t ETSI_TS_UNB_CRB_UPG1[TSUNBPHY_UNB_NUM_P] = {
				{ 5, 21, 13,  6, 22, 14,  1, 17,  9,  0, 16,  8,  7, 23, 15,  4, 20, 12,  3, 19, 11,  2, 18, 10},
				{ 4, 20, 12,  1, 17,  9,  0, 16,  8,  6, 22, 14,  7, 23, 15,  2, 18, 10,  5, 21, 13,  3, 19, 11},
				{ 4, 20, 12,  3, 19, 11,  6, 22, 14,  7, 23, 15,  0, 16,  8,  5, 21, 13,  2, 18, 10,  1, 17,  9},
				{ 6, 22, 14,  2, 18, 10,  7, 23, 15,  0, 16,  8,  1, 17,  9,  4, 20, 12,  5, 21, 13,  3, 19, 11},
				{ 7, 23, 15,  4, 20, 12,  3, 19, 11,  2, 18, 10,  6, 22, 14,  0, 16,  8,  1, 17,  9,  5, 21, 13},
				{ 3, 19, 11,  6, 22, 14,  2, 18, 10,  0, 16,  8,  7, 23, 15,  1, 17,  9,  4, 20, 12,  5, 21, 13},
				{ 3, 19, 11,  1, 17,  9,  5, 21, 13,  7, 23, 15,  0, 16,  8,  2, 18, 10,  6, 22, 14,  4, 20, 12},
				{ 0, 16,  8,  6, 22, 14,  3, 19, 11,  2, 18, 10,  4, 20, 12,  7, 23, 15,  5, 21, 13,  1, 17,  9}};

#ifdef __AVR_ARCH__
			return (uint8_t)pgm_read_byte(&(ETSI_TS_UNB_CRB_UPG1[TSMAPattern][burstIdx]));
#else
			return ETSI_TS_UNB_CRB_UPG1[TSMAPattern][burstIdx];
#endif
		} break; 	// TsUnb_UPG1

		case TsUnb_UPG2: {
			//! Carrier sequence of the TSMA uplink pattern of UPG2
			const
#ifdef __AVR_ARCH__
		static PROGMEM
#endif
		ETSI_TS_UNB_CRB_t ETSI_TS_UNB_CRB_UPG2[TSUNBPHY_UNB_NUM_P] = {
				{ 4, 20, 12,  0, 16,  8,  3, 19, 11,  5, 21, 13,  1, 17,  9,  7, 23, 15,  2, 18, 10,  6, 22, 14},
				{ 3, 19, 11,  7, 23, 15,  2, 18, 10,  5, 21, 13,  4, 20, 12,  0, 16,  8,  1, 17,  9,  6, 22, 14},
				{ 6, 22, 14,  0, 16,  8,  1, 17,  9,  4, 20, 12,  3, 19, 11,  5, 21, 13,  2, 18, 10,  7, 23, 15},
				{ 3, 19, 11,  1, 17,  9,  4, 20, 12,  5, 21, 13,  2, 18, 10,  7, 23, 15,  6, 22, 14,  0, 16,  8},
				{ 5, 21, 13,  2, 18, 10,  0, 16,  8,  6, 22, 14,  7, 23, 15,  1, 17,  9,  4, 20, 12,  3, 19, 11},
				{ 1, 17,  9,  3, 19, 11,  4, 20, 12,  6, 22, 14,  7, 23, 15,  5, 21, 13,  2, 18, 10,  0, 16,  8},
				{ 5, 21, 13,  1, 17,  9,  2, 18, 10,  4, 20, 12,  3, 19, 11,  0, 16,  8,  6, 22, 14,  7, 23, 15},
				{ 3, 19, 11,  6, 22, 14,  5, 21, 13,  1, 17,  9,  7, 23, 15,  2, 18, 10,  0, 16,  8,  4, 20, 12}};

#ifdef __AVR_ARCH__
			return (uint8_t)pgm_read_byte(&(ETSI_TS_UNB_CRB_UPG2[TSMAPattern][burstIdx]));
#else
			return ETSI_TS_UNB_CRB_UPG2[TSMAPattern][burstIdx];
#endif
		} break;	// TsUnb_UPG2

		case TsUnb_UPG3: {
			//! Carrier sequence of the TSMA uplink pattern of UPG3
			const
#ifdef __AVR_ARCH__
		static PROGMEM
#endif
		ETSI_TS_UNB_CRB_t ETSI_TS_UNB_CRB_UPG3[1] = {
				{ 1, 5, 4, 3, 2, 17, 21, 20, 19, 18, 9, 13, 12, 11, 10, 6, 0, 7, 22, 16, 23, 14, 8, 15}};

#ifdef __AVR_ARCH__
			return (uint8_t)pgm_read_byte(&(ETSI_TS_UNB_CRB_UPG3[0][burstIdx]));
#else
			return ETSI_TS_UNB_CRB_UPG3[0][burstIdx];
#endif
		} break;	// TsUnb_UPG3
		}

		return 0;	// We should normally never reach this point
	}
	

};

};	// namespace TsUnb
};	// namespace TUnbLib

#endif 	// TSUNB_PHY_H_

