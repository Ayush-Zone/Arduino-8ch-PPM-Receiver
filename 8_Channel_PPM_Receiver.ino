// 8 Channel PPM Receiver
// Ayush Sharma
// https://github.com/Ayush-Zone



#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// LED pin for indicating data reception
int LedPin = 8;

////////////////////// PPM CONFIGURATION//////////////////////////
#define channel_number 8  //set the number of channels
#define sigPin 2  //set PPM signal output pin on the arduino
#define PPM_FrLen 27000  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PPM_PulseLen 400  //set the pulse length
//////////////////////////////////////////////////////////////////

int ppm[channel_number];

const uint64_t pipeIn =  0xE9E8F0F0E1LL;

RF24 radio(9, 10);

// The sizeof this struct should not exceed 32 bytes
struct MyData {
byte throttle;
byte yaw;
byte pitch;
byte roll;
byte aux1;
byte aux2;
byte aux3;
byte aux4;
};

MyData data;

void resetData() 
{
  // 'safe' values to use when no radio input is detected
  data.throttle = 0;
  data.yaw = 127;
  data.pitch = 127;
  data.roll = 127;
  data.aux1 = 0;
  data.aux2= 0;
  data.aux3 = 0;
  data.aux4= 0;
  
  setPPMValuesFromData();
}

void setPPMValuesFromData()
{
  ppm[0] = map(data.roll, 0, 255, 1000, 2000);
  ppm[1] = map(data.pitch,      0, 255, 1000, 2000);
  ppm[2] = map(data.throttle,    0, 255, 1000, 2000);
  ppm[3] = map(data.yaw,     0, 255, 1000, 2000);
  ppm[4] = map(data.aux1,     0, 1, 1000, 2000);
  ppm[5] = map(data.aux2,     0, 1, 1000, 2000);  
  ppm[6] = map(data.aux3,     0, 1, 1000, 2000);
  ppm[7] = map(data.aux4,     0, 1, 1000, 2000);  
  }

/**************************************************/

void setupPPM() {
  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, 0);  //set the PPM signal pin to the default state (off)

  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;

  OCR1A = 100;  // compare match register (not very important, sets the timeout for the first interrupt)
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();
}

void setup()
{  
  Serial.begin(9600);
  resetData();
  setupPPM();

  pinMode(LedPin, OUTPUT);
  
  // Set up radio module
  radio.begin();
  radio.setDataRate(RF24_250KBPS); // Both endpoints must have this set the same
  radio.setAutoAck(false);
  radio.setCRCLength(RF24_CRC_8);
  radio.setChannel(108);

  radio.openReadingPipe(1,pipeIn);
  radio.startListening();
}

/**************************************************/

unsigned long lastRecvTime = 0;

void recvData()
{  
  while ( radio.available() ) {        
    radio.read(&data, sizeof(MyData));
    lastRecvTime = millis();
    digitalWrite(LedPin, HIGH);
  }
}

/**************************************************/

void loop()
{
  recvData();

  unsigned long now = millis();
  if ( now - lastRecvTime > 1000 ) {
    // signal lost?
    resetData();
    digitalWrite(LedPin, LOW);
  }
  
  
Serial.print("Throttle: "); Serial.print(data.throttle);  Serial.print("    ");
Serial.print("Yaw: ");      Serial.print(data.yaw);       Serial.print("    ");
Serial.print("Pitch: ");    Serial.print(data.pitch);     Serial.print("    ");
Serial.print("Roll: ");     Serial.print(data.roll);      Serial.print("    ");
Serial.print("Aux1: ");     Serial.print(data.aux1);      Serial.print("    ");
Serial.print("Aux2: ");     Serial.print(data.aux2);      Serial.print("    ");
Serial.print("Aux3: ");     Serial.print(data.aux3);      Serial.print("    ");
Serial.print("Aux4: ");     Serial.print(data.aux4);      Serial.print("\n");



  setPPMValuesFromData();
}

/**************************************************/


#define clockMultiplier 2 // set this to 2 if you are using a 16MHz arduino, leave as 1 for an 8MHz arduino

ISR(TIMER1_COMPA_vect){
  static boolean state = true;

  TCNT1 = 0;

  if ( state ) {
    //end pulse
    PORTD = PORTD & ~B00000100; // turn pin 2 off. Could also use: digitalWrite(sigPin,0)
    OCR1A = PPM_PulseLen * clockMultiplier;
    state = false;
  }
  else {
    //start pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;

    PORTD = PORTD | B00000100; // turn pin 2 on. Could also use: digitalWrite(sigPin,1)
    state = true;

    if(cur_chan_numb >= channel_number) {
      cur_chan_numb = 0;
      calc_rest += PPM_PulseLen;
      OCR1A = (PPM_FrLen - calc_rest) * clockMultiplier;
      calc_rest = 0;
    }
    else {
      OCR1A = (ppm[cur_chan_numb] - PPM_PulseLen) * clockMultiplier;
      calc_rest += ppm[cur_chan_numb];
      cur_chan_numb++;
    }     
  }
}
