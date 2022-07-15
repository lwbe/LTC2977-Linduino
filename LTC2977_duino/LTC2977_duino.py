#! coding: utf-8

import serial
import sys, os
import time
sys.path.append(os.path.dirname(__file__))
import binary_utils
import xml.etree.ElementTree as ET

desc_file="ltc2977.desc"

class LTC2977_duino(object):
    type_convert = {
        "R/W Byte"  : "WB",
        "R Byte"    : "WB",
        "Send Byte" : "SB",
        "R/W Word"  : "WW",
        "R Word"    : "WW",
        "R Block"   : "Block"
    }

    proj_global_cmd = [
        'WRITE_PROTECT',   
        'VIN_ON',
        'VIN_OFF',
        'OT_FAULT_LIMIT',               #_GLOBAL',
        'OT_FAULT_RESPONSE',            #_GLOBAL',
        'OT_WARN_LIMIT',                #_GLOBAL',
        'UT_WARN_LIMIT',                #_GLOBAL',
        'UT_FAULT_LIMIT',               #_GLOBAL',
        'UT_FAULT_RESPONSE',            #_GLOBAL',
        'VIN_OV_FAULT_LIMIT',
        'VIN_OV_FAULT_RESPONSE',        #_GLOBAL',
        'VIN_OV_WARN_LIMIT',
        'VIN_UV_WARN_LIMIT',
        'VIN_UV_FAULT_LIMIT',
        'VIN_UV_FAULT_RESPONSE',        #_GLOBAL',
        'USER_DATA_00',
        'USER_DATA_02',
        'USER_DATA_04',
        'MFR_CONFIG_ALL_LTC2977',
        'MFR_PWRGD_EN',                 #_LTC2978',
        'MFR_FAULTB10_RESPONSE',        #_LTC2978',
        'MFR_FAULTB11_RESPONSE',        # _LTC2978',
        'MFR_VINEN_OV_FAULT_RESPONSE',  #__LTC2978',
        'MFR_VINEN_UV_FAULT_RESPONSE',  #_LTC2978',
        'MFR_RETRY_DELAY',              #_LTC_GLOBAL',
        'MFR_RESTART_DELAY',            #_GLOBAL',
        'MFR_POWERGOOD_ASSERTION_DELAY',#_LTC',
        'MFR_WATCHDOG_T_FIRST',         #_LTC',
        'MFR_WATCHDOG_T',               #_LTC',
        'MFR_PAGE_FF_MASK',             #_LTC2978',
        'MFR_I2C_BASE_ADDRESS',         #_LTC',
        'MFR_RETRY_COUNT'               #_LTC'
    ]
    
    proj_paged_cmd = [
        'OPERATION',
        'ON_OFF_CONFIG',
        'VOUT_COMMAND',
        'VOUT_MAX',
        'VOUT_MARGIN_HIGH', 
        'VOUT_MARGIN_LOW',
        'VOUT_OV_FAULT_LIMIT', 
        'VOUT_OV_FAULT_RESPONSE', 
        'VOUT_OV_WARN_LIMIT',
        'VOUT_UV_WARN_LIMIT',
        'VOUT_UV_FAULT_LIMIT',
        'VOUT_UV_FAULT_RESPONSE', 
        'POWER_GOOD_ON',
        'POWER_GOOD_OFF', 
        'TON_DELAY', 
        'TON_RISE',
        'TON_MAX_FAULT_LIMIT',
        'TON_MAX_FAULT_RESPONSE', 
        'TOFF_DELAY',
        'USER_DATA_01',
        'USER_DATA_03',
        'MFR_CONFIG_LTC2977',
        'MFR_FAULTBz0_PROPAGATE',       #_LTC2978',
        'MFR_FAULTBz1_PROPAGATE',       #_LTC2978', 
        'MFR_DAC',                      #_LTC',
        'MFR_VOUT_DISCHARGE_THRESHOLD'  #_LTC'
    ]
    

    def __init__(self, port="/dev/ttyUSB0", baudrate=115900, timeout=5):

        try:
            self.ser = serial.Serial(port=port, baudrate=baudrate, timeout=timeout)
        except Exception as e:
            print("Couldn't open serial connection\nError: %s\nAborting" % e)
            sys.exit(0)
            
        self.writeEOL = "\n"
        self.readEOL = "\r\n"
        time.sleep(2)
        self.ser.flushInput()
        self.ser.flushOutput()
        self.description_cmd_code={}
        self.description_cmd_name={}
        self.cmd_names=[]
        des = [i for i in open(desc_file).readlines() if not i.startswith("#")]
        for d in des:
            # COMMAND NAME:CMD:TYPE:PAGED:DA Type:UNITS:EEPROM:DEFAULT VALUE:REF PAGE

            t = [i.strip() for i in d.split(":")]
            self.description_cmd_code[t[1]] = {
                'name'       : t[0],
                'type'       : self.type_convert[t[2]],
                'dataformat' : t[4]
            }
            self.description_cmd_name[t[0]] = {
                'code'       : t[1],
                'type'       : self.type_convert[t[2]],
                'dataformat' : t[4]
            }
            self.cmd_names.append(t[0])

        
    # internals
    def __write(self,line):
        b = bytes(line+self.writeEOL, "utf-8")
        return self.ser.write(b)

    def __read(self):
        s = self.ser.readline()
        return s.decode().replace(self.readEOL,"")

    def __convert(self,dataformat,v):
        if dataformat == 'L11':
            return binary_utils.L11ToFloat(int(v))
        elif dataformat == 'L16':
            return binary_utils.L16ToFloat(int(v))
        else:
            return int(v)

    def __wconvert(self,dataformat,v):
        if dataformat == 'L11':
            return binary_utils.FloatToL11(float(v))
        elif dataformat == 'L16':
            return binary_utils.FloatToL16(float(v))
        else:
            return v
    # public
    def scan(self):
        self.__write("scan")
        return self.__read()
    
    def clear_faults(self):
        self.__write("clear_faults")
        return self.__read()

    def store(self):
        self.__write("store")
        return self.__read()
        

    # write read function    
    def raw_write(self,data):
        self.__write("write:"+data)
        #time.sleep(.1)
        return self.__read()
                
    def raw_read(self,data):
        self.__write("read:"+data)
        #time.sleep(.1)
        return self.__read()
    
    def i2c_write(self, address, *values):
        self.__write("i2c_write:%d,%s" % (address,",".join(["%d"%i for i in values])))
        return self.__read()

    def i2c_read(self, address, *values):
        self.__write("i2c_read:%d,%s" % (address,",".join(["%d"%i for i in values])))
        return self.__read()


    def write(self,ltc,page,cmd,value):
        d = self.description_cmd_name[cmd]
        if not d:
            return "error: %s is unknow" % cmd
        value = self.__wconvert(d['dataformat'], value)
        self.__write("write:%d,%d,%s,%s,%d" % (ltc, page, d['type'], d['code'], value))
        return self.__read()

    def read(self,ltc,page,cmd):
        d = self.description_cmd_name[cmd]
        if not d:
            return "error: %s is unknow" % cmd
        self.__write("read:%d,%d,%s,%s" % (ltc,page,d['type'],d['code']))
        #time.sleep(.1)
        retval = self.__read()
        return int(retval)

    def read_h(self,ltc,page,cmd):
        d = self.description_cmd_name[cmd]
        if not d:
            return "error: %s is unknow" % cmd
        self.__write("read:%d,%d,%s,%s" % (ltc,page,d['type'],d['code']))
        #time.sleep(.1)
        retval = self.__read()
        return self.__convert(d['dataformat'], retval)

    def dump_txt(self,ltc_i2c):
        """
        dump the values of the ltc following the .txt file of the LTPowerPlay
        """
        
        for p in range(-1,8):
            if p == -1:
                cmd_names = self.proj_global_cmd
            else:
                cmd_names = self.proj_paged_cmd
            for i in cmd_names:
                cmd_code = self.description_cmd_name[i]['code']
                cmd_type = self.description_cmd_name[i]['type']
                value = self.raw_read("%s,%s,%s,%s" % ( ltc_i2c,str(p),cmd_type,cmd_code))
                
                print( "%s,%s,%s,%s,%s,%s" % (hex(ltc_i2c),p,cmd_type,cmd_code,hex(int(value)),i))
                                            
    def dump(self,ltc_i2c):
        """
        dump the values of the ltc in human readable form
        """
        dump_list = [] 
        for p in range(-1,8):
            if p == -1:
                cmd_names = self.proj_global_cmd
            else:
                cmd_names = self.proj_paged_cmd
            for i in cmd_names:
                d = self.description_cmd_name[i]
                cmd_code = d['code']
                cmd_type = d['type']

                value = self.read( ltc_i2c,p,i)
                dump_list.append( "%s,%s,%s,%s" % (hex(ltc_i2c),p,i,value))

        return dump_list

    def dump_to_file(self,ltcs,filename):
        
        data=[]
        if not isinstance(ltcs,list):
            ltcs = [ltcs]
        for l in ltcs:
            data.extend(self.dump(l))
        with open(filename, 'w') as outfile:
            for line in data:
                outfile.write(line+"\n")
                
    def load_from_file(self,filename):
        
        with open(filename, 'r', encoding='utf-8') as infile:
            for line in infile:
                if not line.startswith("#"):
                    self.write(*line.strip().split(","))
                    time.sleep(0.1)

    def load_from_txt(self,filename):
        #return "untested !!"
        with open(filename, 'r', encoding='utf-8') as infile:
            for line in infile:
                if not line.startswith("#"):
                    self.raw_write(line.strip())


    def load_from_proj(self,filename):
    #    return "untested !!"
        tree = ET.parse(filename)
        root=tree.getroot()
        
        Chips=[i for i in  root if i.tag=="Chip"]
        for c in Chips:
            addr = c.attrib['addr7bit']
            for p in c:
                if (p.tag=='Global'):
                    page=-1
                else:
                    page=p.attrib['num']
                for r in list(p):
                    if r.attrib['len']=='1':
                        le="WB"
                    else:
                        le="WW"
                    cmd = r.attrib['cmd']
                    print("%s,%s,%s,%s,%s,%s" % (addr,page,le,cmd,r.attrib['hex'],self.description_cmd_code[cmd]['name']))

if __name__== "__main__":
    fs = LTC2977_duino()
    fs.scan()
    #fs.description.keys()
    #    print(fs.raw_read("0x33,1,WB,0x01"))
    #    print(fs.raw_write("0x33,-1,WB,0x01,0x80"))

    #print(fs.raw_read("0x33,0,WW,0x21"))
    #fs.write("0x33,0,WW,0x21,1.22")
    #print(fs.raw_read("0x33,0,WW,0x21"))

    #fs.dump_proj(0x33)
