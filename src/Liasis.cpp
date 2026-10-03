/******************************************************************************
Liasis.cpp
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

#include "Wire.h"
#include "math.h"
#include "Liasis.h"

Liasis::Liasis() {
}

bool Liasis::begin() {
  Wire.begin();
  Wire.beginTransmission(ADC_ADR);
  if (Wire.endTransmission() != 0) return false;
  WriteWord_LE(ADC_ADR, ADC_CONF, ADC_CONF_MASK);
  return true;
}

float Liasis::getThermo() {
  return GetADC(ADC1) * (0.125);
}

float Liasis::getThermoC(uint8_t Accuracy) {
  float Val = (GetADC(ADC1) * (0.125) - 900) / 100;
  float Conv = 0;
  uint8_t n = 3;
  if (Accuracy == 1) {
    float P[8] = { 4.7335e-6, -1.224e-4, 8.2641e-4, 0.0017, -0.0213, -0.2526, 7.3409, 24.9717 };
    n = 7;
    for (int i = 0; i <= n; i++) {
      Conv = Conv + P[i] * pow(Val, n - i);
    }
  } else {
    float P[4] = { 0.0082, -0.2792, 7.1477, 25.1051 };
    n = 3;
    for (int i = 0; i <= n; i++) {
      Conv = Conv + P[i] * pow(Val, n - i);
    }
  }
  return Conv;
}

float Liasis::getTemp() {
  float Val = float(GetADC(ADC0)) * (1.25e-4);
  return TempConvert(Val, 1.8, 10000.0, Beta, 100000.0) - 273.15;
}

float Liasis::TempConvert(float V, float Vcc, float R, float A, float B, float C, float D, float R25) {
  float Rt = -R / (1 - (Vcc / V));
  float LogRt = log(Rt / R25);
  float T = 1.0 / (A + B * LogRt + C * pow(LogRt, 2.0) + D * pow(LogRt, 3.0));
  return T;
}

float Liasis::TempConvert(float V, float Vcc, float R, float Beta, float R25) {
  float Rt = -R / (1 - (Vcc / V));
  float T = 1.0 / ((1.0 / Beta) * log(Rt / R25) + 1 / 298.15);
  return T;
}

String Liasis::getHeader() {
  return "IR_Long [mV],IR_Long [C],PyrgT [C],";
}

String Liasis::getString() {
  return String(getThermo()) + "," + String(getThermoC()) + "," + String(getTemp()) + ",";
}

unsigned int Liasis::GetADC(unsigned int Num) {
  unsigned int ADC_Config = ADC_CONF_MASK | (Num << 12);
  WriteWord_LE(ADC_ADR, ADC_CONF, ADC_Config);
  delay(300);
  return ReadWord_LE(ADC_ADR, ADC_CONV);
}

uint8_t Liasis::WriteWord_LE(uint8_t Adr, uint8_t Command, unsigned int Data) {
  Wire.beginTransmission(Adr);
  Wire.write(Command);
  Wire.write((Data >> 8) & 0xFF);
  Wire.write(Data & 0xFF);
  return Wire.endTransmission();
}

int Liasis::ReadWord_LE(uint8_t Adr, uint8_t Command) {
  bool Error = SendCommand(Adr, Command);
  unsigned long localTime = millis();
  Wire.requestFrom(Adr, 2);
  while (Wire.available() < 2 && (millis() - localTime) < timeout)
    ;
  uint8_t ByteHigh = Wire.read();
  uint8_t ByteLow = Wire.read();
  return ((ByteHigh << 8) | ByteLow);
}

uint8_t Liasis::SendCommand(uint8_t Adr, uint8_t Command) {
  Wire.beginTransmission(Adr);
  Wire.write(Command);
  return Wire.endTransmission();
}
