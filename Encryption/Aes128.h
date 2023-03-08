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
 * @brief	Basic AES-128 encryption algorithms
 *
 * @authors	Joerg Robert, Clemens Neumueller
 * @file	TsUnbAes.h
 *
 * This file implements the basic AES-128 encryption algorithms according to
 * the NIST standard available here: https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf/
 *
 */


#ifndef TSUNB_AES_H_
#define TSUNB_AES_H_

#include <stdlib.h>
#include <stdint.h>
#ifdef __AVR_ARCH__
#include <avr/pgmspace.h>
#endif

namespace TsUnbLib {

//! Bytes per cipher
#define AES_BYTES		16

//! Key length in words
#define AES_NK			4

//! Block size in words
#define AES_NB			4

//! Number of rounds
#define AES_NR			10

//! Number of bytes per word
#define AES_WORD		4

//! Modulo polynomial for multiplication
#define AES_MOD_POLY	0x11B

//! CMAC subkey generation constant RB
#define AES_CMAC_RB		0x87u

//! Substitution values for the byte 0xXY
const
#ifdef __AVR_ARCH__
PROGMEM
#endif
uint8_t AES_sBox[256] = {
		// Y 0     1     2     3     4     5     6     7      8    9     A      B    C     D     E     F        X
		0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,	// 0
		0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,	// 1
		0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,	// 2
		0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,	// 3
		0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,	// 4
		0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,	// 5
		0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,	// 6
		0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,	// 7
		0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,	// 8
		0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,	// 9
		0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,	// A
		0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,	// B
		0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,	// C
		0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,	// D
		0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,	// E
		0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16	// F
};



/**
 * @brief Implementation of AES-128 for ETSI TS 103 357 TS-UNB
 *
 * This class implemets the AES-128 encryption for ETSI TS 103 357 according to
 * https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf
 *
 * This class uses special memory optimization for AVR microprocessors, e.g.
 * to support the ATmega328p.
 *
 */
class Aes128 {
public:

	/**
	 * @brief Initializes this module by expanding the chipher key
	 *
	 * @param 	key 	128-bit cipher key
	 *
	 */
	void init (const uint8_t* const key) {
		for (uint16_t i = 0; i < AES_BYTES; ++i)
			keyW[0][i] = key[i];

		uint8_t temp[AES_WORD];
		uint8_t * lastWord;

		lastWord = &keyW[0][AES_BYTES - AES_WORD];

		// Will not work for other AES key lengths
		for (uint16_t roundIdx = 1; roundIdx <= AES_NR; ++roundIdx) {
			subRotRconWord (lastWord, temp, roundIdx);
			lastWord = temp;

			for (uint8_t wordIdx = 0; wordIdx < AES_WORD; ++wordIdx)	{
				xorWord (&keyW[roundIdx - 1][wordIdx << 2],
						lastWord, &keyW[roundIdx][wordIdx << 2]);

				lastWord = &keyW[roundIdx][wordIdx << 2];
			}
		}
	}


	/**
	 * @brief Encrypts the input data using the AES-128 algorithm
	 *
	 * @param 	in 		Plain text input data (16 byte)
	 * @param 	out 	Encrypted output data (16 byte)
	 *
	 */
	void chipher (const uint8_t* const in, uint8_t* const out) {
		for (uint8_t i = 0; i < AES_BYTES; ++i)
			out[i] = in[i];

		addRoundKey (out, 0);

		for (uint8_t i = 1; i < AES_NR; ++i)	{
			subBytesAndShiftRows (out);
			mixColumns (out);
			addRoundKey (out, i);
		}

		subBytesAndShiftRows (out);
		addRoundKey (out, AES_NR);
	}



	void generateCmac(const uint8_t* const input, const uint16_t inputLen, uint8_t* const output) {
		uint8_t subkey1[AES_BYTES], subkey2[AES_BYTES];

		cmacGenerateSubkey(subkey1, subkey2);

		uint8_t blocks = (uint16_t)(inputLen + AES_BYTES - 1) >> 4;
		uint8_t flag;

		if(blocks == 0)	{
			blocks = 1;
			flag = 0;
		}
		else {
			if(inputLen & 0x000Fu)
				flag = 0;
			else
				flag = 1;
		}

		for(uint8_t i = 0; i < AES_BYTES; ++i)
			output[i] = 0;

		for(uint16_t msg = 0; msg < (uint16_t)(blocks - 1); ++msg)
		{
			xor16byte(&input[msg << 4], output);
			chipher(output,output);
		}

		if(flag) {
			xor16byte(subkey1, output);
			xor16byte(&input[(blocks-1) << 4], output);
		}
		else {
			xor16byte(subkey2, output);
			uint8_t extraBytes = inputLen & 0x000Fu;
			for(uint8_t i = 0; i < AES_BYTES; ++i)	{
				if(i < extraBytes)
					output[i] ^= input[((blocks-1) << 4) + i];
				else if(i == extraBytes)
					output[i] ^= 0x80u;
				else
					output[i] ^= 0x00u;
			}
		}
		chipher(output,output);
	}

