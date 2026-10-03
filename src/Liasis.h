/******************************************************************************
Liasis.h
Library for interfacing with the Liasis longwave pyrgeometer
Bobby Schulz @ Northern Widget LLC
7/5/2018
https://github.com/NorthernWidget-Skunkworks/Liasis_Library

Measures atmospheric downwelling longwave (thermal infrared) radiation
via a ZTP-135SR thermopile, sensitive to ~5-30 µm.

"The laws of nature are constructed in such a way as to make the universe
as interesting as possible."
-Freeman Dyson

License: GNU GPL v3. You should find a copy in the repository.
******************************************************************************/

#ifndef Liasis_h
#define Liasis_h

#include "Arduino.h"

#define ADC_ADR 0x4A

#define ADC_CONF 0x01
#define ADC_CONV 0x00
#define ADC_CONF_MASK 0xC383

#define ADC0 0x00
#define ADC1 0x01
#define ADC2 0x02
#define ADC3 0x03

class Liasis {
public:
  Liasis();
  bool begin();
  float getThermo();
  float getThermoC(uint8_t Accuracy = 0);
  float getTemp();
  String getHeader();
  String getString();

private:
  const float Beta = 3960;
  const unsigned long timeout = 100;

  float TempConvert(float V, float Vcc, float R, float A, float B, float C, float D, float R25);
  float TempConvert(float V, float Vcc, float R, float Beta, float R25);
  unsigned int GetADC(unsigned int Num);
  uint8_t WriteWord_LE(uint8_t Adr, uint8_t Command, unsigned int Data);
  int ReadWord_LE(uint8_t Adr, uint8_t Command);
  uint8_t SendCommand(uint8_t Adr, uint8_t Command);
};

#endif
