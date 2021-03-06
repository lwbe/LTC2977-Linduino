/*
  bit operations
*/
#include <stdint.h>
#include <Wire.h>
#include <math.h> 
#include <Arduino.h>
#include "LTC2977_duino.h"
/*--------------------------------------------------------------------------------------------------------*/
/*
 * Utility function useful for debug
 */
/*--------------------------------------------------------------------------------------------------------*/

bool checkAddress(uint8_t address){
  Wire.beginTransmission(address);
  if (Wire.endTransmission() == 0){
    return true;
  } else {
    return false;
  }
}

void ScanI2CBus(uint8_t start_addr, uint8_t end_addr) {
  // scan the bus
  uint8_t retval;
  for (uint8_t address = start_addr ; address < end_addr ; address++) {
    Serial.print(F("I2C Address : 0x"));
    Serial.print(address, HEX);
    Serial.print(F(" ("));
    Serial.print(address, DEC);
    Serial.print(F(") status : "));

    Wire.beginTransmission(address);
    retval = Wire.endTransmission();
    Serial.print(retval);

    // return value of the wire endtransmission return the status of the line
    switch (retval)
    {
      case 0:
        Serial.println(F(" - Success"));
        break;
      case 1:
        Serial.println(F(" - Data too long"));
        break;
      case 2:
        Serial.println(F(" - NACK for address"));
        break;
      case 3:
        Serial.println(F(" - NACK for data"));
        break;
      case 4:
        Serial.println(" - Other Error");
        break;
    }
    delay(300);
  }
}

void Scan_ALL_I2CBus(){
  ScanI2CBus(0,127);
}


/*--------------------------------------------------------------------------------------------------------*/
/*
  The folowing function extract values from the ltc2977 following the information in the documentation.

  PLEASE NOTE that these functions applies on the current page
*/
/*--------------------------------------------------------------------------------------------------------*/
int8_t read_VOUT_MODE(uint8_t address) {
  // extract the exponent from the VOUT_MODE value. See the documentation but it is mostly useless in fact since it's hardwired to -13
  uint8_t vout_mode, bits;

  vout_mode = PMBUS_ReadWord(address, VOUT_MODE);
  bits = extract_bits_from_value(vout_mode, 0, 5);
  return two_complement(bits, 5);
}
/*--------------------------------------------------------------------------------------------------------*/
float read_READ_VOUT(uint8_t address) {
  int8_t N = read_VOUT_MODE(address);
  uint16_t Y = PMBUS_ReadWord(address, READ_VOUT);

  return Y * pow(2.0, N);
}
/*--------------------------------------------------------------------------------------------------------*/
float read_READ_VOUT_Values(uint8_t address,uint8_t command) {
  int8_t N = read_VOUT_MODE(address);
  uint16_t Y = PMBUS_ReadWord(address, command);

  return Y * pow(2.0, N);
}

/* 
          Bit operations 
*/ 
uint16_t extract_bits_from_value(uint16_t val, uint8_t start, uint8_t length){
  // extract bit sequence from val from position start to start+length
  
  return (val >> start) & (uint16_t) (pow(2.0, length) -1);
}
// -------
int16_t two_complement(uint16_t val, uint8_t length){
  if (val > (uint16_t) pow(2.0, length -1)){
    return val - (uint16_t) pow(2.0, length);
  } else {
    return val;
  }
}


void print_word_in_bits(uint16_t num)
{
   for(int bit=0;bit<(sizeof(uint16_t) * 8); bit++)
   {
     Serial.print(num & 0x01);
      num = num >> 1;
   }
}
/*
I2C functions
*/

void scanI2C(uint8_t start, uint8_t stop){
  bool is_next = false;
  for (uint8_t address = start; address <= stop ; address++) {
    Wire.beginTransmission(address);
    Wire.write(0);
    if ( Wire.endTransmission()==0){
      Serial.print((is_next)?":":"");
      is_next = true;
      Serial.print(address);
    }
  }
  Serial.println();
}

uint8_t I2C_Send(uint8_t address){
  Wire.beginTransmission(address);
  Wire.write(0);
  return Wire.endTransmission();
}

uint8_t I2C_Write(uint8_t n, char **values){
  if (n > 2){
    uint8_t address = (uint8_t)strtol(values[1], NULL, 0);
    Wire.beginTransmission(address);
    for (uint8_t i = 2 ; i < n ;i++){
       Wire.write((uint8_t)strtol(values[i], NULL, 0));
    }
    return Wire.endTransmission();
  }
}

uint8_t I2C_ReadByte(uint8_t n, char **values){
  uint8_t retval;
  if (n > 2){
    uint8_t addr = (uint8_t)strtol(values[1], NULL, 0);

    Wire.beginTransmission(addr);
    for (uint8_t i = 2 ; i < n ;i++){
       Wire.write((uint8_t)strtol(values[i], NULL, 0));
    }
    Wire.endTransmission(false);
    Wire.requestFrom(addr,(uint8_t) 1);
    if (1 <= Wire.available()){
      retval =  Wire.read();
    }
    return retval;
  }
}

uint16_t I2C_ReadWord(uint8_t n, char **values){
  uint16_t retval, retval2;
  if (n > 2){
    uint8_t addr = (uint8_t)strtol(values[1], NULL, 0);

    Wire.beginTransmission(addr);
    for (uint8_t i = 2 ; i < n ;i++){
       Wire.write((uint8_t) strtol (values[i], NULL, 0));
    }
    Wire.endTransmission(false);
    Wire.requestFrom(addr,(uint8_t) 2);
    if (1 <= Wire.available()){
      retval =  Wire.read();
      retval2 =  Wire.read();
      retval |= retval2 << 8; // shift high byte to be high 8 bits
    }
    return retval;
  }
}

