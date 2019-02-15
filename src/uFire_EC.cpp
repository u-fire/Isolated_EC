#include "uFire_EC.h"

const float uFire_EC::tempCoefEC       = 0.019;
const float uFire_EC::tempCoefSalinity = 0.021;

uFire_EC::uFire_EC(uint8_t i2c_address)
{
  _address = i2c_address;
  #ifndef ARDUINO_SAMD_ZERO
  Wire.begin();
  #endif // ifndef ARDUINO_SAMD_ZERO
}

uFire_EC::uFire_EC()
{
  _address = EC_SALINITY;
  #ifndef ARDUINO_SAMD_ZERO
  Wire.begin();
  #endif // ifndef ARDUINO_SAMD_ZERO
}

#ifdef ESP32
uFire_EC::uFire_EC(uint8_t sda, uint8_t scl, uint8_t i2c_address)
{
  _address = i2c_address;
  Wire.begin(sda, scl, 100000);
}

uFire_EC::uFire_EC(uint8_t sda, uint8_t scl)
{
  _address = uFire_EC;
  Wire.begin(sda, scl, 100000);
}

#endif // ifndef ESP32

uFire_EC::~uFire_EC()
{}

float uFire_EC::measureEC(bool newTemp)
{
  if (newTemp)
  {
    measureTemp();
  }

  _send_command(EC_MEASURE_EC);
  delay(EC_EC_MEASUREMENT_TIME);
  raw = _read_register(EC_RAW_REGISTER);

  if (raw == 0.0)
  {
    mS = NAN; // make it NaN so the following statement will -1 everything
  }
  else
  {
    mS = _read_register(EC_MS_REGISTER);
  }

  if (mS == mS)
  {
    PPM_500     = mS * 500;
    PPM_640     = mS * 640;
    PPM_700     = mS * 700;
    uS          = mS * 1000;
    S           = mS / 1000;
    salinityPSU = _read_register(EC_SALINITY_PSU);
  }
  else
  {
    mS          = -1;
    PPM_500     = -1;
    PPM_640     = -1;
    PPM_700     = -1;
    uS          = -1;
    S           = -1;
    salinityPSU = -1;
  }

  return mS;
}

float uFire_EC::measureEC()
{
  return measureEC(usingTemperatureCompensation());
}

float uFire_EC::measureSalinity()
{
  measureEC(usingTemperatureCompensation());
  return salinityPSU;
}

float uFire_EC::measureTemp()
{
  _send_command(EC_MEASURE_TEMP);
  delay(EC_TEMP_MEASURE_TIME);
  tempC = _read_register(EC_TEMP_REGISTER);
  if (tempC == -127.0)
  {
    tempF = -127;
  }
  else
  {
    tempF = ((tempC * 9) / 5) + 32;
  }
  return tempC;
}

void uFire_EC::setTemp(float temp_C)
{
  _write_register(EC_TEMP_REGISTER, temp_C);
  tempC = temp_C;
  tempF = ((tempC * 9) / 5) + 32;
}

void uFire_EC::calibrateProbe(float solutionEC)
{
  bool dualpoint = usingDualPoint();

  useDualPoint(false);
  _write_register(EC_SOLUTION_REGISTER, solutionEC);
  _send_command(EC_CALIBRATE_PROBE);
  delay(EC_EC_MEASUREMENT_TIME);
  useDualPoint(dualpoint);
}

void uFire_EC::calibrateProbeLow(float solutionEC)
{
  bool dualpoint = usingDualPoint();

  useDualPoint(false);
  _write_register(EC_SOLUTION_REGISTER, solutionEC);
  _send_command(EC_CALIBRATE_LOW);
  delay(EC_EC_MEASUREMENT_TIME);
  useDualPoint(dualpoint);
}

void uFire_EC::calibrateProbeHigh(float solutionEC)
{
  bool dualpoint = usingDualPoint();

  useDualPoint(false);
  _write_register(EC_SOLUTION_REGISTER, solutionEC);
  _send_command(EC_CALIBRATE_HIGH);
  delay(EC_EC_MEASUREMENT_TIME);
  useDualPoint(dualpoint);
}

void uFire_EC::setDualPointCalibration(float refLow, float refHigh, float readLow, float readHigh)
{
  _write_register(EC_CALIBRATE_REFLOW_REGISTER,   refLow);
  _write_register(EC_CALIBRATE_REFHIGH_REGISTER,  refHigh);
  _write_register(EC_CALIBRATE_READLOW_REGISTER,  readLow);
  _write_register(EC_CALIBRATE_READHIGH_REGISTER, readHigh);
}

float uFire_EC::getCalibrateOffset()
{
  return _read_register(EC_CALIBRATE_OFFSET_REGISTER);
}

float uFire_EC::getCalibrateHighReference()
{
  return _read_register(EC_CALIBRATE_REFHIGH_REGISTER);
}

float uFire_EC::getCalibrateLowReference()
{
  return _read_register(EC_CALIBRATE_REFLOW_REGISTER);
}

float uFire_EC::getCalibrateHighReading()
{
  return _read_register(EC_CALIBRATE_READHIGH_REGISTER);
}

float uFire_EC::getCalibrateLowReading()
{
  return _read_register(EC_CALIBRATE_READLOW_REGISTER);
}

void uFire_EC::useTemperatureCompensation(bool b)
{
  uint8_t retval;
  uint8_t config = _read_byte(EC_CONFIG_REGISTER);

  if (b)
  {
    retval = bitSet(config, EC_TEMP_COMPENSATION_CONFIG_BIT);
  }
  else
  {
    retval = bitClear(config, EC_TEMP_COMPENSATION_CONFIG_BIT);
  }

  _write_byte(EC_CONFIG_REGISTER, retval);
}

