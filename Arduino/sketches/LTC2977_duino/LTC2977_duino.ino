#include <LTC2977_duino.h>

#include <stdint.h>
#include <Wire.h>

// sketch to implement a lightweight PMBUS

/* DEBUGGING */
//#define DEBUG

#if defined(DEBUG)
#define debug(...)   Serial.print( __VA_ARGS__)
#define debugln(...) Serial.println(__VA_ARGS__)
#else
#define debug(...)
#define debugln(...)
#endif
/* DEBUGING PART END */

#define SB 0
#define WB 1
#define WW 2


#define MCP_BASE_I2C 0x20
#define MAX_PAGE 8

// the library is in the libraries directory in ~/Work/SFGD/supervisor
//#include "FEB_lib.h"

void configureLTC(uint8_t ltc_1, uint8_t ltc_2);

bool on_error = false;



void writeToLTC(uint8_t i2c, uint8_t page, uint8_t type, uint8_t cmd_code, uint16_t value){
  // writing to page
  debug("i2c :");
  debug(i2c,HEX);
  debug(" ");
  debug(page,HEX);
  debug(" ");
  debug(type);
  debug(" ");
  debug(cmd_code,HEX);
  debug(" ");
  debug(value,HEX);
  
  PMBUS_WriteByte(i2c, PAGE, page);
  switch(type){
    case SB:
      break;
      
    case WB:
      debug("  WB");
      PMBUS_WriteByte(i2c, cmd_code, value);
      break;
      
    case WW:
       debug("   WW");

      PMBUS_WriteWord(i2c, cmd_code, value);
      break;

    default:
      break;
  }
  debugln("");
}

uint16_t readFromLTC(uint8_t i2c, uint8_t page, uint8_t type, uint8_t cmd_code){
  // reading from page
  uint16_t value;
  debug("i2c :");
  debug(i2c,HEX);
  debug(" ");
  debug(page,HEX);
  debug(" ");
  debug(type);
  debug(" ");
  debug(cmd_code,HEX);
  PMBUS_WriteByte(i2c, PAGE, page);
  switch(type){
    case SB:
      break;
      
    case WB:
      debug("  WB");
      value = (uint16_t)PMBUS_ReadByte(i2c, cmd_code);
      break;
      
    case WW:
       debug("   WW");

      value = PMBUS_ReadWord(i2c, cmd_code);
      break;

    default:
      break;
  }
  debugln("");
  return value;
}


//  query function for debugging during configuration.
void readValues(uint8_t i2c)
{

  debugln("readValues");

  Serial.print("  ltc: "); Serial.print(" 0x"); Serial.println(i2c, HEX);
  Serial.print("    temperature : "); Serial.println(L11_to_Float(PMBUS_ReadWord(i2c, READ_TEMPERATURE_1)));
  Serial.print("    Vin  : "); Serial.println(L11_to_Float(PMBUS_ReadWord(i2c, READ_VIN)));
  for (uint8_t i_p = 0; i_p < MAX_PAGE; i_p++) {
    PMBUS_WriteByte(i2c, PAGE, i_p);
    Serial.print("    page:"); Serial.println(i_p);
    Serial.print("      Vout: "); Serial.println(L16_to_Float_mode(-13, PMBUS_ReadWord(i2c, READ_VOUT)));
    Serial.print("      Vout Margin High: "); Serial.println(L16_to_Float_mode(-13, PMBUS_ReadWord(i2c, VOUT_MARGIN_HIGH)));
    Serial.print("      Vout Margin Low: "); Serial.println(L16_to_Float_mode(-13, PMBUS_ReadWord(i2c, VOUT_MARGIN_LOW)));
  }
  Serial.println("::END");
}

void readTonToff(uint8_t i2c)
{

  debugln("readTonToff");

  Serial.print("  ltc: "); Serial.print(" 0x"); Serial.println(i2c, HEX);
  for (uint8_t i_p = 0; i_p < MAX_PAGE; i_p++) {
    PMBUS_WriteByte(i2c, PAGE, i_p);
    Serial.print("    page:"); Serial.println(i_p);
    Serial.print("      Ton: "); Serial.println(L11_to_Float(PMBUS_ReadWord(i2c, TON_DELAY)));
    Serial.print("      Toff: "); Serial.println(L11_to_Float(PMBUS_ReadWord(i2c, TOFF_DELAY)));
  }
}

void readStatus(uint8_t i2c)
{

  debugln("readStatus");

  Serial.print("  ltc: "); Serial.print(" (0x"); Serial.println(i2c, HEX);
  Serial.print("    STATUS_INPUT: "); Serial.println(PMBUS_ReadByte(i2c, STATUS_INPUT), BIN);
  Serial.print("    STATUS_TEMPERATURE: "); Serial.println(PMBUS_ReadByte(i2c, STATUS_TEMPERATURE), BIN);
  Serial.print("    STATUS_CML: "); Serial.println(PMBUS_ReadByte(i2c, STATUS_CML), BIN);
  for (uint8_t i_p = 0; i_p < MAX_PAGE; i_p++) {
    PMBUS_WriteByte(i2c, PAGE, i_p);

    Serial.print("    page:"); Serial.println(i_p);
    Serial.print("      STATUS_BYTE: "); Serial.println(PMBUS_ReadByte(i2c, STATUS_BYTE), BIN);
    Serial.print("      STATUS_WORD: "); Serial.println(PMBUS_ReadWord(i2c, STATUS_WORD), BIN);
    Serial.print("      STATUS_VOUT: "); Serial.println(PMBUS_ReadByte(i2c, STATUS_VOUT), BIN);
  }
}

