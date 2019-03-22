import cmd
from uFire_EC import uFire_EC

ec = uFire_EC()


class ECShell(cmd.Cmd):
    prompt = '> '

    def do_config(self, a):
        """prints out all the configuration data\nparameters: none"""
        print("EC Interface Config: " + 'connected' if ec.connected() else '**disconnected**')
        print("calibration: ")
        print("\toffset: " + str(ec.getCalibrateOffset()))
        print("\tdual point: ")
        print("\tlow reference / read: " + str(ec.getCalibrateLowReference()
                                               ) + " / " + str(ec.getCalibrateLowReading()))
        print("\thigh reference / reading: " + str(ec.getCalibrateHighReference()
                                                   ) + " / " + str(ec.getCalibrateHighReading()))
        print("\ttemp. compensation: ")
        print("\t\tconstant: " + str(ec.getTempConstant()))
        print("\t\tcoefficient: " + str(ec.getTempCoefficient()))
        print("\tversion: " + (str(ec.getVersion())) + "." + (str(ec.getFirmware())))

    def do_reset(self, a):
        """reset all saved values\nparameters: none"""
        ec.reset()

    def do_temp(self, temp_C):
        """measures the temperature\nparameters: none"""
        ec.measureTemp()

        print("C/F: " + str(ec.tempC) + " / " + str(ec.tempF))

    def do_raw(self, a):
        """starts an EC measurement\nparameters: none"""
        ec.measureEC()
        print("raw: " + str(ec.raw))

    def do_ec(self, line):
        """starts an EC measurement\nparameters: temp, temp_constant"""
        data = [float(s) for s in line.split()]
        if len(data) == 1:
            ec.measureEC(float(data[0]))
        elif len(data) == 2:
            ec.measureEC(float(data[0]), float(data[1]))
        else:
            ec.measureEC()

        print("mS: " + str(ec.mS))

    def do_low(self, low_reference_mS):
        """returns or sets the low reference/reading values\nparameters\n\tlow reference solution in mS"""
        if low_reference_mS:
            ec.calibrateProbeLow(float(low_reference_mS))
        print("\tlow reference / read: " + str(ec.getCalibrateLowReference()
                                               ) + " / " + str(ec.getCalibrateLowReading()))

    def do_high(self, high_reference_mS):
        """returns or sets the high referencen/reading values\nparameters\n\thigh reference solution in mS"""
        if high_reference_mS:
            ec.calibrateProbeHigh(float(high_reference_mS))
        print("\thigh reference / reading: " + str(ec.getCalibrateHighReference()
                                                   ) + " / " + str(ec.getCalibrateHighReading()))

    def do_cal(self, solution_mS):
        """calibrates the device\nparameters:\n\tcalibration solution in mS"""
        if solution_mS:
            ec.calibrateProbe(float(solution_mS))

        print("\toffset: " + str(ec.getCalibrateOffset()))

    def do_coef(self, tcoefficient):
        """calibrates the device\nparameters:\n\tcoefficient"""
        if tcoefficient:
            ec.setTempCoefficient(float(tcoefficient))

        print("\tcoefficient: " + str(ec.getTempCoefficient()))

    def do_data(self, a):
        """prints all the data registers"""
        print("raw: " + str(ec.raw))
        print("S: " + str(ec.S))
        print("mS: " + str(ec.mS))
        print("uS: " + str(ec.uS))
        print("TDS 500 | 640 | 700: " + str(ec.PPM_500) + " | " + str(ec.PPM_640) + " | " + str(ec.PPM_700))
        print("salinity PSU: " + str(ec.salinityPSU))
        print("C/F: " + str(ec.tempC) + " / " + str(ec.tempF))

    def do_version(self, a):
        """prints the version register"""
        print("\tversion: " + (str(ec.getVersion())) + "." + (str(ec.getFirmware())))

    def do_i2c(self, i2cAddress):
        """changes the I2C address"""
        ec.setI2CAddress(i2cAddress)

    def do_read(self, address):
        print(ec.readEEPROM(address))

    def do_write(self, arg):
        a = arg.split()
        ec.writeEEPROM(a[0], a[1])

    def do_EOF(self, line):
        return True


ECShell().cmdloop()
