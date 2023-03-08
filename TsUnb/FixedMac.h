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
 * @brief Implementation of the TS-UNB fixed uplink MAC
 *
 * @authors	Joerg Robert, Clemens Neumueller
 * @file	TsUnbFixedMac.h
 *
 */


#ifndef TS_UNB_FIXED_UPLINK_MAC_H_
#define TS_UNB_FIXED_UPLINK_MAC_H_

#include <inttypes.h>
#include <stdint.h>

#ifdef __AVR_ARCH__
#include <avr/pgmspace.h>
#endif


#include "../Utils/BitAccess.h"
#include "../Encryption/Aes128.h"


namespace TsUnbLib {
namespace TsUnb {

/**
 * @brief Length of the CMAC initialization vector
 */
#define CMAC_IV_LEN         16
/**
 * @brief Byte indicating the upload data direction
 */
#define DATA_DIRECTION      0x00u
/**
 * @brief Byte indicating the upload data direction with encryption
 */
#define APP_CRYPT_UPLINK    0x10u
/**
 * @brief Block size of the AES encryption in bytes
 */
#define BLOCK_SIZE_AES      16

//! ENUM for the different address modes
enum TsUnbAddressMode {
	TsUnb_Short,	//!< Short address mode
	TsUnb_Long		//!< Long address mode
};


/**
 * @brief	Implementation of TS-UNB Fixed Uplink MAC
 *
 * This class implements the TS-UNB Fixed Uplink MAC as defined in 6.3.2
 *
 */
class FixedUplinkMac {
public:
	FixedUplinkMac () {
		macHeader.reg = 0x00;
		extPkgCnt = 0;
		Aes_p = NULL;
	}

	/**
	 * @brief	Init method, must be called before usingthe other methods
	 * 
	 * @return	Error code, 0 on success
	 */
	int16_t init() {
		return 0;
	}

	/**
	 * @brief	Create MPDU payload out of MAC payload
	 * 
	 * @param	mpduPayload	Pointer to existing array for storing the output data (length at least MPDU_Length)
	 * @param	macPayload	Pointer to input MAC payload
	 * @param	len			Length of MAC payload data
	 * @param	MPF_present	Flag if MPF field is present
	 * @param	MPF_value	Value of MPF field (if present)
	 *
	 * @return  Length of MPDU payload
	 */
	uint16_t encode(uint8_t* const mpduPayload, const uint8_t* const macPayload, const uint16_t len,
			const bool MPF_present = false, const uint8_t MPF_value = 0) {
		// Create AES on the stack
		TsUnbLib::Aes128 Aes;
		Aes_p = &Aes;
		Aes_p->init(networkKey);

		// Set MPF field in header
		macHeader.bit.mpfflag = MPF_present;

		// CMAC initlization vector
		// EUI64
		uint8_t iv[BLOCK_SIZE_AES];
		for(uint8_t i = 0; i < 8; ++i)
			iv[i] = eui64[i];
		iv[8] = 0x00u;
		iv[9] = DATA_DIRECTION;
		iv[10] = extPkgCnt >> 24;
		iv[11] = extPkgCnt >> 16;
		iv[12] = extPkgCnt >> 8;
		iv[13] = extPkgCnt;
		iv[14] = 0xFFu;
		iv[15] = 0xFFu;


		// Actual packet
		uint16_t idx = 0;
		mpduPayload[idx++] = macHeader.reg;
		if(macHeader.bit.addressingflag)    // Long address
		{
			for(uint8_t i = 0; i < 8; ++i)
				mpduPayload[idx++] = eui64[i];
		}
		else
		{
			for(uint8_t i = 0; i < 2; ++i)
				mpduPayload[idx++] = shortAddr[i];
		}
		mpduPayload[idx++] = extPkgCnt >> 16;
		mpduPayload[idx++] = extPkgCnt >> 8;
		mpduPayload[idx++] = extPkgCnt;
		uint16_t beginEncrypted = idx;

		// We never use a MPF header
		if(macHeader.bit.mpfflag)
			mpduPayload[idx++] = MPF_value;

		for(uint8_t i = 0; i < len; ++i)
			mpduPayload[idx++] = macPayload[i];


		// Encryption with IV as input
		for(uint8_t block = 0; beginEncrypted < idx; ++block)
		{
			iv[14] = 0; // Block counter will never exceed one byte
			iv[15] = block;
			uint8_t ivEnc[BLOCK_SIZE_AES];
			Aes_p->chipher(iv, ivEnc);

			for(uint8_t i = 0; (i < BLOCK_SIZE_AES) && (beginEncrypted < idx); ++i)
				mpduPayload[beginEncrypted++] ^= ivEnc[i];
		}

		iv[14] = 0xFFu;
		iv[15] = 0xFFu;

		Aes_p->generateCmac(iv, mpduPayload, idx, iv);

		for(uint8_t i = 0; i < 4; ++i)
			mpduPayload[idx++] = iv[i];
		extPkgCnt++;
		return idx;

	}