uint8_t asels_offset(uint8_t a0 , uint8_t a1) {
  // asel
  //     value 0 logic low
  //     value 1 reserved
  //     value 2 pad is floating
  //     value 3 logic level is high

  if        (a1 == 0 and a0 == 0) {    return 0;
  } else if (a1 == 0 and a0 == 2) {    return 1;
  } else if (a1 == 0 and a0 == 3) {    return 2;
  } else if (a1 == 2 and a0 == 0) {    return 3;
  } else if (a1 == 2 and a0 == 2) {    return 4;
  } else if (a1 == 2 and a0 == 3) {    return 5;
  } else if (a1 == 3 and a0 == 0) {    return 6;
  } else if (a1 == 3 and a0 == 2) {    return 7;
  } else if (a1 == 3 and a0 == 3) {    return 8;
  } else {    return -1;
  }
}


void  getCurrentLTCAddressesOffset(uint8_t r[]) {
  // r should be a 1d array of size 4 and will contain
  //   i2c_address of ltc 1 , offset of ltc 1, i2c_address of ltc 2, offset of ltc 2

  uint8_t i_ltc = 0;
  uint16_t pads;
  uint8_t i2c_base, offset, retval;

  for (uint8_t i_i2c = 0x30; i_i2c < 0x5c; i_i2c++) {
    delay(200);
    Wire.beginTransmission(i_i2c);
    retval = Wire.endTransmission();
    if (retval == 0) {
      i2c_base = PMBUS_ReadByte(i_i2c, MFR_I2C_BASE_ADDRESS);
      pads = PMBUS_ReadWord(i_i2c, MFR_PADS);
      offset = asels_offset(pads >> 6 & 3, pads >> 8 & 3);
      debug("i2c: ");
      debug(i_i2c, HEX);
      debug(" ");
      debug(i2c_base, HEX);
      debug(" ");
      debug(offset, HEX);

      if (i_i2c == i2c_base + offset) {
        debug(" is stored");
        r[i_ltc++] = i_i2c;
        r[i_ltc++] = offset;
      }
      debugln("");
    }
  }
}


int alert_pin = 9;


#define MAX_KEYWORDS 11
char *keywords[MAX_KEYWORDS]= {"i2c_send","i2c_write","i2c_readbyte","i2c_readword","scan","write","read","alert","clear_faults","store","read_values"};

bool checkKeyword(char *kw){
  for (uint8_t i = 0; i<MAX_KEYWORDS ; i++){
    if (strcmp(kw,keywords[i]) == 0){
      return true;
    }
  }
  return false;
}

void remove_spaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}

uint8_t index_char = 0;
bool isRestarted = true;
uint8_t icc,iw;
bool treat_input = false;
#define MAX_COMMANDS 20
char *command[MAX_COMMANDS];
const char * separators = ",:;";

// define the global address for all LTC chip
#define GLOBAL_ADDRESS 0x5b

char c;
char user_input[100];
bool alert_mode = false;
uint8_t  alert_value;

uint8_t getType( char* t){
  if (strcmp(t,"SB")==0){
    return SB;
  } else if (strcmp(t,"WB")==0){
    return WB;
  }  else if (strcmp(t,"WW")==0){
    return WW;
  } else {
    return -1;
  }
}


void setup() {

  //! Initialize the serial port to the PC
  Serial.begin(115200);
  Wire.begin();
  pinMode(alert_pin, INPUT);
}