	void generateCmac(uint8_t* const cmacInitVector, const uint8_t* const input, const uint16_t inputLen, uint8_t* const output) {
		uint8_t subkey1[AES_BYTES], subkey2[AES_BYTES];

		cmacGenerateSubkey(subkey1, subkey2);

		uint8_t blocks = (uint16_t)(inputLen + AES_BYTES - 1) >> 4;
		uint8_t flag;

		if(blocks == 0)	{
			blocks = 1;
			flag = 0;
		}
		else {
			if(inputLen & 0x000Fu)
				flag = 0;
			else
				flag = 1;
		}

		for(uint8_t i = 0; i < AES_BYTES; ++i) {
			output[i] = cmacInitVector[i];

		}

		chipher(output,output);

		for(uint16_t msg = 0; msg < (uint16_t)(blocks - 1); ++msg) {
			xor16byte(&input[msg << 4], output);
			chipher(output,output);
		}

		if(flag) {
			xor16byte(subkey1, output);
			xor16byte(&input[(blocks-1) << 4], output);
		}
		else {
			xor16byte(subkey2, output);
			uint8_t extraBytes = inputLen & 0x000Fu;
			for(uint8_t i = 0; i < AES_BYTES; ++i)	{
				if(i < extraBytes)
					output[i] ^= input[((blocks-1) << 4) + i];
				else if(i == extraBytes)
					output[i] ^= 0x80u;
				else
					output[i] ^= 0x00u;
			}
		}
		chipher(output,output);
	}



private:

	/**
	 * @brief SubByte and the subsequent ShiftRows operation
	 *
	 * @param 	data 	Data this operation is performed on (16 byte)
	 *
	 */
	void subBytesAndShiftRows (uint8_t* const data) {
#ifdef __AVR_ARCH__
		// Row 0
		data[0] = (uint8_t)pgm_read_byte (&(AES_sBox[data[0]]));
		data[4] = (uint8_t)pgm_read_byte (&(AES_sBox[data[4]]));
		data[8] = (uint8_t)pgm_read_byte (&(AES_sBox[data[8]]));
		data[12] = (uint8_t)pgm_read_byte (&(AES_sBox[data[12]]));

		// Row 1
		uint8_t tmp = data[1];
		data[1] = (uint8_t)pgm_read_byte (&(AES_sBox[data[5]]));
		data[5] = (uint8_t)pgm_read_byte (&(AES_sBox[data[9]]));
		data[9] = (uint8_t)pgm_read_byte (&(AES_sBox[data[13]]));
		data[13] = (uint8_t)pgm_read_byte (&(AES_sBox[tmp]));

		// Row 2
		tmp = data[2];
		data[2] = (uint8_t)pgm_read_byte (&(AES_sBox[data[10]]));
		data[10] = (uint8_t)pgm_read_byte (&(AES_sBox[tmp]));
		tmp = data[6];
		data[6] = (uint8_t)pgm_read_byte (&(AES_sBox[data[14]]));
		data[14] = (uint8_t)pgm_read_byte (&(AES_sBox[tmp]));

		// Row 3
		tmp = data[15];
		data[15] = (uint8_t)pgm_read_byte (&(AES_sBox[data[11]]));
		data[11] = (uint8_t)pgm_read_byte (&(AES_sBox[data[7]]));
		data[7] = (uint8_t)pgm_read_byte (&(AES_sBox[data[3]]));
		data[3] = (uint8_t)pgm_read_byte (&(AES_sBox[tmp]));
#else
		// Row 0
		data[0] = AES_sBox[data[0]];
		data[4] = AES_sBox[data[4]];
		data[8] = AES_sBox[data[8]];
		data[12] = AES_sBox[data[12]];

		// Row 1
		uint8_t tmp = data[1];
		data[1] = AES_sBox[data[5]];
		data[5] = AES_sBox[data[9]];
		data[9] = AES_sBox[data[13]];
		data[13] = AES_sBox[tmp];

		// Row 2
		tmp = data[2];
		data[2] = AES_sBox[data[10]];
		data[10] = AES_sBox[tmp];
		tmp = data[6];
		data[6] = AES_sBox[data[14]];
		data[14] = AES_sBox[tmp];

		// Row 3
		tmp = data[15];
		data[15] = AES_sBox[data[11]];
		data[11] = AES_sBox[data[7]];
		data[7] = AES_sBox[data[3]];
		data[3] = AES_sBox[tmp];
#endif
	}