	/**
	 * @brief	Get the MPDU length for MAC_PayloadLength and MPF_present flag
	 * 
	 * @param	MAC_PayloadLength	Payload length of the MAC
	 * @param	MPF_present			Flag if MPF field is present
	 * 
	 * @return	Lenght of the MPDU
	 * 
	 */
	uint16_t MPDU_Length(const uint16_t MAC_PayloadLength, const bool MPF_present = false) const {
		uint8_t ret = 10 + MAC_PayloadLength;
		if (MPF_present) // MPF field present
			ret += 1;

		// Add 6 bytes for long address
		if(macHeader.bit.addressingflag)
			ret += 6;

		return ret;
	}


	/**
	 * @brief	Set the 16 byte network key.
	 * 
	 * @param	k0	Byte 0 of network key
	 * @param	k1	Byte 1 of network key
	 * @param	k2	Byte 2 of network key
	 * @param	k3	Byte 3 of network key
	 * @param	k4	Byte 4 of network key
	 * @param	k5	Byte 5 of network key
	 * @param	k6	Byte 6 of network key
	 * @param	k7	Byte 7 of network key
	 * @param	k8	Byte 8 of network key
	 * @param	k9	Byte 9 of network key
	 * @param	k10	Byte 10 of network key
	 * @param	k11	Byte 11 of network key
	 * @param	k12	Byte 12 of network key
	 * @param	k13	Byte 13 of network key
	 * @param	k14	Byte 14 of network key
	 * @param	k15	Byte 15 of network key
	 * 
	 */
	void setNetworkKey(const uint8_t k0, const uint8_t k1, const uint8_t k2, const uint8_t k3,
			const uint8_t k4, const uint8_t k5, const uint8_t k6, const uint8_t k7,
			const uint8_t k8, const uint8_t k9, const uint8_t k10, const uint8_t k11,
			const uint8_t k12, const uint8_t k13, const uint8_t k14, const uint8_t k15) {
		networkKey[0] = k0;
		networkKey[1] = k1;
		networkKey[2] = k2;
		networkKey[3] = k3;
		networkKey[4] = k4;
		networkKey[5] = k5;
		networkKey[6] = k6;
		networkKey[7] = k7;
		networkKey[8] = k8;
		networkKey[9] = k9;
		networkKey[10] = k10;
		networkKey[11] = k11;
		networkKey[12] = k12;
		networkKey[13] = k13;
		networkKey[14] = k14;
		networkKey[15] = k15;
	}

	/**
	 * @brief	Set the EUI-64.
	 * 
	 * @param	e0	Byte 0 of EUI-64
	 * @param	e1	Byte 1 of EUI-64
	 * @param	e2	Byte 2 of EUI-64
	 * @param	e3	Byte 3 of EUI-64
	 * @param	e4	Byte 4 of EUI-64
	 * @param	e5	Byte 5 of EUI-64
	 * @param	e6	Byte 6 of EUI-64
	 * @param	e7	Byte 7 of EUI-64
	 * 
	 */
	void setEui64(const uint8_t e0, const uint8_t e1, const uint8_t e2, const uint8_t e3,
			const uint8_t e4, const uint8_t e5, const uint8_t e6, const uint8_t e7) {
		eui64[0] = e0;
		eui64[1] = e1;
		eui64[2] = e2;
		eui64[3] = e3;
		eui64[4] = e4;
		eui64[5] = e5;
		eui64[6] = e6;
		eui64[7] = e7;
	}

	/**
	 * @brief	Set the short address.
	 * 
	 * @param	s0	Byte 0 of short address
	 * @param	s1	Byte 1 of short address
	 * 
	 */
	void setShortAddress(const uint8_t s0, const uint8_t s1) {
		shortAddr[0] = s0;
		shortAddr[1] = s1;
	}

