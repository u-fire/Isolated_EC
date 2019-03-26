extern crate ufire_iso_ec;
extern crate shrust;
use shrust::{Shell, ShellIO};
use std::io::prelude::*;
use ufire_iso_ec::*;

fn main() {
	println!("Isolated EC Probe Interface shell");
	println!("type `help` for a list of commands");
	let v = vec![0x3c];
	let mut shell = Shell::new(v);
	shell.new_command_noargs("config", "Prints configuration data", move |io, v| {
		let mut ec = EcProbe::new("/dev/i2c-3", v[0]).unwrap();
		try!(writeln!(io, "Config:"));
		try!(writeln!(io, "	offset: {}", ec.get_calibrate_offset().unwrap()));
		try!(writeln!(io, "	dual point:"));
		try!(writeln!(io, "		low reference / read: {} / {}",
		ec.get_calibrate_low_reference().unwrap(),
		ec.get_calibrate_low_reading().unwrap()));
		try!(writeln!(io, "		high reference / read: {} / {}",
		ec.get_calibrate_high_reference().unwrap(),
		ec.get_calibrate_high_reading().unwrap()));
		try!(writeln!(io, "	temp. compensation:"));
		try!(writeln!(io, "		temp. constant: {}", ec.get_temp_constant().unwrap()));
		try!(writeln!(io, "		temp. coeffiecient: {}", ec.get_temp_coefficient().unwrap()));
		try!(writeln!(io, "	version hw.fw: {}.{}", ec.get_version().unwrap(),ec.get_firmware().unwrap()));
		Ok(())
	});

	shell.new_command_noargs("reset", "Resets all configuration data", move |_, v| {
		let mut ec = EcProbe::new("/dev/i2c-3", v[0]).unwrap();
		ec.reset().unwrap();
		Ok(())
	});

	shell.new_command_noargs("temp", "Measures temperature", move |io, v| {
		let mut ec = EcProbe::new("/dev/i2c-3", v[0]).unwrap();
		let temp_c = ec.measure_temp().unwrap();
		try!(writeln!(io, "C: {}", temp_c));
		Ok(())
	});

	shell.new_command("ec", "`ec <temp C>` : Measures EC", 1, move |io, v, s| {
		let mut ec = EcProbe::new("/dev/i2c-3", v[0]).unwrap();
		let ms = ec.measure_ec(s[0].parse().unwrap()).unwrap();
		try!(writeln!(io, "mS: {}", ms));
		Ok(())
	});

	shell.new_command("sal", "Measures salinity", 1, move |io, v, s| {
		let mut ec = EcProbe::new("/dev/i2c-3", v[0]).unwrap();
		let sal = ec.measure_salinity(s[0].parse().unwrap()).unwrap();
		try!(writeln!(io, "PSU: {}", sal));
		Ok(())
	});

	shell.new_command_noargs("raw", "Gets raw measurement", move |io, v| {
		let mut ec = EcProbe::new("/dev/i2c-3", v[0]).unwrap();
		let raw = ec.measure_raw().unwrap();
		try!(writeln!(io, "raw: {}", raw));
		Ok(())
	});

	shell.new_command("cal", "`cal <mS/cm>` : Calibrate the probe using a single point", 1, move |io, v, s| {
		let mut ec = EcProbe::new("/dev/i2c-3", v[0]).unwrap();
		ec.calibrate_single(s[0].parse().unwrap()).unwrap();
		try!(writeln!(io, "	offset: {}", ec.get_calibrate_offset().unwrap()));
		Ok(())
	});

	shell.new_command("const", "`const <temp C>` : Temperature constant", 1, move |io, v, s| {
		let mut ec = EcProbe::new("/dev/i2c-3", v[0]).unwrap();
		ec.set_temp_constant(s[0].parse().unwrap()).unwrap();
		try!(writeln!(io, "	temp. constant: {}", ec.get_temp_constant().unwrap()));
		Ok(())
	});

	shell.new_command("low", "`low <mS/cm>` : Dual point low calibration", 1, move |io, v, s| {
		let mut ec = EcProbe::new("/dev/i2c-3", v[0]).unwrap();
		ec.calibrate_probe_low(s[0].parse().unwrap()).unwrap();
		try!(writeln!(io, "	low reference / read: {} / {}",
		ec.get_calibrate_low_reference().unwrap(),
		ec.get_calibrate_low_reading().unwrap()));
		Ok(())
	});

	shell.new_command("high", "`high <mS/cm>` : Dual point high calibration", 1, move |io, v, s| {
		let mut ec = EcProbe::new("/dev/i2c-3", v[0]).unwrap();
		ec.calibrate_probe_high(s[0].parse().unwrap()).unwrap();
		try!(writeln!(io, "	high reference / read: {} / {}",
		ec.get_calibrate_high_reference().unwrap(),
		ec.get_calibrate_high_reading().unwrap()));
		Ok(())
	});

	shell.new_command("coef", "`coef <coefficient factor>` : Temperature coefficient", 1, move |io, v, s| {
		let mut ec = EcProbe::new("/dev/i2c-3", v[0]).unwrap();
		ec.set_temp_coefficient(s[0].parse().unwrap()).unwrap();
		try!(writeln!(io, "	temp. coefficient: {}", ec.get_temp_coefficient().unwrap()));
		Ok(())
	});

	shell.new_command_noargs("ver", "Hardware/Firmware of device", move |io, v| {
		let mut ec = EcProbe::new("/dev/i2c-3", v[0]).unwrap();
		try!(writeln!(io, "	version: {}.{}", ec.get_version().unwrap(),ec.get_firmware().unwrap()));
		Ok(())
	});

	shell.new_command("i2c", "`i2c <decimal I2C address>` : Changes the I2C address", 1, move |io, v, s| {
		let mut ec = EcProbe::new("/dev/i2c-3", v[0]).unwrap();
		v[0] = s[0].parse().unwrap();
		ec.set_i2c_address(v[0]).unwrap();
		try!(writeln!(io, "	I2C Address: {}", v[0]));
		Ok(())
	});

	shell.new_command("read", "`read <address>` : Reads an address in EEPROM", 1, move |io, v, s| {
		let mut ec = EcProbe::new("/dev/i2c-3", v[0]).unwrap();
		let eeprom = ec.read_eeprom(s[0].parse().unwrap()).unwrap();
		try!(writeln!(io, "{}", eeprom));
		Ok(())
	});

	shell.new_command("write", "`read ,address> <value>` : Changes the I2C address", 1, move |_, v, s| {
		let mut ec = EcProbe::new("/dev/i2c-3", v[0]).unwrap();
		ec.write_eeprom(s[0].parse().unwrap(), s[1].parse().unwrap()).unwrap();
		Ok(())
	});

    shell.run_loop(&mut ShellIO::default());
}