	/**
	 * @brief MixColumns operation
	 *
	 * @param 	data 	Data this operation is performed on (16 byte)
	 *
	 */
	void mixColumns (uint8_t* const data) {
		uint8_t input[4];
		uint8_t twoTimesPoly;
		uint8_t * column;

		for (uint8_t col = 0; col < AES_NB; ++col)	{

			column = &data[col << 2];
			for (uint8_t i = 0; i < AES_WORD; ++i)	{
				input[i] = column[i];
				column[i] = 0;
			}

			for (uint8_t inRow = 0; inRow < AES_WORD; ++inRow) {
				twoTimesPoly = multiplyByX (input[inRow]);

				column[inRow] ^= twoTimesPoly;
				column[(inRow + 3) & 3] ^= twoTimesPoly;

				for (uint8_t outRow = 1; outRow < AES_WORD; ++outRow)
					column[(inRow + outRow) & 3] ^= input[inRow];
			}
		}
	}

	/**
	 * @brief Multiplies the input by x in GF(2^8)
	 *
	 * @param 	polynomial 	input polynomial
	 *
	 * @return 	uint8_t output polynomial
	 *
	 */
	uint8_t multiplyByX (const uint8_t polynomial) const {
		if (polynomial & 0x80) {

			// Polynomial has to be reduced
			uint16_t tmp = ((uint16_t) polynomial) << 1;
			tmp ^= AES_MOD_POLY;

			return (uint8_t) tmp;
		}
		else
			return polynomial << 1;
	}

	/**
	 * @brief AddRoundKey operation
	 *
	 * @param 	data 		Data this operation is performed on (16 byte)
	 * @param 	roundIdx 	Round of the encryption algorithm
	 *
	 */
	void addRoundKey (uint8_t* const data, const uint8_t roundIdx) const {
		for (uint8_t i = 0; i < AES_BYTES; ++i)
			data[i] ^= keyW[roundIdx][i];
	}

	/**
	 * @brief Apply RotWord and the subsequent SubWord on data and XORs it with Rcon[roundIdx]
	 *
	 * @param 	in 			Word this operation is performed on (4 byte)
	 * @param 	out 		Output word (4 byte)
	 * @param 	roundIdx 	Round of the encryption algorithm
	 *
	 */
	void subRotRconWord (uint8_t* const in, uint8_t* const out,
			const uint8_t roundIdx) const {

		uint8_t rconPoly = 1 << (roundIdx - 1);

		if (roundIdx > 8)
			// Will not work for other AES key lengths
			rconPoly ^= AES_MOD_POLY << (roundIdx - 9);
#ifdef __AVR_ARCH__
		out[0] = (uint8_t)pgm_read_byte (&(AES_sBox[in[1]])) ^ rconPoly;
		out[1] = (uint8_t)pgm_read_byte (&(AES_sBox[in[2]]));
		out[2] = (uint8_t)pgm_read_byte (&(AES_sBox[in[3]]));
		out[3] = (uint8_t)pgm_read_byte (&(AES_sBox[in[0]]));
#else
		out[0] = AES_sBox[in[1]] ^ rconPoly;
		out[1] = AES_sBox[in[2]];
		out[2] = AES_sBox[in[3]];
		out[3] = AES_sBox[in[0]];
#endif
	}

	/**
	 * @brief XORs two words
	 *
	 * @param 	inA 	First input word (4 byte)
	 * @param 	inB 	Second input word (4 byte)
	 * @param 	out 	Output word (4 byte)
	 *
	 */
	void xorWord(const uint8_t* const inA, const uint8_t* const inB,
			uint8_t* const out) const {

		for (uint8_t i = 0; i < AES_WORD; ++i)
			out[i] = inA[i] ^ inB[i];
	}

	/**
	 * @brief XORs two 16 byte arrays in place
	 *
	 * @param 	inA 	First input array
	 * @param 	inOutB 	Second input and output array
	 *
	 */
	void xor16byte(const uint8_t* const inA, uint8_t* const inOutB) const {
		for(uint8_t i = 0; i < AES_BYTES; ++i)
			inOutB[i] ^= inA[i];
	}

	void cmacGenerateSubkey(uint8_t *subkey1, uint8_t *subkey2)	{
		for(uint8_t i = 0; i < AES_BYTES; ++i)
			subkey1[i] = 0;
		chipher(subkey1, subkey1);

		if(leftShift16byte(subkey1, subkey1))
			subkey1[AES_BYTES - 1] ^= AES_CMAC_RB;
		if(leftShift16byte(subkey1, subkey2))
			subkey2[AES_BYTES - 1] ^= AES_CMAC_RB;
	}

	uint8_t leftShift16byte(const uint8_t *dataIn, uint8_t *dataOut) const {
		uint8_t prevMsb = 0;
		for(int8_t i = 15; i >= 0; --i)
		{
			uint8_t msb = dataIn[i] >> 7;
			dataOut[i] = (dataIn[i] << 1) | prevMsb;
			prevMsb = msb;
		}
		return prevMsb;
	}



	//! Memory to store the expended key
	uint8_t keyW[AES_NR + 1][AES_BYTES];

};

};	// namespace TsUnbLib

#endif // TSUNB_AES_H_

