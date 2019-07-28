#pragma once

#include <uFire_EC.h>

class uFire_EC_MP
{
public:
  float value;
  uFire_EC_MP(){}
  void begin(uFire_EC *ec);
  String processMP(String json);
private:
  uFire_EC *ec;
  String ec_reset();
  String ec_connected();
  String ec_temp_const(String);
  String ec_temp_coeff(String);
  String ec_high_ref(String);
  String ec_high_read();
  String ec_low_ref(String);
  String ec_low_read();
  String ec_measure(String);
  String ec_offset(String);
  String ec_temperature();
};
