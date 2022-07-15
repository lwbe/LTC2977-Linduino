// definition of COMMAND CODE for the LTC2977 from the datasheet
#ifndef LTC2977_LINDUINO_INCLUDE
#define LTC2977_LINDUINO_INCLUDE


#include <stdint.h>
#include <Wire.h>

/* define for commands */


#define PAGE                    0x00
#define OPERATION               0x01
#define ON_OFF_CONFIG           0x02
#define CLEAR_FAULTS            0x02
#define WRITE_PROTECT           0x10
#define STORE_USER_ALL          0x15
#define RESTORE_USER_ALL        0x16

#define VOUT_MODE               0x20
#define VOUT_COMMAND            0x21

#define VOUT_MARGIN_HIGH        0x25
#define VOUT_MARGIN_LOW         0x26

#define VOUT_OV_FAULT_LIMIT     0x40
#define VOUT_OV_FAULT_RESPONSE  0x41
#define VOUT_OV_WARN_LIMIT      0x42
#define VOUT_UV_WARN_LIMIT      0x43
#define VOUT_UV_FAULT_LIMIT     0x44
#define VOUT_UV_FAULT_RESPONSE  0x45

#define OT_FAULT_LIMIT          0x4F
#define OT_FAULT_RESPONSE       0x50
#define OT_WARN_LIMIT           0x51
#define UT_WARN_LIMIT           0x52
#define UT_FAULT_LIMIT          0x53
#define UT_FAULT_RESPONSE       0x54

#define VIN_OV_FAULT_LIMIT      0x55
#define VIN_OV_FAULT_RESPONSE   0x56
#define VIN_OV_WARN_LIMIT       0x57
#define VIN_UV_WARN_LIMIT       0x58
#define VIN_UV_FAULT_LIMIT      0x59
#define VIN_UV_FAULT_RESPONSE   0x5A

#define TON_DELAY               0x60
#define TOFF_DELAY              0x64

#define STATUS_BYTE             0x78
#define STATUS_WORD             0x79
#define STATUS_VOUT             0x7A
#define STATUS_INPUT            0x7C
#define STATUS_TEMPERATURE      0x7D
#define STATUS_CML              0x7E

#define READ_VIN                0x88
#define READ_VOUT               0x8B
#define READ_TEMPERATURE_1      0x8D

#define MFR_FAULTBz0_PROPAGATE  0xD2
#define MFR_FAULTBz1_PROPAGATE  0xD3

#define MFR_VOUT_PEAK           0xDD
#define MFR_VIN_PEAK            0xDE
#define MFR_TEMPERATURE_PEAK    0xDF

#define MFR_PAGE_FF_MASK        0xE4
#define MFR_PADS                0xE5
#define MFR_I2C_BASE_ADDRESS    0xE6
#define MFR_FAULT_LOG_CLEAR     0xEC

#define MFR_VOUT_MIN            0xFB
#define MFR_VIN_MIN             0xFC
#define MFR_TEMPERATURE_MIN     0xFD




bool checkAddress(uint8_t address);
void ScanI2CBus(uint8_t start_addr, uint8_t end_addr);
void Scan_ALL_I2CBus();
/*--------------------------------------------------------------------------------------------------------*/
/*
  The folowing function extract values from the ltc2977 following the information in the documentation.

  PLEASE NOTE that these functions applies on the current page
*/
/*--------------------------------------------------------------------------------------------------------*/
int8_t read_VOUT_MODE(uint8_t address);
/*--------------------------------------------------------------------------------------------------------*/
float read_READ_VOUT(uint8_t address);
/*--------------------------------------------------------------------------------------------------------*/
float read_READ_VOUT_Values(uint8_t address,uint8_t command);

/*
  bit operations
*/

uint16_t extract_bits_from_value(uint16_t val, uint8_t start, uint8_t length);

int16_t two_complement(uint16_t val, uint8_t length);
void print_word_in_bits(uint16_t num);


/*
  I2C functions
*/

void scanI2C(uint8_t start, uint8_t stop);
uint8_t I2C_Send(uint8_t address);
uint8_t I2C_Write(uint8_t n, char **values);
uint8_t I2C_ReadByte(uint8_t n, char **values);
uint16_t I2C_ReadWord(uint8_t n, char **values);

/*
  PMBUS read/write function reimplementation using Wire from arduino.
  NOTE this is not a "real" reimplementation of PMBUS Specification.
 */

uint8_t PMBUS_SendByte(uint8_t address, uint8_t command);

uint8_t PMBUS_WriteByte(uint8_t address, uint8_t command, uint8_t val);

uint8_t PMBUS_WriteWord(uint8_t address, uint8_t command, uint16_t val);

uint8_t PMBUS_ReadByte(uint8_t address, uint8_t command);

uint16_t PMBUS_ReadWord(uint8_t address, uint8_t command);


/* 
  conversion function from float to L11 or L16 values
  function extracted from LinearTechnology code
*/

// L16
float L16_to_Float_mode(uint8_t vout_mode, uint16_t input_val);
uint16_t Float_to_L16_mode(uint8_t vout_mode, float input_val);

// L11
float L11_to_Float(uint16_t input_val);
uint16_t Float_to_L11(float input_val);

#endif