void uFire_EC::useDualPoint(bool b)
{
  uint8_t retval;
  uint8_t config = _read_byte(EC_CONFIG_REGISTER);

  if (b)
  {
    retval = bitSet(config, EC_DUALPOINT_CONFIG_BIT);
  }
  else
  {
    retval = bitClear(config, EC_DUALPOINT_CONFIG_BIT);
  }

  _write_byte(EC_CONFIG_REGISTER, retval);
}

uint8_t uFire_EC::getVersion()
{
  return _read_byte(EC_VERSION_REGISTER);
}

uint8_t uFire_EC::getFirmware()
{
  return _read_byte(EC_FW_VERSION_REGISTER);
}

void uFire_EC::reset()
{
  _write_register(EC_CALIBRATE_OFFSET_REGISTER, NAN);
  delay(10);
  _write_register(EC_CALIBRATE_REFHIGH_REGISTER, NAN);
  delay(10);
  _write_register(EC_CALIBRATE_REFLOW_REGISTER, NAN);
  delay(10);
  _write_register(EC_CALIBRATE_READHIGH_REGISTER, NAN);
  delay(10);
  _write_register(EC_CALIBRATE_READLOW_REGISTER, NAN);
  delay(10);
  setTempConstant(25.0);
  delay(10);
  setTempCoefficient(0.019);
  delay(10);
  useDualPoint(false);
  useTemperatureCompensation(false);
}

void uFire_EC::setCalibrateOffset(float offset)
{
  _write_register(EC_CALIBRATE_OFFSET_REGISTER, offset);
}

void uFire_EC::setTempConstant(float b)
{
  _write_register(EC_TEMP_COMPENSATION_REGISTER, b);
}

float uFire_EC::getTempConstant()
{
  return _read_register(EC_TEMP_COMPENSATION_REGISTER);
}

void uFire_EC::setI2CAddress(uint8_t i2cAddress)
{
  _write_register(EC_SOLUTION_REGISTER, i2cAddress);
  _send_command(EC_I2C);
  _address = i2cAddress;
}

bool uFire_EC::usingTemperatureCompensation()
{
  uint8_t retval;

  retval = _read_byte(EC_CONFIG_REGISTER);
  return (retval >> 1)  & 0x01;
}

bool uFire_EC::usingDualPoint()
{
  uint8_t retval;

  retval = _read_byte(EC_CONFIG_REGISTER);
  return (retval >> 0)  & 0x01;
}

bool uFire_EC::connected()
{
  uint8_t retval;

  retval = _read_byte(EC_VERSION_REGISTER);
  if (retval != 0xFF) {
    return true;
  }
  else {
    return false;
  }
}

float uFire_EC::readEEPROM(uint8_t address)
{
  _write_register(EC_SOLUTION_REGISTER, address);
  _send_command(EC_READ);
  return _read_register(EC_BUFFER_REGISTER);
}

void uFire_EC::writeEEPROM(uint8_t address, float value)
{
  _write_register(EC_SOLUTION_REGISTER, address);
  _write_register(EC_BUFFER_REGISTER,   value);
  _send_command(EC_WRITE);
}

void uFire_EC::setTempCoefficient(float temp_coef)
{
  _write_register(EC_TEMPCOEF_REGISTER, temp_coef);
}

float uFire_EC::getTempCoefficient()
{
  return _read_register(EC_TEMPCOEF_REGISTER);
}

void uFire_EC::_change_register(uint8_t r)
{
  Wire.beginTransmission(_address);
  Wire.write(r);
  Wire.endTransmission();
  delay(10);
}

void uFire_EC::_send_command(uint8_t command)
{
  Wire.beginTransmission(_address);
  Wire.write(EC_TASK_REGISTER);
  Wire.write(command);
  Wire.endTransmission();
  delay(10);
}

void uFire_EC::_write_register(uint8_t reg, float f)
{
  uint8_t b[5];
  float   f_val = f;

  b[0] = reg;
  b[1] = *((uint8_t *)&f_val);
  b[2] = *((uint8_t *)&f_val + 1);
  b[3] = *((uint8_t *)&f_val + 2);
  b[4] = *((uint8_t *)&f_val + 3);
  Wire.beginTransmission(_address);
  Wire.write(b, 5);
  Wire.endTransmission();
  delay(10);
}

float uFire_EC::_read_register(uint8_t reg)
{
  float retval;

  _change_register(reg);
  Wire.requestFrom(_address, (uint8_t)1);
  *((uint8_t *)&retval) = Wire.read();
  Wire.requestFrom(_address, (uint8_t)1);
  *((uint8_t *)&retval + 1) = Wire.read();
  Wire.requestFrom(_address, (uint8_t)1);
  *((uint8_t *)&retval + 2) = Wire.read();
  Wire.requestFrom(_address, (uint8_t)1);
  *((uint8_t *)&retval + 3) = Wire.read();
  delay(10);
  return retval;
}

void uFire_EC::_write_byte(uint8_t reg, uint8_t val)
{
  uint8_t b[5];

  b[0] = reg;
  b[1] = val;
  Wire.beginTransmission(_address);
  Wire.write(b, 2);
  Wire.endTransmission();
  delay(10);
}

uint8_t uFire_EC::_read_byte(uint8_t reg)
{
  uint8_t retval;

  _change_register(reg);
  Wire.requestFrom(_address, (uint8_t)1);
  retval = Wire.read();
  delay(10);
  return retval;
}
