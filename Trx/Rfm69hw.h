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
 * @brief	Implements the transmission of radio bursts using the HopeRF RFM69w/RFM69hw module
 *
 * @authors Clemens Neumueller, Joerg Robert
 * @file	Rfm69hw.h
 *
 */



#ifndef RFM69HW_H_
#define RFM69HW_H_

#include <stdint.h>

#include "../Utils/BitAccess.h"

namespace TsUnbLib {
namespace Trx {


//! Write access to the 'RegDataModul' register
#define RFM69_WRITE_DATA_MODUL		0x82
//! Register setting for continuous mode without bit synchronizer 
#define RFM69_CONT_FSK_NOSHAPING	0x60

//! Write access to the 'RegFdevMsb' register
#define RFM69_WRITE_FDEV			0x85

//! Write access to the 'RegFrfMsb' register
#define RFM69_WRITE_FRF				0x87

//! Write access to the 'RegOpMode' register
#define RFM69_WRITE_MODE			0x81

//! Register value for sleep mode
#define RFM69_MODE_SLEEP			0x00

//! Register value for standby mode
#define RFM69_MODE_STDBY			0x01

//! Register value for frequency synthesizer mode
#define RFM69_MODE_FS				0x02

//!brief Set transceiver into transmitter mode
#define RFM69_MODE_TX				0x0C

//! Write access to 'RegPaLevel' register
#define RFM69_WRITE_PA_LEVEL		0x91

//! Register value for PA0 on
#define RFM69_PA0_ON				0x80

//! Register value for PA1 on
#define RFM69_PA1_ON				0x40

//! Register value for PA2 on
#define RFM69_PA2_ON				0x20

//! Write access to 'RegPaRamp' register
#define RFM69_WRITE_PA_RAMP			0x92

//! Register value for 250us up-ramp time for TX mode
#define RFM69_PA_RAMP_250us			0x04

//! Array used for initialization of the transceiver, format: length, address, one or multiple data
#define RFM69_CONFIG_ARRAY  {\
		2, 0x80 + 0x01, 0x00, \
		2, 0x80 + 0x02, 0x01, \
		3, 0x80 + 0x03, 0x34, 0x83, \
		3, 0x80 + 0x05, 0x00, F_DEV, \
		2, 0x80 + 0x12, 0x04, \
		2, 0x80 + 0x13, 0x00, \
		3, 0x80 + 0x2c, 0x00, 0x00, \
		2, 0x80 + 0x2e, 0x00, \
		2, 0x80 + 0x37, 0x00, \
		2, 0x80 + 0x38, 0x00, \
		2, 0x80 + 0x3b, 0xe8, \
		2, 0x80 + 0x3c, 0x80, \
		0}


/**
 * @brief Implementation of burst transmission using the HopeRF RFM69HW
 *
 * This class implements the transmission of the radio bursts using the HopeRF RFM69HW
 * transmitter module. For this purpose the transmitter is switched into the continuous
 * transmission mode and the symbol clock is generated by the microcontroller.
 * The RFM69HW DIO2 pin is not used for the modulation. Consequently, the frequency deviation
 * is set to zeros. Instead this implementation directly changes the transmission frequency
 * for the modulation of the data.
 *
 * Depending on the actual transmitter module the template parameter BOOST_PIN must be set to true.
 * However, some modules do only support one of the modes. For transmit powers >13dBm the BOOST_PIN
 * must be always set to true, which requires the module to support it.
 *
 * The template parameter F_DEV defines the frequency deviation. The RFM69HW has a minimum step size of
 * exactly 61.03515625Hz. The required frequency deviation for the standard mode is 2380.371Hz / 4 =
 * 595.09275Hz, which cannot be achieved 61.03515625Hz. This would require a frequency deviation of 9.75.
 * Though, a frequency deviation of 10 (610.3515625Hz) results has shown to work properly. For other modes
 * the frequency deviation may be adjusted.
 *
 * The template class RadioBurst_T defines a radio burst data structure. The actual implementation has to
 * offer the methods: uint16_t getBurstLength(void), uint8_t* getBurst(void), uint16_t get_channel(void).
 *
 * At an early stage in the program the init() method shall be called. It brings the device into the sleep
 * mode to save energy. It is not part of the constructor to allow the user to start a watchdog before
 * calling the init() method.
 * 
 * @tparam		Cpu_T			Plattform depended implementation
 * @tparam		BOOST_PIN		Use of the PA BOOST pin (depends on the hardware, default is off)
 * @tparam		F_DEV			Frequency deviation resgister setting
 * @tparam		RadioBurst_T	Radio burst class
 *
 */
template <class Cpu_T, bool BOOST_PIN = false, uint32_t F_DEV = 10, 
class RadioBurst_T = TsUnb::RadioBurst<> >
class Rfm69hw {
public:
	Cpu_T Cpu;
	Rfm69hw() {
		txPower = 13;
	}

