The feb_serial module works with the FEB_CONFIGURATOR_2.ino sketche

The protocol in this sketch is

"write"
"read"
"scan"
"alert"
"clear_faults"
"store"
"read_values"




import feb_serial
f=feb_serial.feb_serial("/dev/ttyUSB1")

# scan bus
f.scan()

# typical Out[10]: '35:51:80:81:82:83:84:85:86:87:91'

# to read a value VOUT_COMMAND from the ltc of i2c address 0x33 and page 0x1 
f.read(0x33,0x1,"VOUT_COMMAND")


### Test

    f=feb_serial.feb_serial("COM5")
    f.i2c_read(0x26,0x00)
    # -> '255'
    f.i2c_read(0x26,0x09)
    # -> '1'
    # f.i2c_write(0x26,0x00,0x00)
    # -> 'ok'
    f.i2c_read(0x26,0x00)
    # -> '0'
    f.i2c_write(0x26,0x09,0x00)
    # -> 'ok'
    f.read_h(0x30,0x2,"READ_VOUT")
    # -> 0.019775390625
    f.read_h(0x32,0x2,"READ_VOUT")
    #-> 0.019775390625
    f.i2c_write(0x26,0x09,0x01)
    'ok'
    f.read_h(0x30,0x2,"READ_VOUT")
    1.2000732421875
    f.read_h(0x32,0x2,"READ_VOUT")
    0.0

#### PMBUS et I2C

##### I2C

    f=feb_serial.feb_serial("COM5")
    f.read(0x30,0x00,"READ_VOUT")
    8194
    f.read_h(0x30,0x00,"READ_VOUT")
    0.9998779296875
    f.read_h(0x30,0x01,"READ_VOUT")
    1.099853515625
    'ok'
    f.i2c_write(0x30,0x00,0x00)
    'ok'
    f.i2c_write(0x30,0x01,0x40)
    'ok'
    f.i2c_write(0x30,0x00,0xFF)
    'ok'
    f.i2c_write(0x30,0x01,0x40)
    'ok'
    f.i2c_write(0x5B,0x00,0xFF)
    'ok'
    f.i2c_write(0x5B,0x01,0x80)
    'ok'

#### PMBUS

    f=feb_serial.feb_serial("COM5")
    f.read_h(0x30,0x00,"READ_VOUT")
    1.000244140625
    f.write(0x5B,0xFF,"OPERATION",0x40)
    'ok'
    f.write(0x5B,0xFF,"OPERATION",0x80)
    'ok'
    f.write(0x5B,0xFF,"OPERATION",0x40)
    'ok'
