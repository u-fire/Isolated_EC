#ifndef UFIRE_EC
#define UFIRE_EC

#include <math.h>

#if defined(PARTICLE)
# include "application.h"
# define bitRead(value, bit) (((value) >> (bit)) & 0x01)
# define bitSet(value, bit) ((value) |= (1UL << (bit)))
# define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
# define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#else // if defined(PARTICLE)
# include <Arduino.h>
# include <Wire.h>
#endif // if defined(PARTICLE)


#define EC_SALINITY 0x3c
#define EC_MEASURE_EC 80                  /*!< Command to start an EC measure */
#define EC_MEASURE_TEMP 40                /*!< Command to measure temperature */
#define EC_CALIBRATE_PROBE 20             /*!< Command to calibrate the probe */
#define EC_CALIBRATE_LOW 10               /*!< Command to calibrate the low point of the probe */
#define EC_CALIBRATE_HIGH 8               /*!< Command to calibrate the high point of the probe */
#define EC_I2C 4                          /*!< Command to change the i2c address */
#define EC_READ 2                         /*!< Command to read from EEPROM */
#define EC_WRITE 1                        /*!< Command to write to EEPROM */

#define EC_VERSION_REGISTER 0             /*!< version register */
#define EC_MS_REGISTER 1                  /*!< mS register */
#define EC_TEMP_REGISTER 5                /*!< temperature in C register */
#define EC_SOLUTION_REGISTER 9            /*!< calibration solution register */
#define EC_TEMPCOEF_REGISTER 13           /*!< temperatue coefficient register */
#define EC_CALIBRATE_REFHIGH_REGISTER 17  /*!< reference low register */
#define EC_CALIBRATE_REFLOW_REGISTER 21   /*!< reference high register */
#define EC_CALIBRATE_READHIGH_REGISTER 25 /*!< reading low register */
#define EC_CALIBRATE_READLOW_REGISTER 29  /*!< reading high register */
#define EC_CALIBRATE_OFFSET_REGISTER 33   /*!< calibration offset */
#define EC_SALINITY_PSU 37                /*!< Salinity register */
#define EC_RAW_REGISTER 41                /*!< raw count register */
#define EC_TEMP_COMPENSATION_REGISTER 45  /*!< temperature compensation register */
#define EC_BUFFER_REGISTER 49             /*!< buffer register */
#define EC_FW_VERSION_REGISTER 53         /*!< firmware version register */
#define EC_CONFIG_REGISTER 54             /*!< config register */
#define EC_TASK_REGISTER 55               /*!< task register */

#define EC_EC_MEASUREMENT_TIME 250        /*!< delay between EC measurements */
#define EC_TEMP_MEASURE_TIME 750          /*!< delay for temperature measurement */

#define EC_DUALPOINT_CONFIG_BIT 0         /*!< dual point config bit */
#define EC_TEMP_COMPENSATION_CONFIG_BIT 1 /*!< temperature compensation config bit */

class uFire_EC                         /*! EuFire_EC Class */
{
public:

  float S;                             /*!< EC in Siemens */
  float mS;                            /*!<Salinity EC in milli-Siemens */
  float uS;                            /*!< EC in micro-Siemens */
  float raw;
  long PPM_500;                        /*!< Parts per million using 500 as a multiplier */
  long PPM_640;                        /*!< Parts per million using 640 as a multiplier */
  long PPM_700;                        /*!< Parts per million using 700 as a multiplier */
  float salinityPSU;                   /*!< Salinity measured practical salinity units */
  float tempC;                         /*!< Temperature in C */
  float tempF;                         /*!< Temperature in F */
  static const float tempCoefEC;       /*!< Temperature compensation coefficient for EC measurement */
  static const float tempCoefSalinity; /*!< Temperature compensation coefficient for salinity measurement */
  uFire_EC(uint8_t i2c_address);
  uFire_EC();
  #ifdef ESP32
  uFire_EC(uint8_t sda,
              uint8_t scl,
              uint8_t i2c_address);
  uFire_EC(uint8_t sda,
              uint8_t scl);
  #endif // ifndef ESP32
  ~uFire_EC();
  float measureEC(float tempCoefficient,
                  bool  newTemp);
  float measureEC();
  float measureSalinity();
  float measureTemp();
  void  setTemp(float temp_C);
  void  calibrateProbe(float solutionEC,
                       float tempCoef);
  void  calibrateProbeLow(float solutionEC,
                          float tempCoef);
  void  calibrateProbeHigh(float solutionEC,
                           float tempCoef);
  void  setDualPointCalibration(float refLow,
                                float refHigh,
                                float readLow,
                                float readHigh);
  void    reset();
  void    setTempConstant(float b);
  float   getTempConstant();
  void    useTemperatureCompensation(bool b);
  bool    usingTemperatureCompensation();
  void    useDualPoint(bool b);
  bool    usingDualPoint();
  float   getCalibrateHighReference();
  float   getCalibrateLowReference();
  float   getCalibrateHighReading();
  float   getCalibrateLowReading();
  void    setCalibrateOffset(float offset);
  float   getCalibrateOffset();
  uint8_t getVersion();
  uint8_t getFirmware();
  void    setI2CAddress(uint8_t i2cAddress);
  bool    connected();
  void    writeEEPROM(uint8_t address,
                      float   value);
  float   readEEPROM(uint8_t address);

private:

  uint8_t _address;
  void    _change_register(uint8_t register);
  void    _send_command(uint8_t command);
  void    _write_register(uint8_t reg,
                          float   f);
  void    _write_byte(uint8_t reg,
                      uint8_t val);
  float   _read_register(uint8_t reg);
  uint8_t _read_byte(uint8_t reg);
};

#endif // ifndef UFIRE_EC