	/**
	 * @brief	Set the EUI-64 and the short address (last two bytes of EUI-64).
	 * 
	 * @param	e0	Byte 0 of EUI-64
	 * @param	e1	Byte 1 of EUI-64
	 * @param	e2	Byte 2 of EUI-64
	 * @param	e3	Byte 3 of EUI-64
	 * @param	e4	Byte 4 of EUI-64
	 * @param	e5	Byte 5 of EUI-64
	 * @param	e6	Byte 6 of EUI-64
	 * @param	e7	Byte 7 of EUI-64
	 * 
	 */
	void setAddress(const uint8_t e0, const uint8_t e1, const uint8_t e2, const uint8_t e3,
			const uint8_t e4, const uint8_t e5, const uint8_t e6, const uint8_t e7) {
		eui64[0] = e0;
		eui64[1] = e1;
		eui64[2] = e2;
		eui64[3] = e3;
		eui64[4] = e4;
		eui64[5] = e5;
		shortAddr[0] = eui64[6] = e6;
		shortAddr[1] = eui64[7] = e7;
	}

	/**
	 * @brief	Set the TS-UNB address mode (long or short address).
	 * 
	 * @param	addressMode		Address mode (long or short address)
	 * 
	 */
	void setAddressMode(const TsUnbAddressMode addressMode) {
		switch(addressMode) {
		case TsUnb_Short: {
			macHeader.bit.addressingflag = 0;
			break;
		}

		case TsUnb_Long: {
			macHeader.bit.addressingflag = 1;
			break;
		}
		}
		return;
	}

	/**
	 * @brief	Get LSB of Short Address for Sync Burst 
	 * 
	 * @return	LSB of Short Addresss 
	 * 
	 */
	uint8_t getLsbShortAddress() const {
		return shortAddr[1];
	}

	/**
	 * @brief	Get internal Counter (i.e. extPkgCnt)
	 * 
	 * @return	extPkgCnt
	 * 
	 */
	uint32_t getCounter() const {
		return extPkgCnt;
	}

	uint8_t networkKey[16]; /**< @brief Network key */
	uint8_t eui64[8];       /**< @brief EUI64 */
	uint8_t shortAddr[2];   /**< @brief Short address */
	uint32_t extPkgCnt;     /**< @brief Extended packet counter */

    static const uint8_t MMODE = 0;	/**< @brief MMODE for fixed MAC */

private:


	/**
	 * @brief Union for the MAC header format
	 */
	typedef union {
		/**
		 * @brief Acces the individual flags of the MAC header
		 *
		 * Comments refere to the set flags
		 */
		struct {
			/**
			 * @brief Acknowledgement
			 *
			 * 0: no downlink message received since last uplink transmission\n
			 * 1: downlink transmission received
			 */
			uint8_t  ack:1;
			/**
			 * @brief Attach flag
			 *
			 * 0: regular packet\n
			 * 1: attachment packet, also requires addressing mode = 1, control flag = 1, RX open flag = 1 and response flag = 1
			 */
			uint8_t  attachflag:1;
			/**
			 * @brief Addressing mode
			 *
			 * 0: short addressing mode, 16 bit short address\n
			 * 1: full addressing mode, 64 bit EUI
			 */
			uint8_t  addressingflag:1;
			/**
			 * @brief RX Open flag
			 *
			 * 0: receive window for DL reception off\n
			 * 1: receive window for DL reception on
			 */
			uint8_t  rxopenflag:1;
			/**
			 * @brief Response flag
			 *
			 * 0: no response expected\n
			 * 1: response expected
			 */
			uint8_t  responseflag:1;
			/**
			 * @brief Control flag
			 *
			 * 0: no control payload present (data only packet)\n
			 * 1: control payload present
			 */
			uint8_t  conrolflag:1;
			/**
			 * @brief MPF flag
			 *
			 * 0: No MPF field present\n
			 * 1: MPF field present
			 */
			uint8_t  mpfflag:1;
			/**
			 * @brief MAC version
			 *
			 * 0: Initial version
			 * 1: Reserved
			 */
			uint8_t  macversion:1;
		} bit;
		/**
		 * @brief Byte representation of the MAC header
		 */
		uint8_t reg;
	} macHeader_t;


	//! MAC header storage
	macHeader_t macHeader;

	//! Pointer to instance of AES, actual instance is created on stack in the encode() method
	TsUnbLib::Aes128 *Aes_p;

};

};	// namespace TsUnb
};	// namespace TsUnbLib

#endif // TS_UNB_FIXED_UPLINK_MAC_H_

