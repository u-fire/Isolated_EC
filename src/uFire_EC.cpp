#include "uFire_EC.h"

const float uFire_EC::tempCoefEC       = 0.019;
const float uFire_EC::tempCoefSalinity = 0.021;

bool uFire_EC::begin(uint8_t address, TwoWire &wirePort)
{
  _address = address;
  _i2cPort = &wirePort;

  if (getVersion() <= 2)
  {
    _ec_delay = 750;
  }
  else
  {
    _ec_delay = 500;
  }

  return connected();
}

float uFire_EC::measureEC(float temp, float temp_constant)
{
  setTemp(temp);
  useTemperatureCompensation(true);
  setTempConstant(temp_constant);
  _send_command(EC_MEASURE_EC);
  if(_blocking) delay(_ec_delay);

  _updateRegisters();

  return mS;
}

float uFire_EC::measureTemp()
{
  _send_command(EC_MEASURE_TEMP);
  if(_blocking) delay(EC_TEMP_MEASURE_TIME);

  _updateRegisters();

  return tempC;
}

void uFire_EC::setTemp(float temp_C)
{
  _write_register(EC_TEMP_REGISTER, temp_C);
  tempC = temp_C;
  tempF = ((tempC * 9) / 5) + 32;
}

float uFire_EC::calibrateProbe(float solutionEC, float tempC)
{
  solutionEC = _mS_to_mS25(solutionEC, tempC);
  _write_register(EC_SOLUTION_REGISTER, solutionEC);
  _send_command(EC_CALIBRATE_PROBE);
  if(_blocking) delay(_ec_delay);

  return getCalibrateOffset();
}

float uFire_EC::calibrateProbeLow(float solutionEC, float tempC)
{
  solutionEC = _mS_to_mS25(solutionEC, tempC);
  _write_register(EC_SOLUTION_REGISTER, solutionEC);
  _send_command(EC_CALIBRATE_LOW);
  if(_blocking) delay(_ec_delay);

  return getCalibrateLowReading();
}

float uFire_EC::calibrateProbeHigh(float solutionEC, float tempC)
{
  solutionEC = _mS_to_mS25(solutionEC, tempC);
  _write_register(EC_SOLUTION_REGISTER, solutionEC);
  _send_command(EC_CALIBRATE_HIGH);
  if(_blocking) delay(_ec_delay);

  return getCalibrateHighReading();
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

void uFire_EC::setBlocking(bool b)
{
   _blocking = b;
}

bool uFire_EC::getBlocking()
{
    return _blocking;
}

void uFire_EC::readData()
{
  _updateRegisters();
  getCalibrateHighReading();
  getCalibrateHighReference();
  getCalibrateLowReading();
  getCalibrateLowReference();
  getCalibrateOffset();
}

float uFire_EC::_mS_to_mS25(float mS, float tempC)
{
  return mS / (1 - (getTempCoefficient() * (tempC - 25)));
}

void uFire_EC::_updateRegisters()
{
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

  tempC = _read_register(EC_TEMP_REGISTER);
  if (tempC == -127.0)
  {
    tempF = -127;
  }
  else
  {
    tempF = ((tempC * 9) / 5) + 32;
  }
}

void uFire_EC::_change_register(uint8_t r)
{
  _i2cPort->beginTransmission(_address);
  _i2cPort->write(r);
  _i2cPort->endTransmission();
  //delay(10);
}

void uFire_EC::_send_command(uint8_t command)
{
  _i2cPort->beginTransmission(_address);
  _i2cPort->write(EC_TASK_REGISTER);
  _i2cPort->write(command);
  _i2cPort->endTransmission();
  //delay(10);
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
  _i2cPort->beginTransmission(_address);
  _i2cPort->write(b, 5);
  _i2cPort->endTransmission();
  //delay(10);
}

float uFire_EC::_read_register(uint8_t reg)
{
  float retval;

  _change_register(reg);
  _i2cPort->requestFrom(_address, (uint8_t)1);
  *((uint8_t *)&retval) = _i2cPort->read();
  _i2cPort->requestFrom(_address, (uint8_t)1);
  *((uint8_t *)&retval + 1) = _i2cPort->read();
  _i2cPort->requestFrom(_address, (uint8_t)1);
  *((uint8_t *)&retval + 2) = _i2cPort->read();
  _i2cPort->requestFrom(_address, (uint8_t)1);
  *((uint8_t *)&retval + 3) = _i2cPort->read();
  //delay(10);
  return retval;
}

void uFire_EC::_write_byte(uint8_t reg, uint8_t val)
{
  uint8_t b[5];

  b[0] = reg;
  b[1] = val;
  _i2cPort->beginTransmission(_address);
  _i2cPort->write(b, 2);
  _i2cPort->endTransmission();
  //delay(10);
}

uint8_t uFire_EC::_read_byte(uint8_t reg)
{
  uint8_t retval;

  _change_register(reg);
  _i2cPort->requestFrom(_address, (uint8_t)1);
  retval = _i2cPort->read();
  //delay(10);
  return retval;
}