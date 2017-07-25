#ifdef AVR
#include <SoftwareSerial.h>
#else
#include "wiring_private.h"
#endif

#ifdef AVR
#define BT_RX 14
#define BT_TX 15
#define BT_KEY 9

SoftwareSerial bt(BT_RX, BT_TX);
#else

#define BT_RX 11
#define BT_TX 10
Uart bt(&sercom1, BT_RX, BT_TX, SERCOM_RX_PAD_0, UART_TX_PAD_2);

void SERCOM1_Handler()
{
  bt.IrqHandler();
}

#endif

String btBuf;
bool waitForOk()
{
  int mode = 0;
  while(1) {
    while(bt.available()) {
      char c = bt.read();
      switch(mode) {
        case 0:
          if(c == '\r') {
            mode = 1;
          } else {
            btBuf += c;
          }
          break;
        case 1:
          if(c == '\n') {
            if(btBuf == "OK") {
              btBuf = "";
              return true;
            }
            btBuf = "";
            return false;
          } else {
            mode = 0;
          }
          break;
        default:
          break;
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  while(!Serial.available()) { };
  while(Serial.available()) {
    Serial.read();
  }
  
  Serial.println("HC-05 AT Commander Online");
  
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  
  bt.begin(38400);

  delay(1000);
  #ifdef AVR
  bt.listen();
  #else
  pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(11, PIO_SERCOM);
  #endif
}

String cmdBuf;

void loop()
{
  int sent;
  while(Serial.available()) {
    char c = Serial.read();
    switch(c) {
      case '\n':
        break;
      case '\r':
        cmdBuf += '\r';
        cmdBuf += '\n';
        sent = bt.print(cmdBuf.c_str());
        Serial.print(cmdBuf);
        cmdBuf = "";
        break;
      default:
        cmdBuf += c;
    }
  }

  while(bt.available()) {
    Serial.write(bt.read());
  }
}