/* 
   PMBUS like function
*/
uint8_t PMBUS_SendByte(uint8_t address, uint8_t command){
  Wire.beginTransmission(address);
  Wire.write(command);              
  return Wire.endTransmission();    
}

// -------

uint8_t PMBUS_WriteByte(uint8_t address, uint8_t command, uint8_t val){
  Wire.beginTransmission(address);
  Wire.write(command);              
  Wire.write(val);              
  return Wire.endTransmission();    
}

// -------

uint8_t PMBUS_WriteWord(uint8_t address, uint8_t command, uint16_t val){
  Wire.beginTransmission(address);
  Wire.write(command);
  Wire.write(lowByte(val));              
  Wire.write(highByte(val));              
  return Wire.endTransmission();    
}

// -------

uint8_t PMBUS_ReadByte(uint8_t address, uint8_t command){
  uint8_t retval;
  
  Wire.beginTransmission(address);
  Wire.write(command);
  Wire.endTransmission(false);  // ? according to https://f-leb.developpez.com/tutoriels/arduino/bus-i2c/ needed argument false for restart    
  Wire.requestFrom(address,(uint8_t) 1);
  if (1 <= Wire.available()){
    retval =  Wire.read();
  }
  return retval;
}

// -------

uint16_t PMBUS_ReadWord(uint8_t address, uint8_t command){
  uint16_t retval;
  Wire.beginTransmission(address);
  Wire.write(command);
  Wire.endTransmission(false);  // ? according to https://f-leb.developpez.com/tutoriels/arduino/bus-i2c/ needed argument false for restart    
  Wire.requestFrom(address,(uint8_t) 2);
  if (1 <= Wire.available()){
    retval  = Wire.read(); // receive low byte as lower 8 bits
    retval |= Wire.read() << 8; // shift high byte to be high 8 bits
  }
  return retval;
}





/* 
  conversion function from float to L11 or L16 values
  function extracted from LinearTechnology code
*/

/*
 * Convert L16 value to float
 *
 * exponent: vout_mode
 * input_value: the value to convert
 * return: converted value
 *
 * The address is used to get the exponent to use
 */
float L16_to_Float_mode(uint8_t vout_mode, uint16_t input_val)
{
  // Assume Linear 16, pull out 5 bits of exponent, and use signed value.
  int8_t exponent = (int8_t) vout_mode & 0x1F;

  // Sign extend exponent from 5 to 8 bits
  if (exponent > 0x0F) exponent |= 0xE0;

  // Convert mantissa to a float so we can do math.
  float mantissa = (float)input_val;

  float value = mantissa * pow(2.0, exponent);

  return  value;
}

/*
 * Convert Float value to L16
 *
 * vout_mode: vout_mode
 * input_value: the value to convert
 * return: converted value
 *
 * The address is used to get the exponent to use
 */
uint16_t Float_to_L16_mode(uint8_t vout_mode, float input_val)
{
  // Assume Linear 16, pull out 5 bits of exponent, and use signed value.
  int8_t exponent = vout_mode & 0x1F;

  // Sign extend exponent from 5 to 8 bits
  if (exponent > 0x0F) exponent |= 0xE0;

  // Scale the value to a mantissa based on the exponent
  uint16_t mantissa = (uint16_t)(input_val / pow(2.0, exponent));

  return mantissa;
}

/*
 * Convert L11 value to float
 *
 * input_value: value to convert
 * return: converted value
 */
float L11_to_Float(uint16_t input_val)
{
  // Extract exponent as MS 5 bits
  int8_t exponent = (int8_t) (input_val >> 11);

  // Extract mantissa as LS 11 bits
  int16_t mantissa = input_val & 0x7ff;

  // Sign extend exponent from 5 to 8 bits
  if (exponent > 0x0F) exponent |= 0xE0;

  // Sign extend mantissa from 11 to 16 bits
  if (mantissa > 0x03FF) mantissa |= 0xF800;

  // Compute value as mantissa * 2^(exponent)
  return  mantissa * pow(2.0,(float)exponent);
}
/*
 * Convert float to L11
 *
 * input_val: the value to convert
 * 
 * return: converted value
 */
uint16_t Float_to_L11(float input_val)
{
  uint16_t uExponent;
  uint16_t uMantissa;

  // Set exponent to -16
  int exponent = -16;

  // Extract mantissa from input value
  float mantissa = (input_val / pow(2.0, exponent));
  // Search for an exponent that produces
  // a valid 11-bit mantissa
      //Serial.println("COMPUTE L11 1");
    //Serial.println(exponent);
    //Serial.println(input_val / pow(2.0, exponent));

  do
  {
    if ((mantissa >= -1024) && (mantissa <= +1023))
    {
      break; // stop if mantissa valid
    }
    exponent++;
    mantissa = (input_val / pow(2.0, exponent));
  }
  while (exponent < +15);

  // Format the exponent of the L11
  uExponent = exponent << 11; // Format the mantissa of the L11
  uMantissa = ((int)mantissa) & 0x07FF;

  // Compute value as exponent | mantissa
  return uExponent | uMantissa;
}