	~Rfm69hw() {
	}


	/**
	 * @brief Init method
	 *
	 * This module initializes the RFM69HW and brings it into sleep mode.
	 * It must be called before the send function. Furthermore, it shall
	 * be called as early as possible in the code to bring the RFM69HW
	 * into sleep mode to save energy after power on.
	 *
	 * @return 0 if OK, negative falue in case of errors
	 */
	int16_t init(void) {
		// TODO check if chips is actually present and return negative value in case of error
		Cpu.spiInit();

		// Read register 0x0c and check if 0x02, if no there is no chip
		{
			uint8_t spiData[2]={0x0c, 0};
			Cpu.spiSendReceive(spiData, 2);
			if (spiData[1] != 0x02) {
				// Chip not found!
				Cpu.spiDeinit();   

				return -1;
			}
		}

		/*
		 * Initialize with presets
		 */
		const uint8_t presets[] = RFM69_CONFIG_ARRAY;
		uint8_t idx = 0;
		while (presets[idx] != 0) {
			Cpu.spiSend(&presets[idx + 1], presets[idx]);
			idx += presets[idx] + 1;
		}
		setMode(RFM69_MODE_SLEEP);

		Cpu.spiDeinit();

		return 0;
	}

	/**
	 * \brief	Transmit method
	 * 
	 * This method transmits the complete packed contained in the \p numTxBursts with
	 * the frequency offset given in \p frequency.
	 *
	 * \param	Bursts		Pointer to burst data
	 * \param	numTxBursts	Number of transmit bursts
	 * \param	frequency	Frequency f0 of the transmission (module dependent in register values)
	 * 
	 * \return 0 if OK, negative falue in case of errors
	 */
	int16_t transmit(const RadioBurst_T* const Bursts, const uint16_t numTxBursts, const uint32_t frequency) {
		Cpu.spiInit();

		Cpu.initTimer();
		setTxPwrReg(txPower);

		// Give the system the time of four bits to initialize everything (approx. 10ms)
		Cpu.addTimerDelay(4);
		Cpu.startTimer();

		for (uint16_t burstIdx = 0; burstIdx < numTxBursts;	++burstIdx) {
			Cpu.resetWatchdog();

			// Special handling in case of zero length bursts
			if (Bursts[burstIdx].getBurstLength() == 0) {
				Cpu.waitTimer();
				if (burstIdx + 1 < numTxBursts) {
					Cpu.addTimerDelay((int16_t)Bursts[burstIdx].get_T_RB() - Bursts[burstIdx].getBurstLength());
				}
				continue;
			}

			const uint32_t cFrequency = (uint32_t) Bursts[burstIdx].getCarrierOffset();
			const uint32_t modFreq = frequency + cFrequency;
			Cpu.waitTimer();
			setFrequencyReg(modFreq);

			const uint8_t* burstData = Bursts[burstIdx].getBurst();
			for (uint8_t byteIdx = 0; byteIdx < Bursts[burstIdx].getBurstLengthBytes(); ++byteIdx) {
				uint8_t data[2] = {0x80, burstData[byteIdx]};
				Cpu.spiSend(data, 2);
			}

			// Add one dummy byte, if this byte is actually transmitted, the TX switches itself into sleep mode 
			// because we did not set  the sleep command
			{
				uint8_t data[2] = {0x80, 0};
				Cpu.spiSend(data, 2);
			}
			setMode(RFM69_MODE_FS);

			Cpu.addTimerDelay(2);
			Cpu.waitTimer();
			setMode(RFM69_MODE_TX);

			Cpu.addTimerDelay(Bursts[burstIdx].getBurstLength());
			Cpu.waitTimer();
			setMode(RFM69_MODE_SLEEP);

			/*
			 * If we are not in the last burst wait for the next burst to start.
			 * If we are in the last burst we do not have to restart the counter again.
			 * 
			 * We wake up 2 bits before the new burst starts. This gives us enough time to shift the
			 * data into the FIFO before the next transmission starts.
			 */
			if (burstIdx + 1 < numTxBursts) {
				Cpu.addTimerDelay((int16_t)Bursts[burstIdx].get_T_RB() - Bursts[burstIdx].getBurstLength() - 2);
			}
		}
		//Cpu.waitTimer();
		setMode(RFM69_MODE_SLEEP);
		Cpu.stopTimer();
		Cpu.spiDeinit();

		return 0;
	}

