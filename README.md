# TS-UNB 

Welcome to TS-UNB-Lib. It implements the standard ETSI TS 103 357 TS-UNB.  
The purpose of this TS-UNB-Lib is academic and non-commercial use. Therefore, Fraunhofer does
not offer any support for the TS-UNB-Lib. Furthermore, the TS-UNB-Lib is NOT identical and on 
the same quality level as the commercially-licensed MIOTY software also available from Fraunhofer.
Users are encouraged to check the Fraunhofer website for additional applications information and documentation.


## Supported Devices

This library supports the devices listet below. The list of devices will be extended in the future.
### Systems
* Arduino systems with ATmega328p processor (8MHz and 16MHz); 
Please note: The oscillator frequency of the ATmega328p crystal oscillator defines the exact transmission rate. Sometimes
the maximum frequency offset of the crystal oscillator is too high. In this case the data cannot be received or it is
received with reduced robustness. Please ensure that you use a good oscillator with a maximum offset of 20PPM.

### Transceiver Chipsets
* HopeRF RFM69w
* HopeRF RFM69hw


## License
See the file [LICENSE.md](LICENSE.md) for details on the software license for this library.

## Installation
Please copy the complete library in the Arduino\libraries folder and try the examples.
