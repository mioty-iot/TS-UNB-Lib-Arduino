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
 * @brief	Provides simplified access to the bits of a byte array with MSB first
 *
 * @authors	Joerg Robert
 * @file	BitAccess.h
 *
 */



#ifndef BIT_ACCESS_H_
#define BIT_ACCESS_H_

#include <inttypes.h>

namespace TsUnbLib {

/**
 * @brief Read bit out of byte memory
 *
 * @param bitIdx	Position of the bit
 * @param memory	Pointer to the memory containing the bits
 *
 * @return	bit		Read bit
 */
static inline uint8_t readBit(const uint_fast16_t bitIdx, const uint8_t* const memory) {
	const uint_fast16_t byteIdx = bitIdx >> 3;			// equivalent to / 8;
	const uint_fast8_t intraByteIdx = bitIdx & 0x07;	// equivalent to % 8

	// Check if bit is set, always shift to MSB
	if ((memory[byteIdx] << intraByteIdx) & 0x80)
		return 1;
	else
		return 0;
}

/**
 * @brief Write bit into byte memory
 * 
 * @param bit		Bit that should be written
 * @param bitIdx	Position of the bit
 * @param memory	Pointer to the memory containing the bits
 */
static inline void writeBit(const uint8_t bit, const uint_fast16_t bitIdx, uint8_t* const memory) {
	const uint_fast16_t byteIdx = bitIdx >> 3;			// equivalent to / 8;
	const uint_fast8_t intraByteIdx = bitIdx & 0x07;	// equivalent to % 8

	if (bit) {
		memory[byteIdx] |= (0x80 >> intraByteIdx);
	}
	else {
		memory[byteIdx] &= ~(0x80 >> intraByteIdx);
	}

	return;
}

};	// namespace TsUnbLib
#endif // BIT_ACCESS_H_