void loop() {

  if (alert_mode){
    alert_value == digitalRead(alert_pin);
  }
   
  if (Serial.available() > 0) {
    c = Serial.read();
    
    if (c == 10) {  // 10 == linefeed
      user_input[icc++]  = '\0';  
      icc = 0;
      // split the string according to separators
      iw = 0;
      command[iw] = strtok ( user_input, separators );
      while (command[iw] != NULL){
        remove_spaces(command[iw]);
        command[++iw] = strtok ( NULL, separators ); 
      }
      treat_input = true;
    } else {          
      user_input[icc++] = c;
    }
  }

  if (treat_input){
    treat_input = false;

    if (checkKeyword(command[0])){
      /*---------------------------------------------------------------  
       *   keyword : i2c_send
       ---------------------------------------------------------------*/    
      if (strcmp(command[0],"i2c_send") == 0){
        uint8_t i2c =      (uint8_t)strtol(command[1], NULL, 0);
        I2C_Send(i2c);
        Serial.println("ok");
      /*---------------------------------------------------------------  
       *   keyword : i2c_write
       ---------------------------------------------------------------*/    
      } else if (strcmp(command[0],"i2c_write") == 0){
        I2C_Write(iw,command);
        
        Serial.println("ok");
      /*---------------------------------------------------------------
       *   keyword : i2c_readbyte
       ---------------------------------------------------------------*/
      } else if (strcmp(command[0],"i2c_readbyte") == 0){

        Serial.println(I2C_ReadByte(iw,command));

      /*---------------------------------------------------------------
       *   keyword : i2c_readword
       ---------------------------------------------------------------*/
      } else if (strcmp(command[0],"i2c_readword") == 0){
        /* Serial.print(iw);
        for (uint8_t i = 0 ; i < iw ; i++){
          Serial.print(" , ");
          Serial.print(command[i]);
        } 
        Serial.print(" ");*/
        Serial.println(I2C_ReadWord(iw,command));

      /*---------------------------------------------------------------
       *   keyword : write
       ---------------------------------------------------------------*/
      } else if (strcmp(command[0],"write") == 0){

        if (iw == 6){
        
          uint8_t i2c =      (uint8_t)strtol(command[1], NULL, 0); 
          uint8_t page =     (uint8_t)strtol(command[2], NULL, 0);
          uint8_t type = getType(command[3]);
          uint8_t cmd_code = (uint8_t)strtol(command[4], NULL, 0);
          uint16_t value =  (uint16_t)strtol(command[5], NULL, 0);
        
          writeToLTC(i2c, page, type, cmd_code, value);
          Serial.println("ok");
        } else {
          Serial.print("ERR:write:wrong number of args expect 5 got ");
          Serial.println(iw - 1);
        }
      /*---------------------------------------------------------------        
       *   keyword : read
       ---------------------------------------------------------------*/
      } else if (strcmp(command[0],"read") == 0){
        if (iw == 5){
        
          uint8_t i2c =      (uint8_t)strtol(command[1], NULL, 0); 
          uint8_t page =     (uint8_t)strtol(command[2], NULL, 0);
          uint8_t type = getType(command[3]);
          uint8_t cmd_code = (uint8_t)strtol(command[4], NULL, 0);
        
          Serial.println(readFromLTC(i2c, page, type, cmd_code));
        } else {
          Serial.print("ERR:read:wrong number of args expect 4 got ");
          Serial.println(iw - 1);
        }

      /*---------------------------------------------------------------
       *   keyword: scan 
       ---------------------------------------------------------------*/
      } else if (strcmp(command[0],"scan") == 0){
        if (iw == 1){
            scanI2C(0,127);    
        } else if (iw == 2) {
          uint8_t start_addr =      (uint8_t)strtol(command[1], NULL, 0); 
          scanI2C(start_addr, start_addr);
          
        } else if (iw == 3) {
          uint8_t start_addr =      (uint8_t)strtol(command[1], NULL, 0); 
          uint8_t stop_addr =      (uint8_t)strtol(command[2], NULL, 0); 
          scanI2C(start_addr, stop_addr);
          
        } else {
          Serial.println("ERR:scan:too_many_params");
        }
      /*---------------------------------------------------------------
       *   keyword: alert
       ---------------------------------------------------------------*/
      } else if (strcmp(command[0],"alert") == 0){
        if (iw == 1){
          Serial.print("ALERT:");          
          Serial.println((alert_mode) ? "on" : "off" );
        } else if (iw == 2) {
          if (strcmp(command[1], "on") == 0){
            alert_mode = true;
          } else if (strcmp(command[1],"off")==0) {
            alert_mode = false;
          } else {
            Serial.print("ERR:alert:unknown_type:");
            Serial.println(command[1]);
          }    
        } else {
            Serial.print("ERR:alert:too_many_params");
        }
      /*---------------------------------------------------------------
       *   keyword: clear_faults
       ---------------------------------------------------------------*/
      } else if (strcmp(command[0],"clear_faults") == 0){
        PMBUS_WriteByte(GLOBAL_ADDRESS, PAGE, -1);
        // CLEAR_FAULTS
        PMBUS_SendByte(GLOBAL_ADDRESS, 0x03);
        // MFR_FAULT_LOG_CLEAR
        PMBUS_SendByte(GLOBAL_ADDRESS, 0xec);

        Serial.println("ok");
      /*---------------------------------------------------------------
       *   keyword: store
       ---------------------------------------------------------------*/
      } else if (strcmp(command[0],"store") == 0){
        PMBUS_WriteByte(GLOBAL_ADDRESS, PAGE, -1);
        PMBUS_SendByte(GLOBAL_ADDRESS, 0x15);
        Serial.println("ok");
      /*---------------------------------------------------------------
       *   keyword: read_values
       ---------------------------------------------------------------*/      
      } else if (strcmp(command[0],"read_values") == 0){
        if (iw > 1 and iw < MAX_COMMANDS){
          for ( int i = 1 ; i < iw ; i++){ 
            Serial.println(i,HEX);
            readValues((uint8_t)strtol(command[i], NULL, 0));     
          }
        } else {
          Serial.println("ERR:read_values:uncorrect_nb_params");
        }
      }
    } else {
      Serial.print("ERR:unknown_keyword:");
      Serial.println(command[0]);
    }
  }
}