	/**
	 * @brief Sets the transmit power
	 *
	 * This method is used to set the transmit power in dBm.
	 * The default transmit power is 13dBm.
	 *
	 * @param 	power	Transmit power in dBm
	 */
	void setTxPower(const int8_t power) {
		txPower = power;
	}

private:

	/**
	 * @brief Set frequency register
	 *
	 * This method sets the frequency register to the value frequency.
	 * Caution: This method assumes that SPI is initialized!
	 *
	 */
	void setFrequencyReg(const uint32_t frequency) {
		/*
		 * Union to simplify byte access.
		 */
		union splituint32 {
			uint8_t bytes[4];
			uint32_t single;
		};
		union splituint32 regVal;
		regVal.single = frequency;
		uint8_t data[4] = {
				RFM69_WRITE_FRF, regVal.bytes[2],
				regVal.bytes[1], regVal.bytes[0]};
		Cpu.spiSend(data, 4);
	}
	
	/**
	 * @brief Set the transmit power register
	 *
	 * This methods sets the transmit power in the register.
	 * Caution: This method assumes that SPI is initialized!
	 *
	 * Details how on setting the transmit power can be found
	 * in the RFM69HW datasheet.
	 *
	 * @param	power	Transmit power in dBm
	 *
	 * @return	The actually set transmit power.
	 */
	int8_t setTxPwrReg(int8_t power) {
		// Special handling if boost pin is used
		if (BOOST_PIN) {
			// Limit to valid power range
			if (power > 17)
				power = 17;
			if (power < -2)
				power = -2;

			// If less than 13dBm use PA1 only, otherwise use PA1 and PA2
			if (power <= 13) {
				uint8_t regPower = power + 18;
				uint8_t data[2] = {RFM69_WRITE_PA_LEVEL,(uint8_t) (RFM69_PA1_ON | regPower)};
				Cpu.spiSend(data, 2);
			}
			else {
				// Use PA1 and PA2
				uint8_t regPower = power + 14;

				uint8_t data[2] = {RFM69_WRITE_PA_LEVEL, (uint8_t) (RFM69_PA1_ON | RFM69_PA2_ON | regPower)};
				Cpu.spiSend(data, 2);
			}
			return power;
		}
		else {
			// PA Boost Pin is not used

			// Limit to valid power range
			if (power > 13)
				power = 13;
			if (power < -18)
				power = -18;

			uint8_t regPower = power + 18;

			uint8_t data[2] = {RFM69_WRITE_PA_LEVEL, (uint8_t) (RFM69_PA0_ON + regPower)};
			Cpu.spiSend(data, 2);

			return power;
		}
	}

	/**
	 * @brief Set RFM69HW mode
	 *
	 * This methods sets the power mode of the RFM69HW.
	 * Caution: This method assumes that SPI is initialized!
	 *
	 * @param	mode	RFM69HW mode according to datasheet
	 */
	void setMode(const uint8_t mode) {
		uint8_t data[2] = {RFM69_WRITE_MODE, mode};
		Cpu.spiSend(data, 2);
	}

	//! Internal register to store the transmit power
	int8_t txPower;
};

};	// namespace Trx
};	// namespace TsUnbLib

#endif	/*RFM69HW_H_*/

