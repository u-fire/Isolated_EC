from uFire_EC import uFire_EC

# Be sure to follow the directions here:
# https://ufire.co/docs/uFire_EC/#raspberry-pi

# For Raspberry Pi 3, 3 indicates the I2C-GPIO port
# on Raspberry Pi 3, this is always 3 after following the above link's instruction
# on Raspberry Pi 4, it can't be determined ahead of time
# from a terminal, type `ls /dev/i2c <tab>` after pressing <tab>, you should
# get a listing of available I2C busses. 

ec = uFire_EC(i2c_bus=3)

ec.measureEC()
print("mS: " + str(ec.mS))
