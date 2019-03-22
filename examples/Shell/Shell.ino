/*
   This allows you to run various functions on a command-line like interface.
   Enter:
   `config` to see the configuration of the device.
   'ec' to get a measurement in mS
   'data' to see all the data  members of the class like uS, and S
   'temp' to read the temperature
   'reset' to reset all the configuration stored in the device

   Single Point Calibration:
    cal 2.0 <to calibrate in a 2.0 mS/cm solution>

   Dual Point Calibration:
    low 0.7 <to calibrate the low value as 0.7 mS/cm>
    high 2.0 <to calibrate the high value as 2.0 mS/cm>

   Measure Temperature:
    temp

   Measure EC:
    ec
    ec <current temperature> <temperature to adjust to>

   Display results in PPM 500:
    tds

   Change the I2C address:
    i2c 3d <address is a interpreted as a hex number without the 0x>

   Read/Write custom EEPROM data:
    read 300 <returns a float stored at address 300>
    write 300 123.4 <writes 123.4 at address 300>
 */

#include <Arduino.h>
#include "uFire_EC.h"

#ifdef ESP32
EC_Salinity EC(19, 23); // sda, scl
#else // ifdef ESP32
uFire_EC EC;
#endif // ifdef ESP32

String buffer, cmd, p1, p2;

void config()
{
  Serial.print("EC Probe Interface: ");
  Serial.println(EC.connected() ? "connected" : "*disconnected*");
  Serial.println("calibration:");
  Serial.print("  offset: "); Serial.println(EC.getCalibrateOffset(), 4);
  Serial.println("  dual point: ");
  Serial.print("    low reference / read: "); Serial.print(EC.getCalibrateLowReference(), 4);
  Serial.print(" /  "); Serial.println(EC.getCalibrateLowReading(), 4);
  Serial.print("    high reference / read: "); Serial.print(EC.getCalibrateHighReference(), 4);
  Serial.print(" / "); Serial.println(EC.getCalibrateHighReading(), 4);
  Serial.println("temp. compensation: ");
  Serial.print("    constant: ");
  Serial.println(EC.getTempConstant());
  Serial.print("    coefficient: ");
  Serial.println(EC.getTempCoefficient(), 3);
  Serial.print("hardware/firmware version: ");
  Serial.print(EC.getVersion(), HEX);
  Serial.print(":");
  Serial.println(EC.getFirmware(), HEX);
}

void config_reset()
{
  EC.reset();
}

void temperature()
{
  EC.measureTemp();
  Serial.print("C|F: ");
  Serial.print(EC.tempC);
  Serial.print(" | ");
  Serial.println(EC.tempF);
}

void data()
{
  Serial.print("raw: ");
  Serial.println(EC.raw);
  Serial.print(" S/cm: ");
  Serial.println(EC.S, 4);
  Serial.print("mS/cm: ");
  Serial.println(EC.mS, 4);
  Serial.print("uS/cm: ");
  Serial.println(EC.uS);
  Serial.print("TDS PPM 500|640|700: ");
  Serial.print(EC.PPM_500);
  Serial.print(" | ");
  Serial.print(EC.PPM_640);
  Serial.print(" | ");
  Serial.println(EC.PPM_700);
  Serial.print("salinity PSU: ");
  Serial.println(EC.salinityPSU, 4);
  Serial.print("C|F: ");
  Serial.print(EC.tempC);
  Serial.print(" |  ");
  Serial.println(EC.tempF);
}

void i2c()
{
  if (p1.length())
  {
    EC.setI2CAddress(strtoul(p1.c_str(), 0, 16));
  }
}

void ec()
{
  while (Serial.available() == 0)
  {
    if (p2.length())
    {
      EC.measureEC(p1.toFloat(), p2.toFloat());
      Serial.print("mS/cm: ");
      Serial.println(EC.mS, 4);
    }
    else if (p1.length())
    {
      EC.measureEC(p1.toFloat());
      Serial.print("mS/cm: ");
      Serial.println(EC.mS, 4);
    }
    else
    {
      EC.measureEC();
      Serial.print("mS/cm: ");
      Serial.println(EC.mS, 4);
    }
    delay(1000);
  }
}

void tds()
{
    while (Serial.available() == 0)
    {
      EC.measureEC();
      Serial.print("tds: ");
      Serial.println(EC.PPM_500);
      delay(1000);
    }
}

void raw()
{
    while (Serial.available() == 0)
    {
      EC.measureEC();
      Serial.print("raw: ");
      Serial.println(EC.raw);
      delay(1000);
    }
}

void low() {
  if (p1.length()) 
  {
    EC.calibrateProbeLow(p1.toFloat());
  }
  Serial.print("low reference | read: "); Serial.print(EC.getCalibrateLowReference(), 2);
  Serial.print(" | "); Serial.println(EC.getCalibrateLowReading(), 2);
}

void high() {
  if (p1.length()) 
  {
    EC.calibrateProbeHigh(p1.toFloat());
  }
  Serial.print("high reference | read: "); Serial.print(EC.getCalibrateHighReference(), 2);
  Serial.print(" | "); Serial.println(EC.getCalibrateHighReading(), 2);
}

void calibrate() {
  if (p1.length()) 
  {
    EC.calibrateProbe(p1.toFloat());
  }

  Serial.print("offset: ");
  Serial.println(EC.getCalibrateOffset(), 5);
}

void coeffieicnet() {
  if (p1.length()) 
  {
    EC.setTempCoefficient(p1.toFloat());
  }

  Serial.print("coefficient: ");
  Serial.println(EC.getTempCoefficient(), 5);
}

void read()
{
  if (p1.length())
  {
    Serial.println(EC.readEEPROM(p1.toFloat()));
  }
}

void write()
{
  if (p1.length())
  {
    EC.writeEEPROM(p1.toInt(), p2.toFloat());
  }
}

void cmd_run()
{
  if ((cmd == "conf") || (cmd == "config") || (cmd == "c")) config();
  if ((cmd == "reset") || (cmd == "r")) config_reset();
  if ((cmd == "temp") || (cmd == "t")) temperature();
  if ((cmd == "data") || (cmd == "d")) data();
  if ((cmd == "calibrate") || (cmd == "cal")) calibrate();
  if (cmd == "low") low();
  if (cmd == "high") high();
  if (cmd == "i2c") i2c();
  if (cmd == "ec") ec();
  if (cmd == "raw") raw();
  if (cmd == "read") read();
  if (cmd == "write") write();
  if (cmd == "tds") tds();
  if (cmd == "coef") coeffieicnet();
}

void cli_process()
{
  while (Serial.available())
  {
    char c = Serial.read();

    switch (c)
    {
    case '\n': // new line
      Serial.println();
      cmd = buffer.substring(0, buffer.indexOf(" ", 0));
      cmd.trim();
      buffer.remove(0, buffer.indexOf(" ", 0));
      buffer.trim();
      p1 = buffer.substring(0, buffer.indexOf(" ", 0));
      p1.trim();
      buffer.remove(0, buffer.indexOf(" ", 0));
      buffer.trim();
      p2 = buffer.substring(0, buffer.indexOf(" ", 0));
      p2.trim();
      cmd_run();
      Serial.print("> ");
      buffer = "";
      break;

    case '\b': // backspace
      buffer.remove(buffer.length() - 1);
      Serial.print("\b \b");
      break;

    default: // everything else
      buffer += c;
      Serial.print(c);
    }
  }
}

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  config();
  Serial.print("> ");
}

void loop()
{
  cli_process();
}
